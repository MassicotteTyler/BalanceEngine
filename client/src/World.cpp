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
  //_WorldView.move(0.f, _ScrollSpeed * dt.asSeconds());
  _WorldView.setCenter(_PlayerAircraft->getPosition());
  _PlayerAircraft->setVelocity(0.f, 0.f);

  while(!_CommandQueue.isEmpty())
    _SceneGraph.onCommand(_CommandQueue.pop(), dt);
  adaptPlayerVelocity();

  _SceneGraph.update(dt);
  adaptPlayerPosition();
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

  /*
  //two escort aircrafts
  std::unique_ptr<Aircraft> leftEscort(new Aircraft(Aircraft::Raptor,
        _Textures));
  leftEscort->setPosition(-80.f, 50.f);
  _PlayerAircraft->attachChild(std::move(leftEscort));

  std::unique_ptr<Aircraft> rightEscort(new Aircraft(Aircraft::Raptor,
        _Textures));
  rightEscort->setPosition(80.f, 50.f);
  _PlayerAircraft->attachChild(std::move(rightEscort));
  */
}

void World::adaptPlayerPosition()
{
  sf::FloatRect viewBounds(_WorldView.getCenter() - _WorldView.getSize() / 2.f,
      _WorldView.getSize());
  const float borderDistance = 40.f;

  sf::Vector2f position = _PlayerAircraft->getPosition();
  position.x = std::max(position.x, viewBounds.left + borderDistance);
  position.x = std::min(position.x, viewBounds.left + viewBounds.left +
      viewBounds.width - borderDistance);
  position.y = std::max(position.y, viewBounds.top + borderDistance);
  position.y = std::min(position.y, viewBounds.top +
      viewBounds.height - borderDistance);
  _PlayerAircraft->setPosition(position);
}

void World::adaptPlayerVelocity()
{
  sf::Vector2f velocity = _PlayerAircraft->getVelocity();

  //Reduce velocity for diag movement
  if (velocity.x != 0.f && velocity.y != 0.f)
    _PlayerAircraft->setVelocity(velocity / std::sqrt(2.f));

  //Add scrolling velocity
  _PlayerAircraft->accelerate(0.f, _ScrollSpeed);
}

CommandQueue& World::getCommandQueue()
{
  return _CommandQueue;
}
