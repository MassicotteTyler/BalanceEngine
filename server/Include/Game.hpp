#ifndef GAME_HPP
#define GAME_HPP

#include <World.hpp>
#include <Player.hpp>

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
    void processInputs();
    void update(sf::Time deltaTime);
    void render();
    void updateStatistics(sf::Time elaspedTime);

  private:
    static const sf::Time TimePerFrame;

    sf::RenderWindow gWindow;
    World gWorld;
    Player _Player;

    sf::Font font;
    sf::Text statisticsText;
    sf::Time statisticsUpdateTime;
    std::size_t statisticsNumFrames;
};
#endif
