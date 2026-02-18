#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

#include <iostream>
#include <string>
#include <chrono>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

static uint64_t now_ms() {
    using namespace std::chrono;
    return (uint64_t)duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

static bool send_all(SOCKET s, const char* data, int len) {
    int sent = 0;
    while (sent < len) {
        int n = send(s, data + sent, len - sent, 0);
        if (n <= 0) return false;
        sent += n;
    }
    return true;
}

static bool send_line(SOCKET s, const std::string& line) {
    std::string msg = line;
    msg.push_back('\n');
    return send_all(s, msg.c_str(), (int)msg.size());
}

static bool recv_line(SOCKET s, std::string& outLine, size_t maxLen = 2048) {
    outLine.clear();
    outLine.reserve(128);

    while (outLine.size() < maxLen) {
        char ch;
        int n = recv(s, &ch, 1, 0);
        if (n == 0) return false;
        if (n < 0) return false;
        if (ch == '\n') return true;
        if (ch != '\r') outLine.push_back(ch);
    }
    return false;
}

static bool starts_with(const std::string& s, const char* prefix) {
    size_t i = 0;
    while (prefix[i] != '\0') {
        if (i >= s.size() || s[i] != prefix[i]) return false;
        ++i;
    }
    return true;
}

int main() {
    const char* host = "127.0.0.1";
    const char* port = "27015";

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cout << "WSAStartup failed\n";
        return 1;
    }

    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

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

    if (connect(sock, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
        std::cout << "connect failed\n";
        closesocket(sock);
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    freeaddrinfo(result);

    // 1) HELLO
    send_line(sock, "HELLO AliEray");
    {
        std::string line;
        if (!recv_line(sock, line)) {
            std::cout << "server closed\n";
            closesocket(sock);
            WSACleanup();
            return 1;
        }
        std::cout << "[CLIENT] " << line << "\n";
    }

    // 2) 10 kez PING/PONG RTT ölç
    const int count = 10;
    uint64_t minRtt = (uint64_t)-1;
    uint64_t maxRtt = 0;
    uint64_t sumRtt = 0;

    for (int i = 1; i <= count; i++) {
        uint64_t t0 = now_ms();
        send_line(sock, "PING " + std::to_string(t0));

        std::string line;
        if (!recv_line(sock, line)) {
            std::cout << "server closed\n";
            break;
        }

        if (starts_with(line, "PONG ")) {
            uint64_t sentTs = 0;
            try {
                sentTs = std::stoull(line.substr(5));
            } catch (...) {
                std::cout << "[CLIENT] invalid pong: " << line << "\n";
                continue;
            }

            uint64_t rtt = now_ms() - sentTs;
            std::cout << "[CLIENT] RTT #" << i << " = " << rtt << " ms\n";

            if (rtt < minRtt) minRtt = rtt;
            if (rtt > maxRtt) maxRtt = rtt;
            sumRtt += rtt;
        } else {
            std::cout << "[CLIENT] Unexpected: " << line << "\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    if (minRtt != (uint64_t)-1) {
        double avg = (double)sumRtt / (double)count;
        std::cout << "[CLIENT] RTT stats: min=" << minRtt
                  << "ms max=" << maxRtt
                  << "ms avg=" << avg << "ms\n";
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
