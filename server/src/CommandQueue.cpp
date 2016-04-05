#include <CommandQueue.hpp>
#include <SceneNode.hpp>

void CommandQueue::push(const Command& command)
{
  _Queue.push(command);
}

Command CommandQueue::pop()
{
  Command command = _Queue.front();
  _Queue.pop();
  return command;
}

bool CommandQueue::isEmpty() const
{
  return _Queue.empty();
}
