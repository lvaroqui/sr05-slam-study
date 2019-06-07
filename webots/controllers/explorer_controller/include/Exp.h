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
#include <math.h>
#include "utils.h"


class Exp {
    std::vector<std::pair<int, int>> points_;
    MailBox *netMailBox_;
    MailBox mailBox_;
    std::thread runner_;

    int x = 0;
    int y = 0;
    int heading = 0;

    void run() {
        while (true) {
            while (mailBox_.size() > 0) {
                AirplugMessage msg = mailBox_.pop();
                std::cout << "EXP received : " << msg.serialize() << std::endl;

                if (msg.getType() == AirplugMessage::local && msg.getEmissionApp() == "ROB") {
                    RobAck ra(msg.getValue("roback"));
                    if (ra.getType() == RobAck::joined || ra.getType() == RobAck::moved ||
                        ra.getType() == RobAck::turned) {

                        // Getting robot position
                        std::vector<string> tmp;
                        boost::split(tmp, msg.getValue("robotpos"), [](char c) { return c == ','; });
                        x = std::stoi(tmp[0]);
                        y = std::stoi(tmp[1]);
                        heading = std::stoi(tmp[2]);

                        AirplugMessage ackMsg("ROB", "MAP", AirplugMessage::air);
                        ackMsg.add("roback", "curr:" + std::to_string(x) + "," + std::to_string(y) + "," +
                                             std::to_string(heading));

                        // If a collision is detected adding point to MAP
                        if (!msg.getValue("robcol").empty()) {
                            std::vector<std::pair<int, int>> points;
                            int xWall = x + 14 * cos(heading * M_PI / 180);
                            int yWall = y + 14 * sin(heading * M_PI / 180);
                            for (int i = -10; i <= 10; i++) {
                                std::pair<int, int> point(xWall + i * cos((heading + 90) * M_PI / 180),
                                                          yWall + i * sin((heading + 90) * M_PI / 180));
                                auto it = find(points.begin(), points.end(), point);
                                if (it == points.end()) {
                                    points.push_back(point);
                                }
                            }

                            // Adding points to local map
                            points_.reserve(points.size());
                            points_.insert(points_.end(), points.begin(), points.end());
                            sort(points_.begin(), points_.end());
                            points_.erase(unique(points_.begin(), points_.end()), points_.end());

                            ackMsg.add("obs", fromVectorOfPairsToString(points));
                        }
                        netMailBox_->push(ackMsg);
                    }
                }
                else if (msg.getEmissionApp() == "MAP"){
                    if (msg.getValue("typemsg") == "MAPCO") {
                        AirplugMessage message("NET", "MAP", AirplugMessage::air);
                        message.add("typemsg", "ROBCO");
                        message.add("xpos", std::to_string(x));
                        message.add("ypos", std::to_string(y));
                        message.add("heading", std::to_string(heading));
                        message.add("obs", fromVectorOfPairsToString(points_));
                        netMailBox_->push(message);
                    }
                }
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
