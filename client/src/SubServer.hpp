#include <SubServer.hpp>


SubServer::SubServer()
{

}

SubServer::run()
{
  while(true)
  {
    receivePackets();
    sendPackets();
  }
}

SubServer::receivePackets()
{

}


void SubServer::handleIncomingPacket(sf::Packet& packet,
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

void SubServer::sendPackets(sf::Packet& packet)
{
  for (PeerPtr& peer : _Peers)
  {
    if (peer->ready)
    {
      peer->socket.send(packet);
    }
  }
}
