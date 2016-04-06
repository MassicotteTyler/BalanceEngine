#include <GameClient.hpp>
#include <stdexcept>
#include <iostream>

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: game IP_ADDRESS " << std::endl;
    return(1);
  }
  try
  {
    GameClient gc(argv[1]);
  }
  catch (std::exception& e)
  {
    std::cout << "\nException: " << e.what() << std::endl;
  }
}
