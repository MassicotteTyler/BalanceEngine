#include <Entity.hpp>
#include <iostream>

void Entity::setVelocity(sf::Vector2f nVelocity)
{
  velocity = nVelocity;
}

void Entity::setVelocity(float vx, float vy)
{
  velocity.x = vx;
  velocity.y = vy;
}

sf::Vector2f Entity::getVelocity() const
{
  return velocity;
}

void Entity::updateCurrent(sf::Time dt)
{
  float rotation = (this->getRotation() * M_PI) / 180.f;
  float x = -1 * velocity.x * sin(rotation) * 10.f;
  float y = velocity.y * cos(rotation) * 10.f;
  sf::Vector2f vec(x,y);
  move(vec * dt.asSeconds());
}

void Entity::accelerate(sf::Vector2f nVelocity)
{
  velocity += nVelocity;
}

void Entity::accelerate(float vx, float vy)
{
  velocity.x += vx;
  velocity.y += vy;
}
