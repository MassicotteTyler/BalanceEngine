#ifndef GAME_HPP
#define GAME_HPP

#include <World.hpp>

#include <SFML/System/Time.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class Game
{
  public:
    Game();
    void run();

  private:
    void processEvents();
    void update(sf::Time deltaTime);
    void render();
    void updateStatistics(sf::Time elaspedTime);
    void handlePlayerInput(sf::Keyboard::Key, bool isPressed);

  private:
    static const sf::Time TimePerFrame;

    sf::RenderWindow gWindow;
    World gWorld;

    sf::Font font;
    sf::Text statisticsText;
    sf::Time statisticsUpdateTime;
    std::size_t statisticsNumFrames;
};
#endif
