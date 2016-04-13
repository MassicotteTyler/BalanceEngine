#include <GameServer.hpp>
#include <NetworkProtocol.hpp>
#include <Aircraft.hpp>

#include <SFML/Network/Packet.hpp>

#include <iostream>

GameServer::RemotePeer::RemotePeer()
  :ready(false)
  ,timedOut(false)
{
  socket.setBlocking(false);
}

GameServer::GameServer(sf::Vector2f mapSize)
:_Thread(&GameServer::executionThread, this)
, _ListenerState(false)
, _ClientTimeoutTime(sf::seconds(3.f))
, _MaxConnectedPlayers(10)
, _ConnectedPlayers(0)
, _WorldHeight(5000.f)
, _MapRect(0.f, _WorldHeight - mapSize.y, mapSize.x, mapSize.y)
, _AircraftCount(0)
, _Peers(1)
, _AircraftIdentifierCounter(0)
, _WaitingThreadEnd(false)
, _LastSpawnTime(sf::Time::Zero)
, _TimeForNextSpawn(sf::seconds(5.f))
{
  std::cout << "GameServer created" << std::endl;
  _ListenerSocket.setBlocking(false);
  _Peers[0].reset(new RemotePeer());
   _Thread.launch();
}

GameServer::~GameServer()
{
  _WaitingThreadEnd = true;
  _Thread.wait();
}

void GameServer::notifyPlayerRealtimeChange(sf::Int32 aircraftIdentifier,
    sf::Int32 action, bool actionEnabled)
{
  for (std::size_t i = 0; i < _ConnectedPlayers; ++i)
  {
    if (_Peers[i]->ready)
    {
      sf::Packet packet;
      packet << static_cast<sf::Int32>(Server::PlayerRealtimeChange);
      packet << aircraftIdentifier;
      packet << action;
      packet << actionEnabled;

      _Peers[i]->socket.send(packet);
    }
  }
}

void GameServer::notifyPlayerEvent(sf::Int32 aircraftIdentifier,
    sf::Int32 action)
{
  for (std::size_t i = 0; i < _ConnectedPlayers; ++i)
  {
    if (_Peers[i]->ready)
    {
      sf::Packet packet;
      packet << static_cast<sf::Int32>(Server::PlayerEvent);
      packet << aircraftIdentifier;
      packet << action;

      _Peers[i]->socket.send(packet);
    }
  }
}

void GameServer::notifyPlayerSpawn(sf::Int32 aircraftIdentifier)
{
  for (std::size_t i = 0; i < _ConnectedPlayers; ++i)
  {
    if (_Peers[i]->ready)
    {
      sf::Packet packet;
      packet << static_cast<sf::Int32>(Server::PlayerConnect);
      packet << aircraftIdentifier <<
        _AircraftInfo[aircraftIdentifier].position.x <<
          _AircraftInfo[aircraftIdentifier].position.y <<
            _AircraftInfo[aircraftIdentifier].angle;
      _Peers[i]->socket.send(packet);
    }
  }
}

void GameServer::setListening(bool enable)
{
  if (enable)
  {
    if (!_ListenerState)
      _ListenerState = (_ListenerSocket.listen(ServerPort) ==
          sf::TcpListener::Done);
  }
  else
  {
    _ListenerSocket.close();
    _ListenerState = false;
  }
}

void GameServer::executionThread()
{
  setListening(true);

  sf::Time stepInterval = sf::seconds(1.f / 60.f);
  sf::Time stepTime = sf::Time::Zero;
  sf::Time tickInterval = sf::seconds(1.f / 20.f);
  sf::Time tickTime = sf::Time::Zero;
  sf::Clock stepClock, tickClock;

  while(_WaitingThreadEnd)
  {
    handleIncomingPackets();
    handleIncomingConnections();

    stepTime += stepClock.getElapsedTime();
    stepClock.restart();

    tickTime += tickClock.getElapsedTime();
    tickClock.restart();

    while(stepTime >= stepInterval)
    {
      //do something
      stepTime -= stepInterval;
    }

    while (tickTime >= tickInterval)
    {
      tick();
      tickTime -= tickInterval;
    }

    //To prevent 100% cpu usage
    sf::sleep(sf::milliseconds(100));
  }
}

void GameServer::tick()
{
  updateClientState();

  bool allAircraftsDone = true;

  //Detect collision and destroy

  //Respawn after destruction
}

sf::Time GameServer::now() const
{
  return _Clock.getElapsedTime();
}

void GameServer::handleIncomingPackets()
{
  bool detectedTimeout = false;

  for (PeerPtr& peer : _Peers)
  {
    if (peer->ready)
    {
      sf::Packet packet;
      if (peer->socket.receive(packet) == sf::Socket::Done)
      {
        std::cout << "Handle clients packet" << std::endl;
        //Interpret packet and react to it
        handleIncomingPacket(packet, *peer, detectedTimeout);

        //Packet was recieved, update ping timer
        peer->lastPacketTime = now();
        packet.clear();
      }

      if (now() >= peer->lastPacketTime + _ClientTimeoutTime)
      {
        peer->timedOut = true;
        detectedTimeout = true;
      }
    }
  }
  if (detectedTimeout)
    handleDisconnections();
}


void GameServer::handleIncomingPacket(sf::Packet& packet,
    RemotePeer& receivingPeer, bool& detectedTimeout)
{
  sf::Int32 packetType;
  packet >> packetType;
  switch (packetType)
  {
    case Client::Quit:
      {
        receivingPeer.timedOut = true;
        detectedTimeout = true;
      } break;
    case Client::PlayerEvent:
      {
        sf::Int32 aircraftIdentifier, action;
        packet >> aircraftIdentifier >> action;
      } break;

    case Client::PlayerRealtimeChange:
      {
        sf::Int32 aircraftIdentifier, action;
        bool actionEnabled;
        packet >> aircraftIdentifier >> action >> actionEnabled;
        _AircraftInfo[aircraftIdentifier].realtimeActions[action] =
          actionEnabled;
        notifyPlayerRealtimeChange(aircraftIdentifier, action,
            actionEnabled);
      } break;

    case Client::RequestConnection:
      {
        std::cout << "Client trying to connect" << std::endl;
        receivingPeer.aircraftIdentifiers.push_back(
            _AircraftIdentifierCounter);
        _AircraftInfo[_AircraftIdentifierCounter].position =
          sf::Vector2f(0, 0);

        sf::Packet requestPacket;
        requestPacket << static_cast<sf::Int32>(Server::PlayerConnect);
        requestPacket <<
          _AircraftInfo[_AircraftIdentifierCounter].position.x;
        requestPacket <<
          _AircraftInfo[_AircraftIdentifierCounter].position.y;

        receivingPeer.socket.send(requestPacket);
        _AircraftCount++;

        //Inform all other peers of the new plane
        for (PeerPtr& peer : _Peers)
        {
          if (peer.get() != &receivingPeer && peer->ready)
          {
            sf::Packet notifyPacket;
            notifyPacket <<
              static_cast<sf::Int32>(Server::PlayerConnect);
            notifyPacket << _AircraftIdentifierCounter;
            notifyPacket <<
              _AircraftInfo[_AircraftIdentifierCounter].position.x;
            notifyPacket <<
              _AircraftInfo[_AircraftIdentifierCounter].position.y;
            peer->socket.send(notifyPacket);
          }
        }
        _AircraftIdentifierCounter++;
      } break;

    case Client::PositionUpdate:
      {
        sf::Int32 aircraftIdentifier;
        sf::Vector2f aircraftPosition;
        float aircraftAngle;
        packet >> aircraftIdentifier >> aircraftPosition.x >>
          aircraftPosition.y >> aircraftAngle;
        _AircraftInfo[aircraftIdentifier].position = aircraftPosition;
        _AircraftInfo[aircraftIdentifier].angle = aircraftAngle;
      } break;

    case Client::GameEvent:
      {
        sf::Int32 action;
        float x, y;

        packet >> action >> x >> y;
        //Handle actions
      } break;
  }
}

void GameServer::updateClientState()
{
  sf::Packet updatePacket;
  updatePacket << static_cast<sf::Int32>(_AircraftInfo.size());

  for (auto aircraft : _AircraftInfo)
  {
    updatePacket << aircraft.first << aircraft.second.position.x <<
      aircraft.second.position.y << aircraft.second.angle;
  }

  sendToAll(updatePacket);
}

void GameServer::handleIncomingConnections()
{
  if (!_ListenerState)
    return;

  if (_ListenerSocket.accept(_Peers[_ConnectedPlayers]->socket)
      == sf::TcpListener::Done)
  {
    std::cout << "Incoming connection" << std::endl;
    _AircraftInfo[_AircraftIdentifierCounter].position =
      sf::Vector2f(0, 0);

    sf::Packet packet;
    packet << static_cast<sf::Int32>(Server::SpawnPlayer);
    packet << _AircraftIdentifierCounter;
    packet << _AircraftInfo[_AircraftIdentifierCounter].position.x;
    packet << _AircraftInfo[_AircraftIdentifierCounter].position.y;

    _Peers[_ConnectedPlayers]->aircraftIdentifiers.push_back(
        _AircraftIdentifierCounter);

    //broadcastMessage("New Player!");
    informWorldState(_Peers[_ConnectedPlayers]->socket);
    notifyPlayerSpawn(_AircraftIdentifierCounter++);

    _Peers[_ConnectedPlayers]->socket.send(packet);
    _Peers[_ConnectedPlayers]->ready = true;
    _Peers[_ConnectedPlayers]->lastPacketTime = now();

    _AircraftCount++;
    _ConnectedPlayers++;

    std::cout << "New player joined" << std::endl;
    if (_ConnectedPlayers >= _MaxConnectedPlayers)
      setListening(false);
    else
      _Peers.push_back(PeerPtr(new RemotePeer()));
    _ListenerSocket.setBlocking(false);
  }

}

void GameServer::handleDisconnections()
{
  std::cout << "Handle disconnect" << std::endl;
  for (auto itr = _Peers.begin(); itr != _Peers.end(); )
  {
    if ((*itr)->timedOut)
    {
      //Inform other players of the disconnect, erase
      for (sf::Int32 identifier : (*itr)->aircraftIdentifiers)
      {
        sendToAll(sf::Packet() << static_cast<sf::Int32>
            (Server::PlayerDisconnect) << identifier);
        _AircraftInfo.erase(identifier);
      }

      _ConnectedPlayers--;
      _AircraftCount -= (*itr)->aircraftIdentifiers.size();

      itr = _Peers.erase(itr);

      //Listen state if needed
      if (_ConnectedPlayers < _MaxConnectedPlayers)
      {
        _Peers.push_back(PeerPtr(new RemotePeer()));
        setListening(true);
      }

     // broadcastMessage("Player has disconnected.");
    }
    else
    {
      ++itr;
    }
  }
}

void GameServer::informWorldState(sf::TcpSocket& socket)
{
  sf::Packet packet;
  packet << static_cast<sf::Int32>(Server::InitialState);
  packet << static_cast<sf::Int32>(_AircraftCount);

  for (std::size_t i = 0; i < _ConnectedPlayers; ++i)
  {
    if (_Peers[i]->ready)
    {
      for (sf::Int32 identifier : _Peers[i]->aircraftIdentifiers)
      {
        packet << identifier << _AircraftInfo[identifier].position.x <<
          _AircraftInfo[identifier].position.y <<
            _AircraftInfo[identifier].angle;
      }
    }
  }
    socket.send(packet);
}

void GameServer::broadcastMessage(const std::string& message)
{
  for (std::size_t i = 0; i < _ConnectedPlayers; ++i)
  {
    if (_Peers[i]->ready)
    {
      sf::Packet packet;
      packet << static_cast<sf::Int32>(Server::BroadcastMessage);
      packet << message;

      _Peers[i]->socket.send(packet);
    }
  }
}

void GameServer::sendToAll(sf::Packet& packet)
{
  for (PeerPtr& peer : _Peers)
  {
    if (peer->ready)
    {
      peer->socket.send(packet);
    }
  }
}
