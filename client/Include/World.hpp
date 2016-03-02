#ifndef WORLD_HPP
#define WORLD_HPP

#include <ResourceHolder.hpp>
#include <ResourceIdentifiers.hpp>
#include <SceneNode.hpp>
#include <SpriteNode.hpp>
#include <Aircraft.hpp>
#include <Command.hpp>
#include <CommandQueue.hpp>


#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <array>
#include <queue>


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
    CommandQueue& getCommandQueue();

  private:
    void loadTextures();
    void buildScene();
    void adaptPlayerPosition();
    void adaptPlayerVelocity();

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
    CommandQueue _CommandQueue;

    sf::FloatRect _WorldBounds;
    sf::Vector2f _SpawnPosition;
    float _ScrollSpeed;
    Aircraft* _PlayerAircraft;
};
#endif
