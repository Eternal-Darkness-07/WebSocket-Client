#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <string>

class WebSocketClient {
public:
    WebSocketClient();
    ~WebSocketClient();

    // Connect to a WebSocket URI (e.g., "wss://echo.websocket.events")
    bool connect(const std::string &uri);

    // Start the event loop (this call blocks until the connection closes)
    void run();

    // Send a text message.
    void sendMessage(const std::string &message);

    // receive the message.
    std::string receiveMessage();

    //wait until connection is fully ready to send message
    void waitUntilReady();

    // Close the connection gracefully.
    void close();

private:
    // Pimpl idiom to hide implementation details.
    class Impl;
    Impl *pImpl;
};

#endif // WEBSOCKETCLIENT_H
