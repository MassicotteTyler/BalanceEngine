#ifndef WORLD_HPP
#define WORLD_HPP

#include <ResourceHolder.hpp>
#include <ResourceIdentifiers.hpp>
#include <SceneNode.hpp>
#include <SpriteNode.hpp>
#include <Aircraft.hpp>

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <array>

namespace sf
{
  class RenderWindow;
}

class World : private sf::NonCopyable
{
  public:
    explicit World(sf::RenderWindow& window);
    void update(sf::Time dt);
    void draw();

  private:
    void loadTextures();
    void buildScene();

  private:
    enum Layer
    {
      Background,
      Air,
      LayerCount
    };

  private:
    sf::RenderWindow& _Window;
    sf::View _WorldView;
    TextureHolder _Textures;

    SceneNode _SceneGraph;
    std::array<SceneNode*, LayerCount> _SceneLayers;

    sf::FloatRect _WorldBounds;
    sf::Vector2f _SpawnPosition;
    float _ScrollSpeed;
    Aircraft* _PlayerAircraft;
};
#endif
