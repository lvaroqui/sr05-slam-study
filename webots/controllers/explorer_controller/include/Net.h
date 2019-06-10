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

#define BROADCAST "broadcast"
#define NEIGHBOURHOOD "neighbourhood"

class Net {
    UDPServer udpServer_;

    bool run_ = true;

    std::string id_;

    std::map<string, UDPClient *> monitors_;

    std::thread runner_;

    MailBox lchMailBox_;
    MailBox airInMailBox_;
    MailBox *airOutMailBox_ = nullptr;

    std::map<string, MailBox *> subscribers;

    std::map<string, int> lastMessagesReceived_;
    int lastMessageSentNumber_;

    void run();

public:
    explicit Net(std::string id, int comPort) : id_(std::move(id)), udpServer_(comPort), lastMessageSentNumber_(0) {
        std::cout << "NET initialized for robot " << id_ << " at port " << comPort << std::endl;
    }

    ~Net() {
        run_ = false;
        runner_.join();
        std::cout << "Net " << id_ << " shutdown";
    }

    MailBox *getMailBox() {
        return &lchMailBox_;
    }

    MailBox *getAirInMailBox() {
        return &airInMailBox_;
    }

    void addAirOutMailBox(MailBox *airOutMailBox) {
        airOutMailBox_ = airOutMailBox;
    }

    void addSubscriber(const string &appName, MailBox *mailBox) {
        subscribers[appName] = mailBox;
    }

    void launch() {
        runner_ = std::thread(&Net::run, this);
    }
};


#endif //EXPLORER_NETWORK_H
