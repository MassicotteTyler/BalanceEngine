#ifndef WORLD_HPP
#define WORLD_HPP

#include <ResourceHolder.hpp>
#include <ResourceIdentifiers.hpp>
#include <SceneNode.hpp>
#include <SpriteNode.hpp>
#include <Aircraft.hpp>
#include <Command.hpp>
#include <CommandQueue.hpp>
#include <NetworkProtocol.hpp>
#include <NetworkNode.hpp>

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

#include <array>
#include <queue>


namespace sf
{
  class RenderTarget;
  class RenderTexture;
}

class World : private sf::NonCopyable
{
  public:
    explicit World(sf::RenderTarget& window);
    void update(sf::Time dt);
    void draw();
    CommandQueue& getCommandQueue();
    Aircraft* addAircraft(int identifier);
    void removeAircraft(int identifier);
    void setWorldHeight(float height);

    Aircraft* getAircraft(int identifier) const;
    sf::FloatRect getWorldBounds() const;
    bool pollGameAction(GameActions::Action& out);

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
    sf::RenderTarget& _Window;
    sf::View _WorldView;
    sf::RenderTexture sceneTex;
    TextureHolder _Textures;

    SceneNode _SceneGraph;
    std::array<SceneNode*, LayerCount> _SceneLayers;
    CommandQueue _CommandQueue;

    sf::FloatRect _WorldBounds;
    sf::Vector2f _SpawnPosition;
    float _ScrollSpeed;
    NetworkNode* _NetworkNode;

    Aircraft* _PlayerAircraft;
    sf::Int32 _LocalPlayerID;
    std::vector<Aircraft*> _PlayerAircrafts;
};
#endif
