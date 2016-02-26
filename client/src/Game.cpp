#include <Game.hpp>
#include <StringHelpers.hpp>

#include <SFML/Window/Event.hpp>

#define PlayerSpeed 250.f
const sf::Time Game::TimePerFrame = sf::seconds(1.f/60.f);

Game::Game()
  : gWindow(sf::VideoMode(640, 480), "Balance")
  , gWorld(gWindow)
  , font()
  , statisticsText()
  , statisticsUpdateTime()
  , statisticsNumFrames(0)
  {
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
      processEvents();
      update(TimePerFrame);
    }

    updateStatistics(elaspedTime);
    render();
  }
}


void Game::processEvents()
{
  sf::Event event;
  while (gWindow.pollEvent(event))
  {
    switch(event.type)
    {
      case sf::Event::KeyPressed:
        handlePlayerInput(event.key.code, true);
        break;
      case sf::Event::KeyReleased:
        handlePlayerInput(event.key.code, false);
        break;
      case sf::Event::Closed:
        gWindow.close();
        break;
      default:
        break;
    }
  }
}

void Game::handlePlayerInput(sf::Keyboard::Key key, bool isPressed)
{
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
