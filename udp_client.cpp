#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>

#include <iostream>
#include <cstdint>
#include <cstring>
#include <vector>
#include <chrono>
#include <thread>

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

using Clock = std::chrono::steady_clock;

static uint64_t ms_since(const Clock::time_point& t0) {
    return (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t0).count();
}

int main() {
    const char* host = "127.0.0.1";
    const char* port = "27016";

    const uint32_t TOTAL_PACKETS = 200;   // kaç paket göndereceğiz
    const uint32_t WINDOW_SIZE   = 16;    // sliding window
    const uint32_t TIMEOUT_MS    = 200;   // paket timeout -> retransmit

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

    // ACK okumak için küçük timeout (poll gibi davranacağız)
    DWORD rcvTimeoutMs = 50;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&rcvTimeoutMs, sizeof(rcvTimeoutMs));

    // Her seq için durum
    std::vector<bool> acked(TOTAL_PACKETS + 1, false);
    std::vector<Clock::time_point> lastSent(TOTAL_PACKETS + 1);

    uint32_t base = 1;            // en eski ACK beklenen seq
    uint32_t nextToSend = 1;      // sıradaki gönderilecek seq

    uint64_t totalSends = 0;
    uint64_t totalRetries = 0;

    auto start = Clock::now();

    auto send_data = [&](uint32_t seq) {
        PacketHeader pkt{};
        pkt.magic = MAGIC;
        pkt.type = TYPE_DATA;
        pkt.reserved = 0;
        pkt.seq = seq;

        int sent = sendto(sock, (const char*)&pkt, (int)sizeof(pkt), 0, result->ai_addr, (int)result->ai_addrlen);
        if (sent != SOCKET_ERROR) {
            lastSent[seq] = Clock::now();
            totalSends++;
        }
    };

    std::cout << "[UDP CLIENT] Sliding window start\n";
    std::cout << "  total=" << TOTAL_PACKETS
              << " window=" << WINDOW_SIZE
              << " timeout=" << TIMEOUT_MS << "ms\n";

    while (base <= TOTAL_PACKETS) {
        // 1) Window'u doldur: base..base+W-1 aralığında nextToSend'i gönder
        while (nextToSend <= TOTAL_PACKETS && nextToSend < base + WINDOW_SIZE) {
            send_data(nextToSend);
            nextToSend++;
        }

        // 2) ACK oku (varsa)
        PacketHeader ack{};
        sockaddr_in from{};
        int fromLen = sizeof(from);

        int bytes = recvfrom(sock, (char*)&ack, (int)sizeof(ack), 0, (sockaddr*)&from, &fromLen);
        if (bytes != SOCKET_ERROR) {
            if (bytes == sizeof(PacketHeader) && ack.magic == MAGIC && ack.type == TYPE_ACK) {
                uint32_t seq = ack.seq;
                if (seq >= 1 && seq <= TOTAL_PACKETS) {
                    acked[seq] = true;
                }
            }
        }

        // 3) base'i ilerlet (arka arkaya ack'lenenleri geç)
        while (base <= TOTAL_PACKETS && acked[base]) {
            if (base % 25 == 0) {
                std::cout << "[UDP CLIENT] delivered up to seq=" << base
                          << " elapsed=" << ms_since(start) << "ms\n";
            }
            base++;
        }

        // 4) Timeout kontrolü: window içindeki ack beklenen paketleri yeniden yolla
        uint32_t windowEnd = std::min(TOTAL_PACKETS, base + WINDOW_SIZE - 1);
        for (uint32_t seq = base; seq <= windowEnd; seq++) {
            if (!acked[seq]) {
                uint64_t age = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - lastSent[seq]).count();
                // lastSent[seq] default olabilir; ilk kez gönderildiyse zaten set oluyor
                if (lastSent[seq].time_since_epoch().count() != 0 && age >= TIMEOUT_MS) {
                    send_data(seq);
                    totalRetries++;
                }
            }
        }

        // CPU yakmasın
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    uint64_t elapsed = ms_since(start);
    std::cout << "[UDP CLIENT] DONE: delivered " << TOTAL_PACKETS << " packets\n";
    std::cout << "[UDP CLIENT] stats: sends=" << totalSends
              << " retries=" << totalRetries
              << " elapsed=" << elapsed << "ms\n";

    closesocket(sock);
    freeaddrinfo(result);
    WSACleanup();
    return 0;
}
