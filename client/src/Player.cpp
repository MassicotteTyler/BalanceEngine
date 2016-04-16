#include <Player.hpp>
#include <CommandQueue.hpp>
#include <Aircraft.hpp>
#include <NetworkProtocol.hpp>

#include <SFML/Network/Packet.hpp>
#include <map>
#include <string>
#include <algorithm>
#include <iostream>
#include <math.h>

#define ROTATE_SPEED 3.f

struct AircraftMover
{
  AircraftMover(int nDirection, int identifier)
    :direction(nDirection)
    ,aircraftID(identifier)
  {
  }

  void operator() (Aircraft& aircraft, sf::Time) const
  {
    aircraft.rotate(ROTATE_SPEED * direction);
    aircraft.accelerate(0, 0);
    std::cout << "Player moving " << direction << std::endl;
  }
  int direction;
  int aircraftID;
};

Player::Player(sf::TcpSocket* socket, sf::Int32 id)
:_Socket(socket)
,_Identifier(id)
{
  _KeyBinding[sf::Keyboard::Left] = MoveLeft;
  _KeyBinding[sf::Keyboard::Right] = MoveRight;

  initializeActions();

  for (auto& pair : _ActionBinding)
    pair.second.category = Category::PlayerAircraft;
}

void Player::handleEvent(const sf::Event& event, CommandQueue& commands)
{
  if (event.type == sf::Event::KeyPressed)
  {
    //Check bindings for key pressed
    auto found = _KeyBinding.find(event.key.code);
    if (found != _KeyBinding.end())
      commands.push(_ActionBinding[found->second]);
  }
}

void Player::handleRealtimeInput(CommandQueue& commands)
{
  for (auto pair: _KeyBinding)
  {
    if (sf::Keyboard::isKeyPressed(pair.first))
      commands.push(_ActionBinding[pair.second]);

  }
}

void Player::assignKey(Action action, sf::Keyboard::Key key)
{
  //Remove keys mapped to action
  for (auto itr = _KeyBinding.begin(); itr != _KeyBinding.end(); itr++)
  {
    if (itr->second == action)
      _KeyBinding.erase(itr);
  }

  _KeyBinding[key] = action;
}

sf::Keyboard::Key Player::getAssignedKey(Action action) const
{
  for (auto pair : _KeyBinding)
  {
    if (pair.second == action)
      return pair.first;
  }

  return sf::Keyboard::Unknown;
}

void Player::initializeActions()
{
  const float playerSpeed = 200.f;

  //Refactor
  _ActionBinding[MoveLeft].action =
    derivedAction<Aircraft>(AircraftMover(-1, _Identifier));
  _ActionBinding[MoveRight].action =
    derivedAction<Aircraft>(AircraftMover(1, _Identifier));
}

bool Player::isRealtimeAction(Action action)
{
  switch(action)
  {
    case MoveLeft:
    case MoveRight:
      return true;
    default:
      return false;
  }
}

void Player::disableAllRealtimeActions()
{
  for (auto& action : _ActionProxies)
  {
    sf::Packet packet;
    packet << static_cast<sf::Int32>(Client::PlayerRealtimeChange);
    packet << _Identifier;
    packet << static_cast<sf::Int32>(action.first);
    packet << false;
    _Socket->send(packet);
  }
}

void Player::handleNetworkEvent(Action action,
    CommandQueue& commands)
{
  commands.push(_ActionBinding[action]);
}

void Player::handleNetworkRealtimeChange(Action action,
    bool actionEnabled)
{
  _ActionProxies[action] = actionEnabled;
}
