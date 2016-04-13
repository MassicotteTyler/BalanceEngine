#ifndef GAMECLIENT_HPP
#define GAMECLIENT_HPP

#include <World.hpp>
#include <Player.hpp>
#include <NetworkProtocol.hpp>
#include <Utility.hpp>

#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class GameClient
{
  public:
    GameClient(std::string ipAddress);
    virtual void draw();
    virtual bool update(sf::Time dt);
    virtual bool handleEvent(const sf::Event& event);
    virtual void onActivate();
    void onDestroy();
    void disableAllRealtimeActions();

  private:
    void updateBroadcastMessage(sf::Time elapsedTime);
    void handlePacket(sf::Int32 packetType, sf::Packet& packet);
    void processInputs();

  private:
    typedef std::unique_ptr<Player> PlayerPtr;
    PlayerPtr _Player;
    World _World;
    sf::RenderWindow _Window;
    std::string ip;
    static const sf::Time TimePerFrame;
    //TextureHolder& _TextureHolder;

    //std::map<int, PlayerPtr> _Players;
    sf::Int32 _LocalPlayerID;
    sf::TcpSocket _Socket;
    bool connected;
    bool playerReady;
    bool worldReady;
    sf::Clock _TickClock;

    std::vector<std::string> _Broadcasts;
    sf::Text _BroadcastText;
    sf::Time _BroadcastElapsedTime;

    sf::Text _FailedConnectionText;
    sf::Clock _FailedConnectionClock;

    bool _Active;
    bool _HasFocus;
    sf::Time _ClientTimeout;
    sf::Time _TimeSinceLastPacket;

};
#endif
