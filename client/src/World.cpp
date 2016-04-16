#include <World.hpp>

#include <SFML/Graphics/RenderWindow.hpp>

World::World(sf::RenderTarget& window)
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
  ,_PlayerAircrafts()
{
  sceneTex.create(640, 480);
  loadTextures();
  buildScene();
  _WorldView.setCenter(0, 0);

//  _WorldView.setCenter(_SpawnPosition);
}

void World::update(sf::Time dt)
{
  //_WorldView.move(0.f, _ScrollSpeed * dt.asSeconds());
  //_WorldView.setCenter(_PlayerAircraft->getPosition());
  //_PlayerAircraft->setVelocity(0.f, 0.f);

  for (Aircraft* a : _PlayerAircrafts)
    a->setVelocity(0.f, 0.f);

  while(!_CommandQueue.isEmpty())
    _SceneGraph.onCommand(_CommandQueue.pop(), dt);
  adaptPlayerVelocity();

  _SceneGraph.update(dt);
//  adaptPlayerPosition();
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
  _Textures.load(Textures::Map, "assets/textures/world_large.png");
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
  sf::Texture& texture = _Textures.get(Textures::Map);
  sf::IntRect textureRect(0, 0, 6338, 3553);
  texture.setRepeated(true);

  //Add background to scene
  std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(texture,
        textureRect));
  //backgroundSprite->setPosition(_WorldBounds.left, _WorldBounds.top);

  backgroundSprite->setPosition(0, 0);
  _SceneLayers[Background]->attachChild(std::move(backgroundSprite));

}

void World::adaptPlayerPosition()
{
  sf::FloatRect viewBounds(_WorldView.getCenter() - _WorldView.getSize() / 2.f,
      _WorldView.getSize());
  const float borderDistance = 40.f;
}

void World::adaptPlayerVelocity()
{
  //_PlayerAircraft->accelerate(_ScrollSpeed, _ScrollSpeed);
  for (Aircraft* a : _PlayerAircrafts)
    a->accelerate(_ScrollSpeed, _ScrollSpeed);
}

CommandQueue& World::getCommandQueue()
{
  return _CommandQueue;
}

bool World::pollGameAction(GameActions::Action& out)
{
  return _NetworkNode->pollGameAction(out);
}

Aircraft* World::getAircraft(int identifier) const
{
  for (Aircraft* a : _PlayerAircrafts)
  {
    if (a->getIdentifier() == identifier)
      return a;
  }
  return nullptr;
}

Aircraft* World::addAircraft(int identifier)
{
  std::unique_ptr<Aircraft> player(
      new Aircraft(Aircraft::Eagle, _Textures));
    //Todo make random spawn
  player->setPosition(_WorldView.getCenter());
  player->setIdentifier(identifier);

  _PlayerAircrafts.push_back(player.get());
  _SceneLayers[Air]->attachChild(std::move(player));
  return _PlayerAircrafts.back();
}

void World::removeAircraft(int identifier)
{
  Aircraft* aircraft = getAircraft(identifier);
  if (aircraft)
  {
    //aircraft->destroy();
    _PlayerAircrafts.erase(std::find(_PlayerAircrafts.begin(),
          _PlayerAircrafts.end(), aircraft));
  }
}

void World::setWorldHeight(float height)
{
  _WorldBounds.height = height;
}
