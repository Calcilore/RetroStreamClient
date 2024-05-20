#include <cstdio>
#include <bits/std_thread.h>

#include "Client.h"
#include "Networking.h"

int main(const int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s <ip> <port> <game name>", argv[0]);
        return -1;
    }

    const char* ip = argv[1];
    const int port = static_cast<int>(strtol(argv[2], nullptr, 0));
    const char* name = argv[3];

    std::thread clientThread = std::thread([ip, port, name]() -> void { Networking::StartThread(ip, port, name); });
    Client::Init();
    return 0;
}
