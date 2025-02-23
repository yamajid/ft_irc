# 💬 IRC Chat Application

## Overview
A robust IRC (Internet Relay Chat) server implementation built in C++, featuring TCP/IP socket programming for reliable real-time communication. This project demonstrates low-level networking concepts while providing a feature-rich chat environment.

## 🌟 Key Features

### 📡 Network Features
- **TCP/IP Implementation**: Reliable message delivery
- **Multi-Client Support**: Concurrent user handling
- **Socket Programming**: Efficient network communication
- **Buffer Management**: Optimized data transmission

### 💭 Chat Features
- **Channel Operations**: Create and manage chat rooms
- **Private Messaging**: Direct user-to-user communication
- **Broadcast Messages**: Server-wide announcements
- **User Authentication**: Basic user verification

### ⌨️ Command System
```bash
# Core Commands
/JOIN <channel>    # Join a channel
/PRIVMSG <user>   # Send private message
/QUIT             # Disconnect from server
/LIST             # List available channels
/WHO              # List users in current channel
/TOPIC            # list the channel topic
/MODE             # list channel mode

```

## 🛠 Technical Stack

### Core Technologies
- **Language**: C++98
- **Network Protocol**: TCP/IP
- **Build System**: Makefile
- **Architecture**: Client-Server Model

### Development Tools
- **Compiler**: cpp
- **Version Control**: Git

## 🚀 Getting Started

### System Requirements
```bash
# Required packages
- cpp compiler
- Make
- POSIX-compliant OS
```

### Build Instructions
```bash
# Clone repository
git clone https://github.com/yamajid/ft_irc.git
cd ft_irc

# Build the project
make

# Run server
./ircserv <port> <password>

# Run client
./client <hostname> <port>
```

## 📖 Usage Guide

### Server Configuration
```bash
# Start server
./ircserv 6667 serverpassword

# Monitor connections
netstat -an | grep 6667
```


## 🔧 Project Structure
```
ft_irc/
├── src/
│   ├── server/
│   │   ├── Server.cpp
│   │   ├── Channel.cpp
│   │   └── User.cpp
│   ├── client/
│   │   └── Client.cpp
│   └── main.cpp
├── include/
│   ├── Server.hpp
│   ├── Channel.hpp
│   └── User.hpp
├── Makefile
└── README.md
```

## 🔍 Technical Details

### Network Implementation
- Socket programming using POSIX API
- Non-blocking I/O operations
- Event-driven architecture
- Buffer management for message handling

### Error Handling
- Robust error checking
- Graceful connection termination
- Memory leak prevention
- Signal handling

## 🛡 Security Features
- Password-protected server access
- Basic flood protection
- Input validation
- Secure channel operations

## 💻 Development

### Building from Source
```bash
# Debug build
make debug

# Release build
make release

# Clean build files
make clean

# Full rebuild
make re
```


## 👥 Contributors
- @yamajid




## 📫 Support
For support and queries:
- Open an issue
- Contact: @yamajid


---
*Note: This project was developed as part of the 42 School curriculum.*
