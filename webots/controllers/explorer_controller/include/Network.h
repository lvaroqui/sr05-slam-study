#include <utility>

//
// Created by luc on 21/05/19.
//

#ifndef EXPLORER_NETWORK_H
#define EXPLORER_NETWORK_H

#include "UDPServer.h"
#include "UDPClient.h"
#include "AirplugMessage.h"
#include "MailBox.h"
#include <thread>
#include <map>

class Network {
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
                if (subscribers.count(msg.getDestinationApp()) == 1) {
                    subscribers[msg.getDestinationApp()]->push(msg);
                }
                else {
                    std::cout << "Unknown destination app: " << msg.getDestinationApp() << std::endl;
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
    explicit Network(std::string name, int comPort) : nodeName_(std::move(name)), udpServer_(comPort) {
    }

    void addSubscriber(const string &appName, MailBox *mailBox) {
        subscribers[appName] = mailBox;
    }

    void launch() {
        runner_ = std::thread(&Network::run, this);
    }

    void giveMessage(AirplugMessage msg) {
        mailBox_.push(msg);
    }
};


#endif //EXPLORER_NETWORK_H
