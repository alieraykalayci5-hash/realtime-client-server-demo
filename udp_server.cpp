#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

#include <iostream>
#include <cstdint>
#include <cstring>
#include <random>
#include <unordered_set>

#pragma comment(lib, "Ws2_32.lib")

#pragma pack(push, 1)
struct PacketHeader {
    uint32_t magic;
    uint16_t type;    // 1=DATA, 2=ACK
    uint16_t reserved;
    uint32_t seq;
};
#pragma pack(pop)

static constexpr uint32_t MAGIC = 0x4A415453; // 'STAJ'
static constexpr uint16_t TYPE_DATA = 1;
static constexpr uint16_t TYPE_ACK  = 2;

int main() {
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
    hints.ai_flags = AI_PASSIVE;

    addrinfo* result = nullptr;
    if (getaddrinfo(nullptr, port, &hints, &result) != 0) {
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

    if (bind(sock, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
        std::cout << "bind failed\n";
        closesocket(sock);
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    freeaddrinfo(result);

    // %30 ACK drop simülasyonu
    std::mt19937 rng{ std::random_device{}() };
    std::uniform_int_distribution<int> dist(1, 100);
    const int dropPercent = 30;

    // Duplicate tespiti için: gördüğümüz seq'leri tutuyoruz (demo için yeterli)
    std::unordered_set<uint32_t> seenSeq;

    std::cout << "[UDP SERVER] Listening on port " << port << "...\n";
    std::cout << "[UDP SERVER] Simulated ACK loss = " << dropPercent << "%\n";

    uint8_t buf[1500];

    while (true) {
        sockaddr_in clientAddr{};
        int clientLen = sizeof(clientAddr);

        int bytes = recvfrom(sock, (char*)buf, sizeof(buf), 0, (sockaddr*)&clientAddr, &clientLen);
        if (bytes == SOCKET_ERROR) continue;
        if (bytes < (int)sizeof(PacketHeader)) continue;

        PacketHeader hdr{};
        std::memcpy(&hdr, buf, sizeof(hdr));
        if (hdr.magic != MAGIC) continue;

        if (hdr.type != TYPE_DATA) continue;

        bool firstTime = (seenSeq.find(hdr.seq) == seenSeq.end());
        if (firstTime) {
            seenSeq.insert(hdr.seq);
            std::cout << "[UDP SERVER] DATA seq=" << hdr.seq << "\n";
        } else {
            std::cout << "[UDP SERVER] DUPLICATE seq=" << hdr.seq << " (retransmit)\n";
        }

        // ACK drop simülasyonu
        int r = dist(rng);
        if (r <= dropPercent) {
            std::cout << "[UDP SERVER] (drop) ACK seq=" << hdr.seq << "\n";
            continue;
        }

        PacketHeader ack{};
        ack.magic = MAGIC;
        ack.type = TYPE_ACK;
        ack.reserved = 0;
        ack.seq = hdr.seq;

        sendto(sock, (const char*)&ack, (int)sizeof(ack), 0, (sockaddr*)&clientAddr, clientLen);
        std::cout << "[UDP SERVER] ACK seq=" << hdr.seq << "\n";
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
