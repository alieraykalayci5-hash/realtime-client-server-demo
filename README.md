# Realtime Client-Server Networking Demo (C++)

A performance-focused C++ networking project implementing both TCP and a custom Reliable UDP layer using Winsock on Windows.

This project demonstrates real-time networking concepts including RTT measurement, retransmission strategies, sliding window protocols, and packet loss simulation.

---

# 🔹 Features

## TCP Layer
- Line-based protocol (HELLO / PING-PONG)
- Multi-threaded TCP server
- RTT measurement (round-trip time)
- Sequential client load testing (50+ connections tested)
- Localhost RTT: ~0–1 ms (avg ~0.4–0.5 ms)

---

## UDP Reliability Layer (Custom Implementation)

A reliable data transmission system built on top of UDP featuring:

- Sequence-based packet identification
- ACK mechanism
- Simulated ACK packet loss (configurable)
- Timeout-based retransmission
- Duplicate packet detection
- Sliding Window transmission
- Performance statistics output

---

# 🔹 Sliding Window Reliability Test (Localhost)

Test Configuration:

- Total Packets: 200
- Window Size: 16
- ACK Loss Simulation: 30%
- Timeout: 200ms

Example Output:

[UDP CLIENT] DONE: delivered 200 packets
[UDP CLIENT] stats: sends=289 retries=89 elapsed=5794ms


Result:
All 200 packets were successfully delivered despite 30% simulated ACK loss.

---

# 🔹 Architecture Overview

## TCP
Client <-> Multi-threaded Server  
Blocking sockets  
Line-based protocol

## UDP
Client → DATA(seq)  
Server → ACK(seq)  

Sliding Window:
- Multiple in-flight packets
- Timeout tracking per packet
- Retransmit on missing ACK
- Duplicate detection on server

---

# 🔹 Build Instructions (Windows - MSVC)

Open x64 Developer Command Prompt:

"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"

Compile TCP:

cl /EHsc server.cpp /Fe:server.exe
cl /EHsc client.cpp /Fe:client.exe

Compile UDP:

cl /EHsc udp_server.cpp /Fe:udp_server.exe
cl /EHsc udp_client.cpp /Fe:udp_client.exe

Run:

Open two separate terminals:

Terminal 1:

udp_server.exe


Terminal 2:

udp_client.exe


---

# 🔹 Engineering Focus

This project focuses on:

- Low-level socket programming
- Reliable transmission over unreliable transport
- Network performance measurement
- Loss simulation and recovery strategies
- Sliding window protocol design
- Practical systems engineering concepts

---

# 🔹 Future Improvements (Optional Extensions)

- Adaptive timeout (RTT-based RTO calculation)
- Dynamic window size tuning
- Congestion control simulation
- Cross-platform (Linux) support
- Benchmark automation

---

# 🔹 Author

Ali Eray Kalaycı  
Computer Engineering Student  
Focused on Real-Time Systems & Networking
