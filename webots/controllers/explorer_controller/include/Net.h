#include <utility>

//
// Created by luc on 21/05/19.
//

#ifndef EXPLORER_NETWORK_H
#define EXPLORER_NETWORK_H

#include "com/UDPServer.h"
#include "com/UDPClient.h"
#include "com/AirplugMessage.h"
#include "com/MailBox.h"
#include <thread>
#include <map>

class Net {
    UDPServer udpServer_;

    std::string nodeName_;

    std::thread runner_;

    MailBox mailBox_;

    std::map<string, MailBox *> subscribers;

    void run() {
        while (true) {
            while (udpServer_.getNumberOfMessages() > 0) {
                mailBox_.push(AirplugMessage(udpServer_.popMessage()));
            }
            while (mailBox_.size() > 0) {
                AirplugMessage msg = mailBox_.pop();
                std::cout << "Received: " << msg.serialize() << std::endl;
                if (subscribers.count(msg.getDestinationApp()) == 1) {
                    subscribers[msg.getDestinationApp()]->push(msg);
                }
            }
        }
    }

    void sendMessageUDP(AirplugMessage message) {
        UDPClient client("localhost", 3000);

        // Adding header
        message.add("sender", nodeName_);

        client.sendMessage(message.serialize());
    }

public:
    explicit Net(std::string name, int comPort) : nodeName_(std::move(name)), udpServer_(comPort) {
    }

    void addSubscriber(const string &appName, MailBox *mailBox) {
        subscribers[appName] = mailBox;
    }

    void launch() {
        runner_ = std::thread(&Net::run, this);
    }

    void giveMessage(AirplugMessage msg) {
        mailBox_.push(msg);
    }
};


#endif //EXPLORER_NETWORK_H