#include "Networking.h"
#include <cstdio>

#include "Client.h"
#include "PacketTypes.h"

udp::socket* Networking::socket;
udp::endpoint Networking::receiverEndpoint;

void Networking::StartThread(const char* ip, const int port, std::string gameName) {
    try {
        boost::asio::io_context io_context;

        udp::resolver resolver(io_context);
        receiverEndpoint = *resolver.resolve(udp::v4(), ip, std::to_string(port)).begin();

        socket = new udp::socket(io_context);
        socket->open(udp::v4());

        std::vector<unsigned char> send_buf(gameName.size() + 1);
        send_buf[0] = C2S_CREATE_GAME;
        std::copy(gameName.begin(), gameName.end(), send_buf.begin() + 1);

        if (gameName.size() >= 5 && gameName[0] == 'j' && gameName[1] == 'o' && gameName[2] == 'i' && gameName[3] == 'n') {
            send_buf[0] = C2S_JOIN_GAME;

            // get int from the rest of the gameName
            const uint32_t gameId = std::stoi(gameName.substr(4));
            const uint32_t netorder = htonl(gameId);
            std::memcpy(&send_buf[1], &netorder, sizeof(netorder));
        }

        socket->send_to(boost::asio::buffer(send_buf), receiverEndpoint);

        std::array<unsigned char, 8192> recv_buf{};
        udp::endpoint sender_endpoint;

        while (!Client::ShouldClose) {
            const size_t len = socket->receive_from(boost::asio::buffer(recv_buf), sender_endpoint);

            if (len == 0) {
                printf("Recieved Empty Packet?\n");
                continue;
            }

            switch (recv_buf[0]) {
            case S2C_UPDATE_DATA:
                    Client::UpdatePixelBuffer(recv_buf);
                    break;

                case S2C_CONNECT_FAIL:
                    Client::ShouldClose = true;
                    break;

                case S2C_GAME_START: {
                    uint32_t network_order;
                    std::memcpy(&network_order, &recv_buf[1], sizeof(network_order));
                    const int code = static_cast<int>(ntohl(network_order));

                    Client::Title = "Code: " + std::to_string(code) + " FPS: ";
                    break;
                }

                default:
                    printf("Invalid packet type recieved!\n");
                    break;
            }
        }

        // send disconnect packet
        send_buf = {C2S_EXIT_GAME};
        socket->send_to(boost::asio::buffer(send_buf), receiverEndpoint);
    } catch (std::exception& e) {
        fprintf(stderr, "Exception: %s\n", e.what());
        Client::ShouldClose = true;
    }
}

void Networking::SendInput(const uint8_t input, const bool pressed) {
    std::array<unsigned char, 3> send_buf = {C2S_SEND_INPUTS, static_cast<uint8_t>(pressed), input};

    socket->send_to(boost::asio::buffer(send_buf), receiverEndpoint);
}
