Realtime Client-Server Networking Demo

C++ / Winsock

Performance-focused C++ networking project on Windows using MSVC and Winsock.

This project implements a multi-threaded TCP server and a custom reliable UDP layer built from scratch, including sliding window delivery, simulated loss, retransmission logic, and performance measurement.

Implemented Components

Multi-threaded TCP server using a thread-per-client model.
TCP line protocol with HELLO and PING-PONG messaging plus RTT measurement.
Custom reliable UDP layer built on raw UDP sockets.
Sliding window transmission model.
Simulated ACK loss and timeout-based retransmission.
Duplicate detection and statistics tracking.

Requirements

Windows 10 or Windows 11.
MSVC toolchain through Visual Studio Build Tools or Visual Studio with Desktop development with C++.
No external dependencies. Pure Winsock implementation.

Scripts use the MSVC environment via vcvars64.bat, allowing batch files to run from a normal Command Prompt.

Quick Start

From the project folder:

build_all.bat builds both TCP and UDP components.
run_tcp.bat runs the TCP server and client demo.
run_udp.bat runs the UDP server and client demo.
clean.bat removes build outputs.

TCP Layer
Features

Line-based protocol using HELLO <name> and PING <timestamp>.
Server replies with ACK <name> and PONG <timestamp>.
Thread-per-client architecture.
RTT measurement on the client side.
Sequential load testing with more than 50 clients.

Localhost Results

RTT approximately 0 to 1 milliseconds, average around 0.4 to 0.5 milliseconds.

Architecture

Client communicates with a threaded TCP server.
Blocking sockets are used.
Text protocol with newline framing.

Reliable UDP Layer

Reliability is implemented on top of UDP.

PacketHeader structure contains magic value, type field, and sequence number.
Sequence numbers are used for ordering.
ACK packets confirm delivery.
Simulated ACK loss defaults to 30 percent.
Timeout-based retransmission ensures delivery.
Duplicate detection is implemented on the server.
Sliding window transmission is implemented on the client.
Statistics include sends, retries, and elapsed time.

Protocol Header Structure

#pragma pack(push, 1)
struct PacketHeader {
uint32_t magic;
uint16_t type;
uint16_t reserved;
uint32_t seq;
};
#pragma pack(pop)

Magic value is STAJ.

Sliding Window Test (Localhost)

Configuration used in the current demo:

Total packets set to 200.
Window size set to 16.
ACK loss simulation set to 30 percent.
Timeout set to 200 milliseconds.

Example result:

[UDP CLIENT] DONE: delivered 200 packets
[UDP CLIENT] stats: sends=289 retries=89 elapsed=5794ms

All packets are delivered successfully despite simulated ACK loss.

Build (Manual / MSVC)

Open x64 Developer Command Prompt.

Initialize environment:

"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"

Navigate to project directory:

cd path\to\realtime-client-server-demo

Compile TCP components:

cl /EHsc server.cpp /Fe:server.exe
cl /EHsc client.cpp /Fe:client.exe

Compile UDP components:

cl /EHsc udp_server.cpp /Fe:udp_server.exe
cl /EHsc udp_client.cpp /Fe:udp_client.exe

Run (Manual)

Open two terminals.

For TCP:

Terminal 1 runs server.exe
Terminal 2 runs client.exe

For UDP:

Terminal 1 runs udp_server.exe
Terminal 2 runs udp_client.exe

Repository Structure

server.cpp and client.cpp implement the TCP demo including line protocol and RTT measurement.
udp_server.cpp and udp_client.cpp implement the reliable UDP demo including sequence numbers, acknowledgments, loss simulation, and sliding window.
build_all.bat builds TCP and UDP components.
run_tcp.bat launches TCP demo.
run_udp.bat launches UDP demo.
clean.bat removes object and executable files.
.gitignore ignores build artifacts.

Notes and Limitations

The reliable UDP demo focuses on delivery using sequence numbers, acknowledgments, retransmission, and sliding window logic.
It does not implement congestion control, bandwidth estimation, or stream reassembly.

UDP server duplicate tracking uses an in-memory set for demonstration. Production systems would use bounded window-based tracking to prevent unbounded memory growth.

Engineering Focus

Low-level socket programming with Winsock.
Reliable delivery over unreliable transport.
Sliding window protocol design.
Loss simulation and recovery.
RTT measurement and performance reporting.
Systems-level thinking.

Future Extensions

RTT-based adaptive timeout.
Dynamic congestion window or congestion control simulation.
Cross-platform port to Linux.
Automated benchmarks and reporting.

Author

Ali Eray Kalaycı
Computer Engineering — Real-Time Systems and Networking Focus