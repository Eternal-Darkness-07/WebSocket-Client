#include "WebSocketClient.h"
#include <gtest/gtest.h>
#include <chrono>
#include <thread>

TEST(WebSocketClientTest, ConnectSendReceive) {
    // Create the client.
    WebSocketClient client;
    
    // Set a valid echo server URI.
    std::string uri = "wss://echo.websocket.events";
    
    // Connect to the server.
    bool connected = client.connect(uri);
    EXPECT_TRUE(connected) << "Failed to connect to " << uri;
    
    // Start the ASIO event loop in a background thread so that asynchronous events are processed.
    std::thread eventThread([&client]() {
        client.run();
    });
    
    // Wait until the connection is fully open.
    client.waitUntilReady();
    
    // Now send a test message.
    std::string testMessage = "Hello, Unit Test!";
    client.sendMessage(testMessage);
    
    // Wait a short time for the echo to come back.
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // Receive the message.
    std::string response = client.receiveMessage();
    
    // Verify that the echoed response matches the test message.
    EXPECT_EQ(response, testMessage);
    
    // Close the connection.
    client.close();
    
    // Wait for the event loop thread to finish.
    eventThread.join();
}

TEST(WebSocketClientTest, CloseWithoutConnect) {
    WebSocketClient client;
    EXPECT_NO_THROW(client.close());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
