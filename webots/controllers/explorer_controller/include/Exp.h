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

class Exp {
    std::vector<std::pair<int, int>> points_;
    MailBox mailBox_;
    std::thread runner_;


    void run() {
        while (true) {
            while (mailBox_.size() > 0) {
                AirplugMessage msg = mailBox_.pop();
                std::cout << "EXP received : " << msg.serialize() << std::endl;

                if (msg.getType() == AirplugMessage::local && msg.getEmissionApp() == "ROB") {
                    RobAck ra(msg.getValue("roback"));
                    switch (ra.getType()) {
                        case RobAck::undefinied:
                            break;
                        case RobAck::moved:
                            break;
                        case RobAck::turned:
                            break;
                        case RobAck::joined:
                            if (!msg.getValue("robcol").empty()) {
                                points_.emplace_back(ra.getCommand()[0], ra.getCommand()[1]);
                            }
                            break;
                        case RobAck::lacc:
                            break;
                        case RobAck::init:
                            break;
                        case RobAck::curr:
                            break;
                        case RobAck::tuned:
                            break;
                        case RobAck::order:
                            break;
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
    Exp() : runner_(&Exp::run, this) {
    }

    MailBox *getMailBox() {
        return &mailBox_;
    }
};


#endif //EXPLORER_EXP_H
