#ifndef NETWORKPROTOCOL_HPP
#define NETWORKPROTOCOL_HPP

#include <SFML/Config.hpp>
#include <SFML/System/Vector2.hpp>

const unsigned short ServerPort = 5000;

namespace Server
{
  enum PacketType
  {
    BroadcastMessage,
    SpawnPlayer,
    InitialState,
    PlayerEvent,
    PlayerRealtimeChange,
    PlayerConnect,
    PlayerDisconnect,
    UpdateClientState
  };
}

namespace Client
{
  enum PacketType
  {
    PlayerEvent,
    PlayerRealtimeChange,
    PositionUpdate,
    RequestConnection,
    GameEvent,
    Quit
  };
}

namespace PlayerActions
{
  enum Action
  {
    TurnLeft,
    TurnRight,
    ActionCount
  };
}

namespace GameActions
{
  enum Type
  {
    EnemyExplode,
  };

  struct Action
  {
    Action()
    {
    }

    Action(Type type, sf::Vector2f position)
      :type(type)
      , position(position)
    {
    }

    Type type;
    sf::Vector2f position;
  };
}
#endif
