#ifndef SERVERPACKETTYPE_H
#define SERVERPACKETTYPE_H

enum ServerPacketType {
    C2S_CREATE_GAME,
    C2S_EXIT_GAME,
    C2S_SEND_INPUTS,
    C2S_JOIN_GAME,
    C2S_KEEPALIVE
};

enum ClientPacketType {
    S2C_UPDATE_DATA,
    S2C_CONNECT_FAIL,
    S2C_GAME_START
};

#endif //SERVERPACKETTYPE_H
