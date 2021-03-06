#include <Player.hpp>
#include <CommandQueue.hpp>
#include <Aircraft.hpp>

#include <map>
#include <string>
#include <algorithm>
#include <iostream>
#include <math.h>

#define ROTATE_SPEED 3.f

struct AircraftMover
{
  AircraftMover(float vx, float vy, int nDirection)
    :velocity(vx, vy),
    direction(nDirection)
  {
  }

  void operator() (Aircraft& aircraft, sf::Time) const
  {
    aircraft.rotate(ROTATE_SPEED * direction);
    aircraft.accelerate(velocity);
  }

  sf::Vector2f velocity;
  int direction;
};

Player::Player()
{
  _KeyBinding[sf::Keyboard::Left] = MoveLeft;
  _KeyBinding[sf::Keyboard::Right] = MoveRight;
  _KeyBinding[sf::Keyboard::Up] =  MoveUp;
  _KeyBinding[sf::Keyboard::Down] = MoveDown;

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
    if (found != _KeyBinding.end() && !isRealtimeAction(found->second))
      commands.push(_ActionBinding[found->second]);
  }
}

void Player::handleRealtimeInput(CommandQueue& commands)
{
  for (auto pair: _KeyBinding)
  {
    if (sf::Keyboard::isKeyPressed(pair.first)
        && isRealtimeAction(pair.second))
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
    derivedAction<Aircraft>(AircraftMover(0.f, 0.f, -1));
  _ActionBinding[MoveRight].action =
    derivedAction<Aircraft>(AircraftMover(0.f, 0.f, 1));
  _ActionBinding[MoveUp].action =
    derivedAction<Aircraft>(AircraftMover(0.f, 0.f, 0));
  _ActionBinding[MoveDown].action =
    derivedAction<Aircraft>(AircraftMover(0.f, 0.f, 0));
}

bool Player::isRealtimeAction(Action action)
{
  switch(action)
  {
    case MoveLeft:
    case MoveRight:
    case MoveDown:
    case MoveUp:
      return true;
    default:
      return false;
  }
}
