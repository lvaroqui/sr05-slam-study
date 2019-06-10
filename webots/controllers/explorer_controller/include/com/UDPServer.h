//
// Created by luc on 19/05/19.
//

#ifndef EXPLORER_UDPSERVER_H
#define EXPLORER_UDPSERVER_H

#include <string>
#include <queue>
#include <thread>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <iostream>

using boost::asio::ip::udp;

class UDPServer {
    boost::asio::io_context ioContext_;
    int port_;
    udp::socket socket_;
    std::thread listener_;
    std::mutex mutex_messages_;
    std::queue<std::string> messages_;
    boost::array<char, 2048> buffer{};

    void listen() {
        while (true) {
            udp::endpoint remote_endpoint;
            socket_.receive_from(boost::asio::buffer(buffer), remote_endpoint);
            mutex_messages_.lock();
            messages_.push(buffer.data());
            mutex_messages_.unlock();
        }
    }

public:
    explicit UDPServer(int port) : port_(port), socket_(ioContext_, udp::endpoint(udp::v4(), port_)),
                          listener_(&UDPServer::listen, this) {

    }

    std::string popMessage() {
        std::string message;
        mutex_messages_.lock();
        message = messages_.front();
        messages_.pop();
        mutex_messages_.unlock();
        return message;
    }

    int getNumberOfMessages() {
        int numberOfMessages;
        mutex_messages_.lock();
        numberOfMessages = messages_.size();
        mutex_messages_.unlock();
        return numberOfMessages;
    }
};

#endif //EXPLORER_UDPSERVER_H
