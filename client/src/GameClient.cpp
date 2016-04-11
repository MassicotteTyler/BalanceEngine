#include <GameClient.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Network/IpAddress.hpp>

#include <iostream>
GameClient::GameClient(std::string ipAddress)
  :_Window(sf::VideoMode(640, 480), "Balance")
  ,_World(_Window)
  ,connected(false)
  ,_HasFocus(true)
  ,_Active(true)
  ,_ClientTimeout(sf::seconds(2.f))
   ,_TimeSinceLastPacket(sf::seconds(0.f))
{
  ip = ipAddress;
  if (_Socket.connect(ip, 5000, sf::seconds(5.f))
      == sf::TcpSocket::Done)
  {
    connected = true;
    std::cout << "Connected to server!" << std::endl;
    sf::Time TimePerFrame = sf::seconds(1.f/60.f);
    while(connected)
    {
      sf::Clock clock;
      sf::Time timeSinceLastUpdate = sf::Time::Zero;
      while(_Window.isOpen())
      {
        sf::Time elaspedTime = clock.restart();
        timeSinceLastUpdate += elaspedTime;
        while (timeSinceLastUpdate > TimePerFrame)
        {
          timeSinceLastUpdate -= TimePerFrame;
          update(TimePerFrame);
        }

        draw();
        std::cout << "Should display" << std::endl;
        _Window.display();
      }
      std::cout << "RIP" << std::endl;

    }
  }
  else
  {
    std::cout << "Could not connect to server" << std::endl;
    _FailedConnectionClock.restart();
  }

  _Socket.setBlocking(true);

}

void GameClient::draw()
{
  if (connected)
  {
    _World.draw();

    _Window.setView(_Window.getDefaultView());

  //  if(!_Broadcasts.empty())
    //  _Window.draw(_BroadcastText);
  }
  //else
    //_Window.draw(_FailedConnectionText);
    
}

void GameClient::onActivate()
{
  _Active = true;
}

void GameClient::onDestroy()
{
  if (connected)
  {
    sf::Packet packet;
    packet << static_cast<sf::Int32>(Client::Quit);
    _Socket.send(packet);
  }
}

bool GameClient::update(sf::Time dt)
{
  if (connected)
  {
    _World.update(dt);

    //Destroy planes

    if (_Active && _HasFocus)
    {
      CommandQueue& commands = _World.getCommandQueue();
      for (auto& pair : _Players)
        pair.second->handleRealtimeInput(commands);
    }

    //Handle network input
    CommandQueue& commands = _World.getCommandQueue();

    //Handle messages from the server
    sf::Packet packet;
    if (_Socket.receive(packet) == sf::Socket::Done)
    {
      _TimeSinceLastPacket = sf::seconds(0.f);
      sf::Int32 packetType;
      packet >> packetType;
      handlePacket(packetType, packet);
    }
    else
    {
      //Check for timeout
      if (_TimeSinceLastPacket > _ClientTimeout)
      {
        connected = false;

        _FailedConnectionText.setString("Lost connection to server");
        centerOrigin(_FailedConnectionText);

        _FailedConnectionClock.restart();
      }
    }

    updateBroadcastMessage(dt);

    GameActions::Action gameAction;
    while (_World.pollGameAction(gameAction))
    {
      sf::Packet packet;
      packet << static_cast<sf::Int32>(Client::GameEvent);
      packet << static_cast<sf::Int32>(gameAction.type);
      packet << gameAction.position.x;
      packet << gameAction.position.y;

      _Socket.send(packet);
    }

    if (_TickClock.getElapsedTime() > sf::seconds(1.f / 20.f))
    {
      sf::Packet positionUpdatePacket;
      positionUpdatePacket <<
        static_cast<sf::Int32>(Client::PositionUpdate);

      for (sf::Int32 identifier : _LocalPlayerIdentifiers)
      {
        if (Aircraft* aircraft = _World.getAircraft(identifier))
          positionUpdatePacket << identifier << aircraft->getPosition().x
            << aircraft->getPosition().y;
      }

      _Socket.send(positionUpdatePacket);
      _TickClock.restart();
    }
    _TimeSinceLastPacket += dt;
  }

  else if (_FailedConnectionClock.getElapsedTime() >= sf::seconds(5.f))
  {
    //Failure and exit
    return false;
  }

  return true;
}

void GameClient::disableAllRealtimeActions()
{
  _Active = false;
  for (sf::Int32 identifier : _LocalPlayerIdentifiers)
    _Players[identifier]->disableAllRealtimeActions();
}

bool GameClient::handleEvent(const sf::Event& event)
{
  //Input handling
  CommandQueue& commands = _World.getCommandQueue();

  for (auto& pair : _Players)
  {
    pair.second->handleEvent(event, commands);
  }

  if (event.type == sf::Event::KeyPressed)
  {

  }
  else if (event.type == sf::Event::GainedFocus)
  {
    _HasFocus = true;
  }
  else if (event.type == sf::Event::LostFocus)
  {
    _HasFocus = false;
  }

  return true;
}

void GameClient::updateBroadcastMessage(sf::Time elapsedTime)
{
  if (_Broadcasts.empty())
    return;

  //Update broadcast timer
  _BroadcastElapsedTime += elapsedTime;
  if (_BroadcastElapsedTime > sf::seconds(2.5f))
  {
    _Broadcasts.erase(_Broadcasts.begin());

    if (!_Broadcasts.empty())
    {
      _BroadcastText.setString(_Broadcasts.front());
      centerOrigin(_BroadcastText);
      _BroadcastElapsedTime = sf::Time::Zero;
    }
  }
}

void GameClient::handlePacket(sf::Int32 packetType, sf::Packet& packet)
{
  switch(packetType)
  {
    std::cout << "Test" << std::endl;
    case Server::BroadcastMessage:
      {
        std::string message;
        packet >> message;
        _Broadcasts.push_back(message);

        if (_Broadcasts.size() == 1)
        {
          _BroadcastText.setString(_Broadcasts.front());
          centerOrigin(_BroadcastText);
          _BroadcastElapsedTime = sf::Time::Zero;
        }
      } break;

    case Server::SpawnPlayer:
      {
        sf::Int32 aircraftIdentifier;
        sf::Vector2f aircraftPosition;
        packet >> aircraftIdentifier >> aircraftPosition.x >>
          aircraftPosition.y;

        Aircraft* aircraft = _World.addAircraft(aircraftIdentifier);
        aircraft->setPosition(aircraftPosition);

        _Players[aircraftIdentifier].reset(new Player(&_Socket,
              aircraftIdentifier));
        _LocalPlayerIdentifiers.push_back(aircraftIdentifier);

        //Gamestarted
      }break;

    case Server::PlayerConnect:
      {
        sf::Int32 aircraftIdentifier;
        sf::Vector2f aircraftPosition;
        packet >> aircraftIdentifier >> aircraftPosition.x
          >> aircraftPosition.y;

        Aircraft* aircraft = _World.addAircraft(aircraftIdentifier);
        aircraft->setPosition(aircraftPosition);

        _Players[aircraftIdentifier].reset(new Player(&_Socket,
              aircraftIdentifier));
      } break;

    case Server::PlayerDisconnect:
      {
        sf::Int32 aircraftIdentifier;
        packet >> aircraftIdentifier;
        _World.removeAircraft(aircraftIdentifier);
        _Players.erase(aircraftIdentifier);
      } break;

    case Server::InitialState:
      {
        sf::Int32 aircraftCount;
        float worldHeight;
        packet >> worldHeight;

        _World.setWorldHeight(worldHeight);

        packet >> aircraftCount;
        for (sf::Int32 i = 0; i < aircraftCount; ++i)
        {
          sf::Int32 aircraftIdentifier;
          sf::Vector2f aircraftPosition;
          packet >> aircraftIdentifier >> aircraftPosition.x >>
            aircraftPosition.y;

          Aircraft* aircraft = _World.addAircraft(aircraftIdentifier);
          aircraft->setPosition(aircraftPosition);

          _Players[aircraftIdentifier].reset(new Player(&_Socket,
                aircraftIdentifier));
        }
      }break;

    case Server::PlayerEvent:
      {
        sf::Int32 aircraftIdentifier;
        sf::Int32 action;
        packet >> aircraftIdentifier >> action;

        auto itr = _Players.find(aircraftIdentifier);
        if (itr != _Players.end())
          itr->second->handleNetworkEvent(
              static_cast<Player::Action>(action), _World.getCommandQueue());
      }break;

    case Server::PlayerRealtimeChange:
      {
        sf::Int32 aircraftIdentifier, action;
        bool actionEnabled;
        packet >> aircraftIdentifier >> action >> actionEnabled;

        auto itr = _Players.find(aircraftIdentifier);
        if (itr != _Players.end())
          itr->second->handleNetworkRealtimeChange(
              static_cast<Player::Action>(action), actionEnabled);
      } break;

    case Server::UpdateClientState:
      {
        float currentWorldPosition;
        sf::Int32 aircraftCount;
        packet >> currentWorldPosition >> aircraftCount;

        //Get view position and set view position


        for (sf::Int32 i = 0; i < aircraftCount; ++i)
        {
          sf::Vector2f aircraftPosition;
          sf::Int32 aircraftIdentifier;
          packet >> aircraftIdentifier >> aircraftPosition.x >>
            aircraftPosition.y;

          Aircraft* aircraft = _World.getAircraft(aircraftIdentifier);
          if(aircraft)
          {
            sf::Vector2f interPosition = aircraft->getPosition() +
              (aircraftPosition - aircraft->getPosition()) * 1.0f;
            aircraft->setPosition(interPosition);
          }
        }
      } break;
  }
}
