#include <utility>

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
#include <chrono>

class Exp {
    bool run_ = true;
    string id_;

    std::vector<std::pair<int, int>> points_;
    MailBox *netMailBox_;
    MailBox mailBox_;
    std::thread runner_;

    int x_ = 0;
    int y_ = 0;
    int heading_ = 0;

    void addPoints(std::vector<std::pair<int, int>> points);
    void handleRobMessage(AirplugMessage msg);
    void handleExpMessage(AirplugMessage msg);

    void run() {
        while (run_) {
            while (mailBox_.size() > 0) {
                AirplugMessage msg = mailBox_.pop();
                // Handle local messages from ROB (acknowledgments and collisions)
                if (msg.getType() == AirplugMessage::local && msg.getEmissionApp() == "ROB") {
                    handleRobMessage(msg);
                }
                else if (msg.getType() == AirplugMessage::air && msg.getEmissionApp() == "EXP") {
                    handleExpMessage(msg);
                }
                else if (msg.getEmissionApp() == "MAP"){
                    if (msg.getValue("typemsg") == "MAPCO") {
                        AirplugMessage message("NET", "MAP", AirplugMessage::air);
                        message.add("typemsg", "ROBCO");
                        message.add("xpos", std::to_string(x_));
                        message.add("ypos", std::to_string(y_));
                        message.add("heading", std::to_string(heading_));
                        message.add("obs", fromVectorOfPairsToString(points_));
                        netMailBox_->push(message);
                    }
                }
            }
            std::this_thread::sleep_for (std::chrono::milliseconds(10));
        }
    }

public:
    Exp(string id, MailBox *netMailBox) : id_(std::move(id)), netMailBox_(netMailBox), runner_(&Exp::run, this) {
    }

    ~Exp() {
        run_ = false;
        runner_.join();
        std::cout << "Exp shutdown";
    }

    MailBox *getMailBox() {
        return &mailBox_;
    }
};


#endif //EXPLORER_EXP_H
