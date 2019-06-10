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
#include "../../../../monitor/utils.h"
#include <chrono>
#include <map>
#include <pair>
#include <limits>

#define TTL_MAX 5

class Exp {
    bool run_ = true;
    string id_;

    Map map_;
    Map unsentMap_;
    MailBox *netMailBox_;
    MailBox mailBox_;
    std::thread runner_;

    int x_ = 0;
    int y_ = 0;
    int heading_ = 0;
    bool inited = false;

    std::map<string, std::pair<int, std::pair<int, int>>> neighbours_;

    void handleRobMessage(AirplugMessage msg);

    void handleExpMessage(AirplugMessage msg);

    void handleMapMessage(AirplugMessage msg);

    void updateAndCheckNeighbours();

    std::pair<string, float> closestNeighbour();

    void run() {
        while (run_) {
            while (mailBox_.size() > 0) {
                AirplugMessage msg = mailBox_.pop();
                // Handle local messages from ROB (acknowledgments and collisions)
                if (msg.getType() == AirplugMessage::local && msg.getEmissionApp() == "ROB") {
                    handleRobMessage(msg);
                } else if (msg.getType() == AirplugMessage::air && msg.getEmissionApp() == "EXP") {
                    handleExpMessage(msg);
                } else if (msg.getEmissionApp() == "MAP") {
                    handleMapMessage(msg);
                }
            }
            updateAndCheckNeighbours(); //TODO : we should run it at a fixed clean rate
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    std::vector<std::pair<int, int>> getPointsBetween(int x1, int y1, int x2, int y2);

public:
    Exp(string id, MailBox *netMailBox) : id_(id), netMailBox_(netMailBox), runner_(&Exp::run, this) {
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
