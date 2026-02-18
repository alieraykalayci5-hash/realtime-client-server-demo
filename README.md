# Realtime Client-Server Networking Demo (C++)

A performance-focused C++ networking project implementing:

- Multi-threaded TCP server
- Custom Reliable UDP layer (built from scratch)
- Sliding Window protocol
- Packet loss simulation
- Timeout-based retransmission
- Duplicate detection
- RTT measurement

Built using MSVC (Winsock) on Windows.

============================================================

1) TCP Layer

Features:
- Line-based protocol (HELLO / PING-PONG)
- Multi-threaded server (thread-per-client)
- RTT measurement
- Sequential load testing (50+ clients tested)

Localhost Results:
RTT ≈ 0–1 ms (avg ~0.4–0.5 ms)

Architecture:
Client <-> Threaded TCP Server
Blocking sockets
Text-based protocol

============================================================

2) Reliable UDP Layer (Custom Protocol)

Reliability implemented on top of UDP:

- Custom PacketHeader structure
- Sequence numbers
- ACK packets
- Simulated ACK loss (30%)
- Timeout-based retransmission
- Duplicate packet detection
- Sliding Window transmission
- Performance statistics output

Protocol Header:

struct PacketHeader {
    uint32_t magic;
    uint16_t type;     // 1=DATA, 2=ACK
    uint16_t reserved;
    uint32_t seq;
};

Magic value: 'STAJ'

============================================================

3) Sliding Window Test Configuration

Total packets: 200
Window size: 16
ACK loss simulation: 30%
Timeout: 200ms

Example Result:

[UDP CLIENT] DONE: delivered 200 packets
[UDP CLIENT] stats: sends=289 retries=89 elapsed=5794ms

Result:
All 200 packets were successfully delivered despite simulated packet loss.

============================================================

4) Build Instructions (Windows - MSVC)

Option A: Use build script (recommended)

Just run:

build_all.bat

This compiles:
- server.exe
- client.exe
- udp_server.exe
- udp_client.exe

------------------------------------------------------------

Option B: Manual Build

Open x64 Developer Command Prompt:

"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"

Navigate to project folder:

cd /d C:\Users\AliEray\Desktop\realtime-client-server-demo

Compile TCP:

cl /EHsc server.cpp /Fe:server.exe
cl /EHsc client.cpp /Fe:client.exe

Compile UDP:

cl /EHsc udp_server.cpp /Fe:udp_server.exe
cl /EHsc udp_client.cpp /Fe:udp_client.exe

============================================================

5) Run Instructions

TCP:

run_tcp.bat

OR manually:

Terminal 1:
server.exe

Terminal 2:
client.exe

------------------------------------------------------------

UDP (Reliable Sliding Window):

run_udp.bat

OR manually:

Terminal 1:
udp_server.exe

Terminal 2:
udp_client.exe

============================================================

6) Engineering Concepts Demonstrated

- Low-level socket programming (Winsock)
- Reliable transmission over unreliable transport (UDP)
- Sliding Window protocol design
- Loss simulation and retransmission strategies
- Duplicate detection
- RTT measurement and timeout handling
- Multi-threaded server design
- Systems-level performance thinking

============================================================

7) Production-Ready Utilities

Included helper scripts:

- build_all.bat     → compile all components
- run_tcp.bat       → launch TCP test
- run_udp.bat       → launch UDP reliability test
- clean.bat         → remove .exe and .obj files

============================================================

8) Future Extensions (Advanced)

- RTT-based adaptive timeout (RTO calculation)
- Dynamic congestion window
- Congestion control simulation
- Cross-platform (Linux / POSIX sockets)
- Benchmark automation
- CI pipeline integration

============================================================

Author:
Ali Eray Kalaycı
Computer Engineering Student
Focused on Real-Time Systems & Networking
