#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

#include <iostream>
#include <cstdint>
#include <cstring>

#pragma comment(lib, "Ws2_32.lib")

#pragma pack(push, 1)
struct PacketHeader {
    uint32_t magic;
    uint16_t type;     // 1=DATA, 2=ACK
    uint16_t reserved;
    uint32_t seq;
};
#pragma pack(pop)

static constexpr uint32_t MAGIC = 0x4A415453; // 'STAJ'
static constexpr uint16_t TYPE_DATA = 1;
static constexpr uint16_t TYPE_ACK  = 2;

int main() {
    const char* host = "127.0.0.1";
    const char* port = "27016";

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cout << "WSAStartup failed\n";
        return 1;
    }

    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    addrinfo* result = nullptr;
    if (getaddrinfo(host, port, &hints, &result) != 0) {
        std::cout << "getaddrinfo failed\n";
        WSACleanup();
        return 1;
    }

    SOCKET sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock == INVALID_SOCKET) {
        std::cout << "socket failed\n";
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // ACK bekleme timeout: 200ms
    DWORD timeoutMs = 200;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeoutMs, sizeof(timeoutMs));

    const int maxRetries = 10;

    std::cout << "[UDP CLIENT] Sending 10 packets with retransmit...\n";

    for (uint32_t seq = 1; seq <= 10; seq++) {
        PacketHeader pkt{};
        pkt.magic = MAGIC;
        pkt.type = TYPE_DATA;
        pkt.reserved = 0;
        pkt.seq = seq;

        bool acked = false;

        for (int attempt = 1; attempt <= maxRetries; attempt++) {
            sendto(sock, (const char*)&pkt, (int)sizeof(pkt), 0, result->ai_addr, (int)result->ai_addrlen);
            std::cout << "[UDP CLIENT] sent seq=" << seq << " attempt=" << attempt << "\n";

            PacketHeader ack{};
            sockaddr_in from{};
            int fromLen = sizeof(from);

            int bytes = recvfrom(sock, (char*)&ack, (int)sizeof(ack), 0, (sockaddr*)&from, &fromLen);
            if (bytes == SOCKET_ERROR) {
                std::cout << "[UDP CLIENT] timeout waiting ACK for seq=" << seq << "\n";
                continue; // retry
            }

            if (bytes == sizeof(PacketHeader) && ack.magic == MAGIC && ack.type == TYPE_ACK && ack.seq == seq) {
                std::cout << "[UDP CLIENT] got ACK seq=" << ack.seq << "\n";
                acked = true;
                break;
            } else {
                std::cout << "[UDP CLIENT] invalid ACK packet\n";
            }
        }

        if (!acked) {
            std::cout << "[UDP CLIENT] FAILED seq=" << seq << " (no ACK after retries)\n";
        }
    }

    closesocket(sock);
    freeaddrinfo(result);
    WSACleanup();
    return 0;
}
