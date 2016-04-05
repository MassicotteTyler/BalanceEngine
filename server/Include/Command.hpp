#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <Category.hpp>
#include <SFML/System/Time.hpp>
#include <functional>
#include <cassert>

class SceneNode;

struct Command
{
  std::function<void(SceneNode&, sf::Time)> action;
  Command();
  unsigned int category;
};

template <typename GameObject, typename Function>
std::function<void(SceneNode&, sf::Time)> derivedAction(Function fn)
{
  return [=] (SceneNode& node, sf::Time dt)
  {
    //Check if cast is safe
    assert(dynamic_cast<GameObject*>(&node) != nullptr);

    fn(static_cast<GameObject&>(node), dt);
  };
}
#endif
