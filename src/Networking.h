#ifndef CLIENT_H
#define CLIENT_H

#include <cstdarg>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

class Networking {
public:
    static void StartThread(const char* ip, int port, std::string gameName);
    static void SendInput(uint8_t input, bool pressed);

private:
    static udp::socket* socket;
    static udp::endpoint receiverEndpoint;
};

#endif //CLIENT_H
