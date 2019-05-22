#include <utility>

//
// Created by luc on 21/05/19.
//

#ifndef EXPLORER_NETWORK_H
#define EXPLORER_NETWORK_H

#include "UDPServer.h"
#include "UDPClient.h"
#include "AirplugMessage.h"
#include <thread>

class Network {
    UDPServer udpServer_;

    std::string nodeName_;

    std::thread runner_;

    std::mutex mutexRobotMessages_;
    std::queue<AirplugMessage> robotMessages_;

    void run() {
        while (true) {
            while (udpServer_.getNumberOfMessages() > 0) {
                mutexRobotMessages_.lock();
                robotMessages_.push(AirplugMessage(udpServer_.popMessage()));
                mutexRobotMessages_.unlock();
            }
        }
    }

public:
    explicit Network(std::string name, int comPort) : nodeName_(std::move(name)), udpServer_(comPort),
                                    runner_(&Network::run, this) {
    }

    void sendMessage(AirplugMessage message) {
        UDPClient client("localhost", 3000);

        // Adding header
        message.add("sender", nodeName_);
        
        client.sendMessage(message.serialize());
    }

    AirplugMessage popRobotMessage() {
        AirplugMessage message;
        mutexRobotMessages_.lock();
        message = robotMessages_.back();
        robotMessages_.pop();
        mutexRobotMessages_.unlock();
        return message;
    }

    int getNumberOfRobotMessages() {
        int numberOfMessages;
        mutexRobotMessages_.lock();
        numberOfMessages = robotMessages_.size();
        mutexRobotMessages_.unlock();
        return numberOfMessages;
    }
};


#endif //EXPLORER_NETWORK_H
