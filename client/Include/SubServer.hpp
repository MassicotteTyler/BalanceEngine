#ifndef SUBSERVER_HPP
#define SUBSERVER_PP

#include <NetworkProtocol.hpp>
#include <Command.hpp>
#include <CommandQueue.hpp>
#include <World.hpp>

#include <vector>

class SubServer
{
  public:
    void run();
  private:
    void receivePackets();
    void sendPackets(sf::Packet& packet);
    void handlePacket(sf::Packet& packet, RemotePeer& receivingPeer,
        bool& detectedTimeout);

  private:
    struct RemotePeer
    {
      RemotePeer();
      sf::TcpSocket socket;
      std::vector<sf::Int32> aircraftIdentifiers;
      bool ready;
      bool timedOut;
    };
    struct AircraftInfo
    {
      sf::Vector2f position;
      float angle;
      std::map<stf::Int32, bool> realtimeActions;
    };

    typedef std::unique_ptr<RemotePeer> PeetPtr;
    std::vector<PeerPtr> _Peers; 

    sf::TcpSocket t_socket;
    sf::UdpSocket u_socket;
}
#endif
