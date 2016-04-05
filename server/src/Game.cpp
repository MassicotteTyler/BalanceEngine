#include <Game.hpp>
#include <StringHelpers.hpp>

#include <SFML/Window/Event.hpp>

#define PlayerSpeed 250.f
const sf::Time Game::TimePerFrame = sf::seconds(1.f/60.f);

Game::Game()
  : gWindow(sf::VideoMode(640, 480), "Balance")
  , gWorld(gWindow)
  , font()
  , _Player()
  , statisticsText()
  , statisticsUpdateTime()
  , statisticsNumFrames(0)
  {
    gWindow.setKeyRepeatEnabled(false);

    font.loadFromFile("assets/fonts/Sansation.ttf");
    statisticsText.setFont(font);
    statisticsText.setPosition(5.f, 5.f);
    statisticsText.setCharacterSize(10);
  }

void Game::run()
{
  sf::Clock clock;
  sf::Time timeSinceLastUpdate = sf::Time::Zero;
  while(gWindow.isOpen())
  {
    sf::Time elaspedTime = clock.restart();
    timeSinceLastUpdate += elaspedTime;
    while (timeSinceLastUpdate > TimePerFrame)
    {
      timeSinceLastUpdate -= TimePerFrame;
      processInputs();
      update(TimePerFrame);
    }

    updateStatistics(elaspedTime);
    render();
  }
}


void Game::processInputs()
{
  CommandQueue& commands = gWorld.getCommandQueue();

  sf::Event event;
  while (gWindow.pollEvent(event))
  {
    _Player.handleEvent(event, commands);

    if (event.type == sf::Event::Closed)
      gWindow.close();
  }
  _Player.handleRealtimeInput(commands);
}

void Game::update(sf::Time elaspedTime)
{
  gWorld.update(elaspedTime);
}

void Game::render()
{
  gWindow.clear();
  gWorld.draw();

  gWindow.setView(gWindow.getDefaultView());
  gWindow.draw(statisticsText);
  gWindow.display();
}

void Game::updateStatistics(sf:: Time elaspedTime)
{
  statisticsUpdateTime += elaspedTime;
  statisticsNumFrames += 1;

  if (statisticsUpdateTime >= sf::seconds(1.0f))
  {
    statisticsText.setString(
        "Frames / Second = " + toString(statisticsNumFrames) + "\n" +
        "Time / Update = " + toString(statisticsUpdateTime.asMicroseconds() / statisticsNumFrames) + "us");
    statisticsUpdateTime -= sf::seconds(1.0f);
    statisticsNumFrames = 0;
  }
}
