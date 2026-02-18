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

------------------------------------------------------------

## 1️⃣ TCP Layer

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

------------------------------------------------------------

## 2️⃣ Reliable UDP Layer (Custom Protocol)

Implemented reliability on top of UDP:

- PacketHeader structure
- Sequence numbers
- ACK packets
- Simulated ACK loss (30%)
- Timeout-based retransmission
- Duplicate packet detection
- Sliding Window transmission
- Performance statistics

Protocol Header:

struct PacketHeader {
    uint32_t magic;
    uint16_t type;     // 1=DATA, 2=ACK
    uint16_t reserved;
    uint32_t seq;
};

Magic value: 'STAJ'

------------------------------------------------------------

## 3️⃣ Sliding Window Test Configuration

Total packets: 200  
Window size: 16  
ACK loss simulation: 30%  
Timeout: 200ms  

Example Result:

[UDP CLIENT] DONE: delivered 200 packets  
[UDP CLIENT] stats: sends=289 retries=89 elapsed=5794ms  

All packets delivered successfully despite simulated packet loss.

------------------------------------------------------------

## 4️⃣ Build Instructions (Windows - MSVC)

Open x64 Developer Command Prompt:

"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"

Then navigate to project folder:

cd /d C:\Users\AliEray\Desktop\realtime-client-server-demo

Compile TCP:

cl /EHsc server.cpp /Fe:server.exe
cl /EHsc client.cpp /Fe:client.exe

Compile UDP:

cl /EHsc udp_server.cpp /Fe:udp_server.exe
cl /EHsc udp_client.cpp /Fe:udp_client.exe

------------------------------------------------------------

## 5️⃣ Run Instructions

Open TWO terminals.

Terminal 1:

udp_server.exe

Terminal 2:

udp_client.exe

------------------------------------------------------------

## 6️⃣ Engineering Focus

This project demonstrates:

- Low-level socket programming
- Reliable transmission over unreliable transport
- Sliding window protocol design
- Loss simulation and recovery
- Performance measurement
- Systems-level thinking

------------------------------------------------------------

## 7️⃣ Future Extensions

- RTT-based adaptive timeout (RTO calculation)
- Dynamic congestion window
- Congestion control simulation
- Cross-platform (Linux) port
- Automated benchmark scripts

------------------------------------------------------------

Author:
Ali Eray Kalaycı
Computer Engineering Student
Focused on Real-Time Systems & Networking
