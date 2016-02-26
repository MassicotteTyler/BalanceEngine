#include <World.hpp>

#include <SFML/Graphics/RenderWindow.hpp>

World::World(sf::RenderWindow& window)
: _Window(window)
, _WorldView(window.getDefaultView())
, _Textures()
, _SceneGraph()
, _SceneLayers()
, _WorldBounds(0.f, 0.f, _WorldView.getSize().x, 2000.f)
, _SpawnPosition(_WorldView.getSize().x / 2.f,
    _WorldBounds.height - _WorldView.getSize().y / 2.f)
, _ScrollSpeed(-50.f)
, _PlayerAircraft(nullptr)
{
  loadTextures();
  buildScene();

  _WorldView.setCenter(_SpawnPosition);
}

void World::update(sf::Time dt)
{
  _WorldView.move(0.f, _ScrollSpeed * dt.asSeconds());

  sf::Vector2f position = _PlayerAircraft->getPosition();
  sf::Vector2f velocity = _PlayerAircraft->getVelocity();

  if (position.x <= _WorldBounds.left + 150.f
      || position.x >= _WorldBounds.left + _WorldBounds.width - 150.f)
  {
    velocity.x = -velocity.x;
    _PlayerAircraft->setVelocity(velocity);
  }

  _SceneGraph.update(dt);
}

void World::draw()
{
  _Window.setView(_WorldView);
  _Window.draw(_SceneGraph);
}

void World::loadTextures()
{
  _Textures.load(Textures::Eagle, "assets/textures/Eagle.png");
  _Textures.load(Textures::Desert, "assets/textures/Desert.png");
  _Textures.load(Textures::Raptor, "assets/textures/Raptor.png");
}
void World::buildScene()
{
  //Init the different layers
  for (std::size_t i = 0; i < LayerCount; i++)
  {
    SceneNode::Ptr layer(new SceneNode());
    _SceneLayers[i] = layer.get();

    _SceneGraph.attachChild(std::move(layer));
  }

  //tiled background
  sf::Texture& texture = _Textures.get(Textures::Desert);
  sf::IntRect textureRect(_WorldBounds);
  texture.setRepeated(true);

  //Add background to scene
  std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(texture,
        textureRect));
  backgroundSprite->setPosition(_WorldBounds.left, _WorldBounds.top);
  _SceneLayers[Background]->attachChild(std::move(backgroundSprite));

  //Add player's aircraft
  std::unique_ptr<Aircraft> leader(new Aircraft(Aircraft::Eagle,
        _Textures));
  _PlayerAircraft = leader.get();
  _PlayerAircraft->setPosition(_SpawnPosition);
  _PlayerAircraft->setVelocity(40.f, _ScrollSpeed);
  _SceneLayers[Air]->attachChild(std::move(leader));

  //two escort aircrafts
  std::unique_ptr<Aircraft> leftEscort(new Aircraft(Aircraft::Raptor,
        _Textures));
  leftEscort->setPosition(-80.f, 50.f);
  _PlayerAircraft->attachChild(std::move(leftEscort));

  std::unique_ptr<Aircraft> rightEscort(new Aircraft(Aircraft::Raptor,
        _Textures));
  rightEscort->setPosition(80.f, 50.f);
  _PlayerAircraft->attachChild(std::move(rightEscort));
}
