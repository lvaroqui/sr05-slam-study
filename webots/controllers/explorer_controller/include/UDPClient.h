#include <utility>

#include <utility>

//
// Created by luc on 17/05/19.
//

#ifndef EXPLORER_TCPCLIENT_H
#define EXPLORER_TCPCLIENT_H

#include <string>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <iostream>

using boost::asio::ip::udp;

class UDPClient {
    boost::asio::io_context ioContext_;
    std::string ip_;
    std::string port_;
    udp::socket socket_;
    udp::resolver resolver_;
    udp::endpoint endpoint_;

public:
    UDPClient(std::string ip, std::string port) : ip_(std::move(ip)), port_(std::move(port)), socket_(ioContext_), resolver_(ioContext_) {
        endpoint_ = *resolver_.resolve(udp::v4(), ip_, port_).begin();
        socket_.open(udp::v4());
    }

    void sendMessage(std::string message) {
        socket_.send_to(boost::asio::buffer(message), endpoint_);
    }
};


//try {
//boost::asio::io_context io_context;
//
//tcp::socket s(io_context);
//tcp::resolver resolver(io_context);
//
//boost::asio::connect(s, resolver.resolve("localhost", "3000"));
//
//char request[10] = "Heyhey\n";
//size_t request_length = std::strlen(request);
//boost::asio::write(s, boost::asio::buffer(request, request_length));
//
//char reply[1024];
//size_t reply_length = boost::asio::read(s,
//                                        boost::asio::buffer(reply, request_length));
//std::cout << "Reply is: ";
//std::cout.write(reply, reply_length);
//std::cout << "\n";
//}
//catch (std::exception &e) {
//std::cerr << "Exception: " << e.what() << "\n";
//}

#endif //EXPLORER_TCPCLIENT_H
