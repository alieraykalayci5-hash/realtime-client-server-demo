# Realtime Client-Server Networking Demo (C++ / Winsock)

Performance-focused C++ networking project on Windows (MSVC + Winsock) implementing:

- Multi-threaded TCP server (thread-per-client)
- TCP line protocol (HELLO / PING-PONG) + RTT measurement
- Custom Reliable UDP layer (built from scratch)
- Sliding Window delivery
- Simulated ACK loss + timeout-based retransmission
- Duplicate detection + basic stats

---

## Requirements

- Windows 10/11
- MSVC toolchain (Visual Studio Build Tools / Visual Studio with **Desktop development with C++**)
- No external dependencies (pure Winsock)

> Scripts use MSVC environment via `vcvars64.bat`, so you can run `.bat` files from a normal CMD.

---

## Quick Start (Recommended)

From project folder:

- **Build everything**
  - `build_all.bat`

- **Run TCP demo**
  - `run_tcp.bat`

- **Run UDP demo**
  - `run_udp.bat`

- **Clean build outputs**
  - `clean.bat`

---

## 1) TCP Layer

### Features
- Line-based protocol: `HELLO <name>`, `PING <timestamp>`
- Server replies: `ACK <name>`, `PONG <timestamp>`
- Multi-threaded server (thread-per-client)
- RTT measurement on client
- Sequential load testing (50+ clients tested)

### Localhost Results
- RTT ≈ 0–1 ms (avg ~0.4–0.5 ms)

### Architecture
Client <-> Threaded TCP Server  
Blocking sockets  
Text protocol with newline framing

---

## 2) Reliable UDP Layer (Custom Protocol)

Reliability implemented on top of UDP:

- `PacketHeader` (magic + type + seq)
- Sequence numbers
- ACK packets
- Simulated ACK loss (default: 30%)
- Timeout-based retransmission
- Duplicate detection on server
- Sliding Window transmission on client
- Stats (sends / retries / elapsed)

### Protocol Header

```cpp
#pragma pack(push, 1)
struct PacketHeader {
    uint32_t magic;    // 'STAJ'
    uint16_t type;     // 1=DATA, 2=ACK
    uint16_t reserved;
    uint32_t seq;      // sequence number
};
#pragma pack(pop)
Magic value: 'STAJ'

3) Sliding Window Test (Localhost)
Configuration (current demo)
Total packets: 200

Window size: 16

ACK loss simulation: 30%

Timeout: 200ms

Example Result
csharp
Kodu kopyala
[UDP CLIENT] DONE: delivered 200 packets
[UDP CLIENT] stats: sends=289 retries=89 elapsed=5794ms
All packets are delivered successfully despite simulated ACK loss.

Build (Manual / MSVC)
Open x64 Developer Command Prompt:

arduino
Kodu kopyala
"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"
Then:

bash
Kodu kopyala
cd /d C:\Users\AliEray\Desktop\realtime-client-server-demo
Compile TCP:

ruby
Kodu kopyala
cl /EHsc server.cpp /Fe:server.exe
cl /EHsc client.cpp /Fe:client.exe
Compile UDP:

ruby
Kodu kopyala
cl /EHsc udp_server.cpp /Fe:udp_server.exe
cl /EHsc udp_client.cpp /Fe:udp_client.exe
Run (Manual)
Open two terminals.

TCP:

Terminal 1: server.exe

Terminal 2: client.exe

UDP:

Terminal 1: udp_server.exe

Terminal 2: udp_client.exe

Repo Structure
server.cpp / client.cpp : TCP demo (line protocol + RTT)

udp_server.cpp / udp_client.cpp : Reliable UDP demo (seq/ack + loss + sliding window)

build_all.bat : builds TCP + UDP

run_tcp.bat : opens 2 windows and runs TCP server/client

run_udp.bat : opens 2 windows and runs UDP server/client

clean.bat : removes .obj and .exe

.gitignore : ignores build artifacts

Notes / Limitations (Demo Scope)
Reliable UDP demo focuses on delivery (seq/ack + retransmit + window).
It does not implement congestion control, bandwidth estimation, or stream reassembly.

UDP server duplicate tracking uses an in-memory set for demonstration; production code would use
bounded tracking (window-based) to avoid unbounded growth.

Engineering Focus
Low-level socket programming (Winsock)

Reliable delivery over unreliable transport (UDP)

Sliding window protocol design

Loss simulation + recovery

RTT measurement + performance reporting

Systems-level thinking

Future Extensions (Optional)
RTT-based adaptive timeout (RTO)

Dynamic congestion window / congestion control simulation

Cross-platform port (Linux)

Automated benchmarks and reporting

Author
Ali Eray Kalaycı
Computer Engineering Student — Real-Time Systems & Networking