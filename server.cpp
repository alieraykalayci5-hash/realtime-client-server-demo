#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <cctype>

#pragma comment(lib, "Ws2_32.lib")

static uint64_t now_ms() {
    using namespace std::chrono;
    return (uint64_t)duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

// TCP'de "mesaj sınırı" olmadığı için satır tabanlı framing yapıyoruz: '\n' görünce mesaj tamam.
// Bu fonksiyon, bir satırı (newline hariç) okur. Hata/kapama olursa false döner.
static bool recv_line(SOCKET s, std::string& outLine, size_t maxLen = 2048) {
    outLine.clear();
    outLine.reserve(128);

    while (outLine.size() < maxLen) {
        char ch;
        int n = recv(s, &ch, 1, 0);
        if (n == 0) return false;          // bağlantı kapandı
        if (n < 0) return false;           // hata
        if (ch == '\n') return true;       // satır bitti
        if (ch != '\r') outLine.push_back(ch);
    }
    // Çok uzun satır => protokol ihlali gibi düşün
    return false;
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

static bool starts_with(const std::string& s, const char* prefix) {
    size_t i = 0;
    while (prefix[i] != '\0') {
        if (i >= s.size() || s[i] != prefix[i]) return false;
        ++i;
    }
    return true;
}

static void handle_client(SOCKET clientSock, int clientNumber) {
    std::cout << "[SERVER] Client #" << clientNumber << " connected.\n";

    // Client ile sürekli konuş (birden fazla mesaj)
    while (true) {
        std::string line;
        if (!recv_line(clientSock, line)) {
            std::cout << "[SERVER] Client #" << clientNumber << " disconnected.\n";
            break;
        }

        std::cout << "[SERVER] Client #" << clientNumber << " says: " << line << "\n";

        // Protokol:
        // PING <timestamp_ms>  => PONG <timestamp_ms>
        // HELLO <text>         => ACK <text>
        // MSG <text>           => ACK <text>
        if (starts_with(line, "PING ")) {
            // Timestamp'i olduğu gibi geri yolluyoruz (RTT client'ta ölçülecek)
            std::string ts = line.substr(5);
            // Basit doğrulama: sadece rakam olsun (opsiyonel)
            bool ok = !ts.empty();
            for (char c : ts) if (!std::isdigit((unsigned char)c)) { ok = false; break; }

            if (ok) {
                send_line(clientSock, "PONG " + ts);
            } else {
                send_line(clientSock, "ERR invalid ping");
            }
        } else if (starts_with(line, "HELLO ")) {
            send_line(clientSock, "ACK " + line.substr(6));
        } else if (starts_with(line, "MSG ")) {
            send_line(clientSock, "ACK " + line.substr(4));
        } else {
            // Bilinmeyen komut
            send_line(clientSock, "ERR unknown command");
        }
    }

    closesocket(clientSock);
}

int main() {
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
    hints.ai_flags = AI_PASSIVE;

    addrinfo* result = nullptr;
    if (getaddrinfo(nullptr, port, &hints, &result) != 0) {
        std::cout << "getaddrinfo failed\n";
        WSACleanup();
        return 1;
    }

    SOCKET listenSock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listenSock == INVALID_SOCKET) {
        std::cout << "socket failed\n";
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    if (bind(listenSock, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
        std::cout << "bind failed\n";
        closesocket(listenSock);
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    freeaddrinfo(result);

    if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "listen failed\n";
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }

    std::cout << "[SERVER] Threaded server listening on port " << port << "...\n";
    std::cout << "[SERVER] Line protocol: HELLO/MSG/PING\n";
    std::cout << "[SERVER] Press Ctrl+C to stop.\n";

    std::atomic<int> clientCounter{0};

    while (true) {
        SOCKET clientSock = accept(listenSock, nullptr, nullptr);
        if (clientSock == INVALID_SOCKET) {
            std::cout << "[SERVER] accept failed\n";
            break;
        }

        int id = ++clientCounter;
        std::thread t(handle_client, clientSock, id);
        t.detach();
    }

    closesocket(listenSock);
    WSACleanup();
    std::cout << "[SERVER] Shutdown.\n";
    return 0;
}
