#ifndef AIRCRAFT_HPP
#define AIRCRAFT_HPP
#include <Entity.hpp>
#include <ResourceIdentifiers.hpp>

#include <SFML/Graphics/Sprite.hpp>

class Aircraft : public Entity
{
  public:
    enum Type
    {
      Eagle,
      Raptor,
    };

  public:
    explicit Aircraft(Type type, const TextureHolder& textures);
    virtual void drawCurrent(sf::RenderTarget& target,
                             sf::RenderStates states) const;
    virtual unsigned int getCategory() const;
    void setIdentifier(int identifier);
    int getIdentifier();

  private:
    Type _Type;
    sf::Sprite _Sprite;
    int _Identifier;
};
#endif
