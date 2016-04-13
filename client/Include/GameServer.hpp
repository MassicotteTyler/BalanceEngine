#ifndef GAMESERVER_HPP
#define GAMESERVER_HPP

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Thread.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/UdpSocket.hpp>

#include <vector>
#include <memory>
#include <map>


class GameServer
{
  public:
    explicit GameServer(sf::Vector2f mapSize);
    ~GameServer();

    void notifyPlayerSpawn(sf::Int32 aircraftIdentifier);
    void notifyPlayerRealtimeChange(sf::Int32 aircraftIdentifier,
        sf::Int32 action, bool actionEnabled);
    void notifyPlayerEvent(sf::Int32 aircraftIdentifier, sf::Int32 action);

  private:
    //Instance of the game
    struct RemotePeer
    {
      RemotePeer();
      sf::TcpSocket socket;
      sf::Time lastPacketTime;
      std::vector<sf::Int32> aircraftIdentifiers;
      bool ready;
      bool timedOut;
    };

    //Store information about current aircraft state
    struct AircraftInfo
    {
      sf::Vector2f position;
      float angle;
      std::map<sf::Int32, bool> realtimeActions;
    };

    typedef std::unique_ptr<RemotePeer> PeerPtr;

  private:
    void setListening(bool enable);
    void executionThread();
    void tick();
    sf::Time now() const;

    void handleIncomingPackets();
    void handleIncomingPacket(sf::Packet& packet, RemotePeer& receivingPeer,
        bool& detectedTimeout);

    void handleIncomingConnections();
    void handleDisconnections();

    void informWorldState(sf::TcpSocket& socket);
    void broadcastMessage(const std::string& message);
    void sendToAll(sf::Packet& packet);
    void updateClientState();

  private:
    sf::Thread _Thread;
    sf::Clock _Clock;
    sf::TcpListener _ListenerSocket;
    bool _ListenerState;
    sf::Time _ClientTimeoutTime;

    std::size_t _MaxConnectedPlayers;
    std::size_t _ConnectedPlayers;

    float _WorldHeight;
    sf::FloatRect _MapRect;
    float _MapScrollSpeed;

    std::size_t _AircraftCount;
    std::map<sf::Int32, AircraftInfo> _AircraftInfo;

    std::vector<PeerPtr> _Peers;
    sf::Int32 _AircraftIdentifierCounter;
    bool _WaitingThreadEnd;

    sf::Time _LastSpawnTime;
    sf::Time _TimeForNextSpawn;
};
#endif
