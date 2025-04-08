#include "WebSocketClient.h"

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <boost/asio/ssl/context.hpp>
#include <iostream>
#include <functional>
#include <memory>
#include <mutex>

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
using websocketpp::connection_hdl;

//
// Implementation class for WebSocketClient using the Pimpl idiom.
// it relies on the caller (ChatInterface) to call run() on the main thread.
//
class WebSocketClient::Impl {
public:
    Impl() : m_connected(false), m_is_first_message(true), m_ready(false) {

        m_client.clear_access_channels(websocketpp::log::alevel::all);
        m_client.clear_error_channels(websocketpp::log::elevel::all);

        // Initialize ASIO.
        m_client.init_asio();
        
        // Set TLS initialization handler.
        m_client.set_tls_init_handler([](connection_hdl) -> std::shared_ptr<boost::asio::ssl::context> {
            auto ctx = std::make_shared<boost::asio::ssl::context>(
                boost::asio::ssl::context::tlsv12_client
            );
            try {
                ctx->set_options(
                    boost::asio::ssl::context::default_workarounds |
                    boost::asio::ssl::context::no_sslv2 |
                    boost::asio::ssl::context::no_sslv3 |
                    boost::asio::ssl::context::single_dh_use
                );
            } catch(std::exception &e) {
                std::cerr << "[WebSocketClient] TLS init error: " << e.what() << std::endl;
            }
            return ctx;
        });
        
        // Set handler for successful connection.
        m_client.set_open_handler([this](connection_hdl hdl) {
            std::lock_guard<std::mutex> lock(m_mutex);
            std::cout << "[WebSocketClient] Connection opened." << std::endl;
            m_hdl = hdl;
            m_connected = true;
            m_ready = true;
            m_cond.notify_all();
        });
        
        // Set handler for incoming messages.
        m_client.set_message_handler([this](connection_hdl, client::message_ptr msg) {
            std::lock_guard<std::mutex> lock(m_mutex);
            if(m_is_first_message) {
                std::cout << "[Server]: " << msg->get_payload() << std::endl;
                m_is_first_message = false;
            }
            else {
                m_last_message = msg->get_payload();
                m_message_received = true;
                m_cond.notify_all();
            }
        });
        
        // Set handler for failed connection.
        m_client.set_fail_handler([this](connection_hdl) {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_connected = false;
            std::cerr << "[WebSocketClient] Connection failed." << std::endl;
        });
        
        // Set handler for closed connection.
        m_client.set_close_handler([this](connection_hdl) {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_connected = false;
            std::cout << "[WebSocketClient] Connection closed." << std::endl;
        });
    }
    
    // Connect to the specified URI.
    bool connect(const std::string &uri) {
        websocketpp::lib::error_code ec;
        client::connection_ptr con = m_client.get_connection(uri, ec);
        if (ec) {
            std::cerr << "[WebSocketClient] Connection init error: " << ec.message() << std::endl;
            return false;
        }
        m_client.connect(con);
        return 1;
    }
    
    // Run the ASIO event loop. This call blocks until the connection closes.
    void run() {
        m_client.run();
    }
    
    // Send a text message.
    void sendMessage(const std::string &message) {
        websocketpp::lib::error_code ec;
        m_client.send(m_hdl, message, websocketpp::frame::opcode::text, ec);
        if (ec)
            std::cerr << "[WebSocketClient] Send error: " << ec.message() << std::endl;
    }
    
    // Close the connection gracefully.
    void close() {
        if (!m_connected) {
            std::cerr << "[WebSocketClient] Attempted to close an already closed connection." << std::endl;
            return; // Avoid attempting to close an invalid connection
        }
        websocketpp::lib::error_code ec;
        m_client.close(m_hdl, websocketpp::close::status::going_away, "Client closed", ec);
        if (ec)
            std::cerr << "[WebSocketClient] Close error: " << ec.message() << std::endl;
    }

    std::string recevieMessage() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.wait(lock, [this](){return m_message_received;});
        m_message_received = false;
        return m_last_message;
    }

    void waitUntilReady() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.wait(lock, [this](){return m_ready;});
    }

    
private:
    client m_client;
    connection_hdl m_hdl;
    bool m_connected;
    bool m_message_received;
    bool m_is_first_message;
    bool m_ready;
    std::mutex m_mutex;
    std::string m_last_message;
    std::condition_variable m_cond;
};

WebSocketClient::WebSocketClient() {
    pImpl = new Impl();
}

WebSocketClient::~WebSocketClient() {
    delete pImpl;
}

bool WebSocketClient::connect(const std::string &uri) {
    return pImpl->connect(uri);
}

void WebSocketClient::run() {
    pImpl->run();
}

void WebSocketClient::sendMessage(const std::string &message) {
    pImpl->sendMessage(message);
}

void WebSocketClient::close() {
    pImpl->close();
}

std::string WebSocketClient::receiveMessage() {
    return pImpl->recevieMessage();
}

void WebSocketClient::waitUntilReady() {
    pImpl->waitUntilReady();
}