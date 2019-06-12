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
#include <utility>
#include <limits>
#include <com/RobOrd.h>

#define TTL_MAX 5
#define CHECK_NEIGHBOURS_RATE 50 // 10ms per unit
#define WARNING_DISTANCE 24 // 24*50 = 1200cm = 12m
#define SAFE_DISTANCE 4 // 4*50 = 200cm = 2m

class Exp {
    bool run_ = true;
    string id_;

    Map map_;
    MailBox *netMailBox_;
    MailBox mailBox_;
    std::thread runner_;

    std::queue<string> actionsQueue;

    bool followWall_ = true;

    int x_ = 0;
    int y_ = 0;
    int heading_ = 0;

    std::map<string, std::pair<int, std::pair<int, int>>> neighbours_;

    void handleRobMessage(AirplugMessage msg);

    void handleExpMessage(AirplugMessage msg);

    void handleMapMessage(AirplugMessage msg);

    void updateAndCheckNeighbours();

    std::pair<string, float> closestNeighbour();
	std::map<std::string, int> clock_;

    void goTowardsNeighbour(std::pair<string, float> neighbour);

    void reportPoint(int x, int y, pointType type);

    void reportPosToMap();

    void popActionsQueue();

    void clearActionsQueue();

    void run() {
        int checkNeighbours = 0;
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
            if (checkNeighbours++ == CHECK_NEIGHBOURS_RATE) {
                checkNeighbours = 0;
                updateAndCheckNeighbours();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    std::vector<std::pair<int, int>> getPointsBetween(int x1, int y1, int x2, int y2);

    enum direction {
        front,
        back,
        right,
        left,
    };

    std::pair<int, int> getPoint(direction dir);

    pointType getPointType(direction dir = front);

    void handleWallFollowing(bool collision);

public:
    Exp(string id, MailBox *netMailBox) : id_(id), netMailBox_(netMailBox), runner_(&Exp::run, this) {
		clock_[id_] = 0;
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
