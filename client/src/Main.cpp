#include <GameClient.hpp>
#include <GameServer.hpp>
#include <stdexcept>
#include <iostream>

int main(int argc, char *argv[])
{
  try
  {
    if (argc < 2)
    {
      GameServer gs(sf::Vector2f(3000, 3000));
    }
    else
    {
      GameClient gc(argv[1]);
    }
  }
  catch (std::exception& e)
  {
    std::cout << "\nException: " << e.what() << std::endl;
  }
}
