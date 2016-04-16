#include <GameClient.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Network/IpAddress.hpp>

#include <iostream>
const sf::Time GameClient::TimePerFrame = sf::seconds(1.f/60.f);

GameClient::GameClient(std::string ipAddress)
  :_Window(sf::VideoMode(640, 480), "Balance")
  ,_World(_Window)
  ,connected(false)
  ,playerReady(false)
  ,worldReady(false)
  ,_HasFocus(true)
  ,_Active(true)
  ,_ClientTimeout(sf::seconds(2.f))
   ,_TimeSinceLastPacket(sf::seconds(0.f))
{
  //Request connection
  //Connect and game Aircraft ID
  //Pass ID and plane to world
  //Get all other aircrafts
  //Loop, update, render
  _Window.setKeyRepeatEnabled(false);
  if (_Socket.connect(ipAddress, ServerPort, sf::seconds(5.f))
          == sf::TcpSocket::Done)
  {
    connected = true;
    while (!playerReady)
    {
      //  _Socket.setBlocking(true);
      sf::Packet packet;
      if (_Socket.receive(packet) == sf::Socket::Done)
      {
        _TimeSinceLastPacket = sf::seconds(0.f);
        sf::Int32 packetType;
        packet >> packetType;
        handlePacket(packetType, packet);
      }

    }
    //_Socket.setBlocking(false);
  }
  else
  {
    std::cout << "Could not connect to server" << std::endl;
    exit(1);
  }

  sf::Clock clock;
  sf::Time timeSinceLastUpdate = sf::Time::Zero;
  while (_Window.isOpen())
  {
    sf::Time elapsedTime = clock.restart();
    timeSinceLastUpdate += elapsedTime;
    while(timeSinceLastUpdate > TimePerFrame)
    {
      timeSinceLastUpdate -= TimePerFrame;
      processInputs();
      update(TimePerFrame);
    }

    draw();
  }
}

void GameClient::processInputs()
{
  CommandQueue& commands = _World.getCommandQueue();

  sf::Event event;
  while(_Window.pollEvent(event))
  {
    if (event.type == sf::Event::Closed)
      _Window.close();
    else
      handleEvent(event);
  }
}
void GameClient::draw()
{
  _Window.clear(sf::Color::Black);
  _World.draw();
  _Window.setView(_Window.getDefaultView());
  _Window.display();
  //_Window.setView(_Window.getDefaultView());

  //  if(!_Broadcasts.empty())
  //  _Window.draw(_BroadcastText);
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
      _Player->handleRealtimeInput(commands);
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

    //updateBroadcastMessage(dt);

    if (_TickClock.getElapsedTime() > sf::seconds(1.f / 20.f))
    {
      sf::Packet positionUpdatePacket;
      positionUpdatePacket <<
        static_cast<sf::Int32>(Client::PositionUpdate);

      Aircraft* aircraft  = _World.getAircraft(_LocalPlayerID);
      if (aircraft)
        return false;
      positionUpdatePacket << _LocalPlayerID << aircraft->getPosition().x
        << aircraft->getPosition().y << aircraft->getRotation();

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
  _Player->disableAllRealtimeActions();
}

bool GameClient::handleEvent(const sf::Event& event)
{
  //Input handling
  CommandQueue& commands = _World.getCommandQueue();


  if (event.type == sf::Event::Closed)
  {
    _Window.close();
  }
  if (event.type == sf::Event::KeyPressed)
  {
    _Player->handleEvent(event, commands);
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

        _Player.reset(new Player(&_Socket, aircraftIdentifier));
        _LocalPlayerID = aircraftIdentifier;
        playerReady = true;
        //Gamestarted
      }break;

    case Server::PlayerConnect:
      {
        sf::Int32 aircraftIdentifier;
        sf::Vector2f aircraftPosition;
        float angle;
        packet >> aircraftIdentifier >> aircraftPosition.x
          >> aircraftPosition.y >> angle;

        Aircraft* aircraft = _World.addAircraft(aircraftIdentifier);
        aircraft->setPosition(aircraftPosition);
        aircraft->setRotation(angle);
      } break;

    case Server::PlayerDisconnect:
      {
        sf::Int32 aircraftIdentifier;
        packet >> aircraftIdentifier;
        _World.removeAircraft(aircraftIdentifier);
      } break;

    case Server::InitialState:
      {
        sf::Int32 aircraftCount;
        packet >> aircraftCount;

        for (sf::Int32 i = 0; i < aircraftCount; ++i)
        {
          sf::Int32 aircraftIdentifier;
          sf::Vector2f aircraftPosition;
          float rotation;

          packet >> aircraftIdentifier >> aircraftPosition.x >>
            aircraftPosition.y >> rotation;

          Aircraft* aircraft = _World.addAircraft(aircraftIdentifier);
          aircraft->setPosition(aircraftPosition);
          aircraft->setRotation(rotation);
        }
      }break;

    case Server::PlayerEvent:
      {
        sf::Int32 aircraftIdentifier;
        sf::Int32 action;
        packet >> aircraftIdentifier >> action;
      }break;

    case Server::PlayerRealtimeChange:
      {
        sf::Int32 aircraftIdentifier, action;
        bool actionEnabled;
        packet >> aircraftIdentifier >> action >> actionEnabled;
      } break;

    case Server::UpdateClientState:
      {
        sf::Int32 aircraftCount;
        packet >> aircraftCount;

        //Get view position and set view position
        for (sf::Int32 i = 0; i < aircraftCount; ++i)
        {
          sf::Vector2f aircraftPosition;
          sf::Int32 aircraftIdentifier;
          float rotation;
          packet >> aircraftIdentifier >> aircraftPosition.x >>
            aircraftPosition.y >> rotation;

          Aircraft* aircraft = _World.getAircraft(aircraftIdentifier);
          if(aircraft)
          {
            sf::Vector2f interPosition = aircraft->getPosition() +
              (aircraftPosition - aircraft->getPosition()) * 1.0f;
            aircraft->setRotation(rotation);
            aircraft->setPosition(interPosition);
          }
        }
      } break;
  }
}
