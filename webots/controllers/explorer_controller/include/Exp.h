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
#include "../../../../pathfinder/pathfinder.h"
#include <chrono>
#include <map>
#include <utility>
#include <limits>
#include <com/RobOrd.h>

#define TTL_MAX 3
#define CHECK_NEIGHBOURS_RATE 50 // 10ms per unit

class Exp {
    bool run_ = true;
    string id_;

    Map map_;
    MailBox *netMailBox_;
    MailBox mailBox_;
    std::thread runner_;

    std::queue<string> actionsQueue;

    enum status {
        standBy,
        followingWall,
        joiningNeighboor,
        pathFindingNavigation
    };

    status status_ = standBy;

    int x_ = 0;
    int y_ = 0;
    int heading_ = 0;

    class Neighbour {
    public:
        Neighbour() = default;

        Neighbour(int TTL, int X, int Y) : ttl(TTL), x(X), y(Y) {}

        int ttl;
        int x;
        int y;
    };

    std::map<string, Neighbour> neighbours_;

    void handleRobMessage(AirplugMessage msg);

    void handleExpMessage(AirplugMessage msg);

    void handleMapMessage(AirplugMessage msg);

    void updateAndCheckNeighbours();

    Neighbour &closestNeighbour();

    void reportPoint(int x, int y, pointType type);

    void reportPosToMap();

    void popActionsQueue();

    void clearActionsQueue();

    void run() {
//        int checkNeighbours = 0;
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
//            if (checkNeighbours++ == CHECK_NEIGHBOURS_RATE) {
//                checkNeighbours = 0;
//                updateAndCheckNeighbours();
//            }
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

    void goToPathFinding(int x, int y);

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
