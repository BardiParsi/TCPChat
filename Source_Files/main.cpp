#ifdef _WIN32 // Windows only undefined selection
#define _WIN32_WINNT 0x0A00
#endif 

#include <iostream>
#include <thread>
#include <memory>
#include <exception>
#include <string> 
#include <vector>
#include "LOGGER.h"
#include "Buffer_Sanitizer.h"
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <boost/thread.hpp>

namespace asio = boost::asio; 
using io_context = asio::io_context;
using error_code = boost::system::error_code;
using endPoint = asio::ip::tcp::endpoint;
using tcpSocket = asio::ip::tcp::socket;
using tcp = asio::ip::tcp;
using resolver = asio::ip::tcp::resolver;

constexpr size_t BUFF_SIZE{1024};

int main() {
        // LOGGER object to log to console
        LOGGER console;
        std::string ipAddress = "127.0.0.1";
        std::string portNum = "3000";
        try {
            // Buffer Cleaner class to make the buffer printable
            Buffer_Sanitizer buffCleaner;
            // Error handler class for Asio
            error_code ec;
            // Initiation object
            io_context context;
            // Resolver class for IP addresses
            resolver resolv(context);
            // Add address of somewhere we wish to connect to port 3000
            auto endPoint = resolv.resolve(ipAddress, portNum, ec);
            // Create a socket with the "context" object
            tcpSocket socket(context);
            // Tell the socket to connect to the end point
            asio::connect(socket, endPoint, ec);

            {
                std::jthread t([&]() {
                    if (!ec) {
                        std::vector<char> buffer(BUFF_SIZE);
                        size_t byteRead = socket.read_some(boost::asio::buffer(buffer), ec);
                        if (!ec) {
                            console.log("Connection is successfully made. ");
                            std::string result = buffCleaner(buffer);
                            console.log(result);
                        }
                        else {
                            console.log("Error reading from socket: ", ec.message());
                        }
                    }
                    else {
                        console.log("Error! Connection failed: ", ec.message());
                    }
                    });
                t.join();
            }
        }
        catch (const std::exception& e) {
            console.log("Unexpected error! ", e.what());
        }

    return 0;
}
