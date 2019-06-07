//
// Created by luc on 03/06/19.
//

#ifndef EXPLORER_EXP_H
#define EXPLORER_EXP_H

#include "com/AirplugMessage.h"
#include "com/RobAck.h"
#include "com/MailBox.h"
#include <thread>
#include <vector>
#include <utility>
#include <boost/algorithm/string/split.hpp>


class Exp {
    std::vector<std::pair<int, int>> points_;
    MailBox *netMailBox_;
    MailBox mailBox_;
    std::thread runner_;


    void run() {
        while (true) {
            while (mailBox_.size() > 0) {
                AirplugMessage msg = mailBox_.pop();
                std::cout << "EXP received : " << msg.serialize() << std::endl;

                if (msg.getType() == AirplugMessage::local && msg.getEmissionApp() == "ROB") {
                    RobAck ra(msg.getValue("roback"));
                    if (ra.getType() == RobAck::joined || ra.getType() == RobAck::moved ||
                        ra.getType() == RobAck::turned) {
                        std::vector<string> tmp;
                        boost::split(tmp, msg.getValue("robotpos"), [](char c) { return c == ','; });
                        int x = std::stoi(tmp[0]);
                        int y = std::stoi(tmp[1]);
                        int heading = std::stoi(tmp[2]);
                        AirplugMessage ackMsg("ROB", "MAP", AirplugMessage::air);
                        ackMsg.add("roback", "curr:" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(heading));
                        if (!msg.getValue("robcol").empty()) {
                            ackMsg.add("obs", std::to_string(x) + "," + std::to_string(y));
                        }
                        else {
                            ackMsg.add("obs", "");
                        }
                        netMailBox_->push(ackMsg);
                    }
                }
                std::cout << "Map points are : " << std::endl;
                for (auto point : points_) {
                    std::cout << point.first << "," << point.second << std::endl;
                }
                std::cout << std::endl;
            }
        }
    }

public:
    Exp(MailBox *netMailBox) : netMailBox_(netMailBox), runner_(&Exp::run, this) {
    }

    MailBox *getMailBox() {
        return &mailBox_;
    }
};


#endif //EXPLORER_EXP_H
