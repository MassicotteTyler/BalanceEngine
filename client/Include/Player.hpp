#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <Command.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Window/Event.hpp>
#include <map>

class CommandQueue;

class Player
{
  public:
    enum Action
    {
      MoveLeft,
      MoveRight,
      MoveUp,
      MoveDown,
      ActionCount
    };

  public:
    Player(sf::TcpSocket* socket, sf::Int32 id);
    void handleEvent(const sf::Event& event, CommandQueue& commands);
    void handleRealtimeInput(CommandQueue& commands);
    void handleRealtimeNetworkInput(CommandQueue& commands);

    void handleNetworkEvent(Action action, CommandQueue& commands);
    void handleNetworkRealtimeChange(Action action, bool actionEnabled);

    void disableAllRealtimeActions();

    void assignKey(Action action, sf::Keyboard::Key key);
    sf::Keyboard::Key getAssignedKey(Action action) const;

  private:
    void initializeActions();
    static bool isRealtimeAction(Action action);

  private:
    std::map<sf::Keyboard::Key, Action> _KeyBinding;
    std::map<Action, Command> _ActionBinding;
    std::map<Action, bool> _ActionProxies;
    int _Identifier;
    sf::TcpSocket* _Socket;
    
};

#endif
