#ifndef CHATINTERFACE_H
#define CHATINTERFACE_H

#include "WebSocketClient.h"

// ChatInterface provides a command-line interface for interacting with the WebSocket server.
class ChatInterface {
public:
    ChatInterface();
    void run();

private:
    WebSocketClient client;
};

#endif //CHATINTERFACE_H