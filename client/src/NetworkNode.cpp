#include <NetworkNode.hpp>
#include <NetworkProtocol.hpp>

NetworkNode::NetworkNode()
  :SceneNode()
  ,_PendingActions()
{

}


unsigned int NetworkNode::getCategory() const
{
  return Category::Network;
}

void NetworkNode::notifyGameAction(GameActions::Type type,
    sf::Vector2f position)
{
  _PendingActions.push(GameActions::Action(type, position));
}

bool NetworkNode::pollGameAction(GameActions::Action& out)
{
  if (_PendingActions.empty())
  {
    return false;
  }
  else
  {
    out = _PendingActions.front();
    _PendingActions.pop();
    return true;
  }
}
