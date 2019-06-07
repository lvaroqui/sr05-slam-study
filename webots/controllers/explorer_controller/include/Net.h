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

    std::map<string, UDPClient *> monitors_;

    std::thread runner_;

    MailBox mailBox_;

    std::map<string, MailBox *> subscribers;

    void run();

public:
    explicit Net(std::string name, int comPort) : nodeName_(std::move(name)), udpServer_(comPort) {
        std::cout << comPort << std::endl;
    }

    MailBox *getMailBox() {
        return &mailBox_;
    }


    void addSubscriber(const string &appName, MailBox *mailBox) {
        subscribers[appName] = mailBox;
    }

    void launch() {
        runner_ = std::thread(&Net::run, this);
    }
};


#endif //EXPLORER_NETWORK_H
