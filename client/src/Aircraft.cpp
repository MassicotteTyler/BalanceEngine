#include <Aircraft.hpp>
#include <ResourceHolder.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

Textures::ID toTextureID(Aircraft::Type type)
{
  switch(type)
  {
    case Aircraft::Eagle:
      return Textures::Eagle;
    case Aircraft::Raptor:
      return Textures::Raptor;
  }

  return Textures::Eagle;
}

Aircraft::Aircraft(Type type, const TextureHolder& textures)
: _Type(type)
, _Sprite((sf::Texture&)textures.get(toTextureID(type)))
{
  _Sprite.setTexture(textures.get(toTextureID(type)));
  sf::FloatRect bounds = _Sprite.getLocalBounds();
  _Sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
}

void Aircraft::drawCurrent(sf::RenderTarget& target,
                           sf::RenderStates states) const
{
  target.draw(_Sprite, states);
}

unsigned int Aircraft::getCategory() const
{
  switch(_Type)
  {
    case Eagle:
      return Category::PlayerAircraft;
      break;
    default:
      return Category::EnemyAircraft;
  }
}

void Aircraft::setIdentifier(int identifier)
{
  _Identifier = identifier;
}

int Aircraft::getIdentifier()
{
  return _Identifier;
}
