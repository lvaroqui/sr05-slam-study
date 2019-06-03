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
    int port_;
    udp::socket socket_;
    udp::resolver resolver_;
    udp::endpoint endpoint_;

public:
    UDPClient(std::string ip, int port) : ip_(std::move(ip)), port_(std::move(port)), socket_(ioContext_), resolver_(ioContext_) {
        endpoint_ = *resolver_.resolve(udp::v4(), ip_, std::to_string(port)).begin();
        socket_.open(udp::v4());
    }

    void sendMessage(std::string message) {
        socket_.send_to(boost::asio::buffer(message + "\n"), endpoint_);
    }
};

#endif //EXPLORER_TCPCLIENT_H
