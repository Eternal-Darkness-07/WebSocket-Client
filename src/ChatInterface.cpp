#include "ChatInterface.h"
#include <iostream>
#include <string>
#include <thread>

ChatInterface::ChatInterface() { }

void ChatInterface::run() {
    std::string uri = "wss://echo.websocket.events";
    if (!client.connect(uri)) {
        std::cerr << "[ChatInterface] Failed to connect to " << uri << std::endl;
        return;
    }
    
    // Start the event loop in a separate thread so that open handler is processed.
    std::thread eventThread([this]() {
        client.run();
    });
    std::cout << "[ChatInterface] Connected to " << uri << std::endl;
    
    // Now wait until the connection is fully ready.
    client.waitUntilReady();
    
    // Spawn a thread to handle user input.
    std::thread inputThread([this]() {
        while (true) {
            std::cout << "> ";
            std::string input;
            std::getline(std::cin, input);
            if (input == "exit") {
                client.close();
                break;
            }
            client.sendMessage(input);
            std::cout << "[Server]: " << client.receiveMessage() << std::endl;
        }
    });
    
    // Wait for both threads to finish.
    inputThread.join();
    eventThread.join();
}
