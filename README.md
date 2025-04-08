# WebSocket Chat Client

## Project Description

This project implements a WebSocket-based chat client application. The main components include:
- WebSocketClient: Handles WebSocket communication
- ChatInterface: Provides the user interface for chat functionality

The application allows users to connect to WebSocket servers, send and receive messages in a chat-like interface.

## Required C++ Libraries

The following libraries are required for this project:

1. **Boost** - Used for networking and asynchronous operations
   ```
   sudo apt-get install libboost-all-dev
   ```
   
2. **OpenSSL** - Required for secure WebSocket connections
   ```
   sudo apt-get install libssl-dev
   ```

3. **WebSocket++** - C++ WebSocket client/server library
   ```
   git clone https://github.com/zaphoyd/websocketpp.git
   cd websocketpp
   mkdir build
   cd build
   cmake ..
   make
   sudo make install
   ```

4. **GN** - Build system
   ```
   git clone https://gn.googlesource.com/gn
   cd gn
   python build/gen.py
   ninja -C out
   ```
   Then add the GN executable to your PATH.

## Build Instructions

### Setting up the build environment

```bash
# Clone the repository
git clone <repository-url>
cd WebSocket-Client
```

### Building with GCC

#### Debug Mode
```bash
gn gen out/debug --args="is_debug=true compiler=\"gcc\""
ninja -C out/debug
```

#### Release Mode
```bash
gn gen out/release --args="is_debug=false compiler=\"gcc\""
ninja -C out/release
```

### Building with Clang

#### Debug Mode
```bash
gn gen out/clang_debug --args="is_debug=true compiler=\"clang\""
ninja -C out/clang_debug
```

#### Release Mode
```bash
gn gen out/clang_release --args="is_debug=false compiler=\"clang\""
ninja -C out/clang_release
```

## Project Structure

```
InternShip_assignment/
├── .gn                           # GN configuration file
├── BUILD.gn                      # Main build file
├── build_files/                  # Build configuration files
│   ├── config/
│   │   ├── BUILD.gn
│   │   └── BUILDCONFIG.gn
│   └── toolchain/
│       └── BUILD.gn
├── src/                          # Source files
│   ├── BUILD.gn
│   ├── main.cpp
│   ├── WebSocketClient.h
│   ├── WebSocketClient.cpp
│   ├── ChatInterface.h
│   └── ChatInterface.cpp
└── test/                         # Test files
    ├── BUILD.gn
    └── WebSocketClient_test.cpp
```

## Running the Application

After building, run the application with:

```bash
./out/debug/chat_client    # for debug build
# or
./out/release/chat_client  # for release build
```

## Running Tests

```bash
./out/debug/websocket_tests  # for debug build tests
```

## Troubleshooting

If you encounter build errors related to missing dependencies, ensure all required libraries are installed with their development headers.

For WebSocket connection issues, verify:
- The WebSocket server is running and accessible
- Network connectivity and firewall settings allow WebSocket connections
- The correct WebSocket URL format is being used (ws:// or wss://)

If you encounter issues with WebSocket++, make sure:
- You have the correct version installed
- The library path is correctly set in your build environment
- All Boost dependencies required by WebSocket++ are installed
