//#include <Game.hpp>
#include <GameServer.hpp>

#include <stdexcept>
#include <iostream>

int main()
{
  try
  {
    //Game game;
    //game.run();
    GameServer server(sf::Vector2f(3000,3000));

  }
  catch (std::exception& e)
  {
    std::cout << "\nException: " << e.what() << std::endl;
  }
}
