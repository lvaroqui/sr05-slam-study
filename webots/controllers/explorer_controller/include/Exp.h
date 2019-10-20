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
#define HELLO_MESSAGE_RATE 100 // 10ms per unit

class Exp {
    bool run_ = true;
    bool init_ = false;
    string id_;

    Map map_;
    MailBox *netMailBox_;
    MailBox mailBox_;
    std::thread runner_;

    std::queue<string> actionsQueue;

    enum status {
        initting,
        standBy,
        followingWall,
        exploring,
        finished
    };

    status status_ = initting;

    int x_ = 0;
    int y_ = 0;
    int heading_ = 0;

    int xHome_ = 0;
    int yHome_ = 0;

    class Neighbour {
    public:
        Neighbour() = default;

        Neighbour(int X, int Y) : x(X), y(Y) {}

        int x = 0;
        int y = 0;
    };

    std::map<string, Neighbour> neighbours_;

    void handleRobMessage(AirplugMessage msg);

    void handleExpMessage(AirplugMessage msg);

    void handleMapMessage(AirplugMessage msg);

    void helloMessage();

    Neighbour &closestNeighbour();

    void reportPoint(int x, int y, pointType type);

    void reportPointToMap(int x, int y, pointType type);

    void reportPosToMap();

    void popActionsQueue();

    void clearActionsQueue();

    void continueExploration() {
        std::pair<int, int> point;
        bool end = false;
        bool pathFound = true;
        do {
            findFrontiers(map_);
            auto mapWithOtherRobots = Map(map_);

            for (const auto& neighbour : neighbours_) {
                for(int i = -1; i <= 1; i++) {
                    for(int j = -1; j <= 1; j++) {
                        mapWithOtherRobots[std::make_pair(neighbour.second.x + i, neighbour.second.y + j)] = wall;
                    }
                }
            }

            if (!pathFound) {
                mapWithOtherRobots[point] = wall;
                map_[point] = wall;
            }
            if (!findClosestFrontier(mapWithOtherRobots, std::make_pair(x_, y_), point)) {
                end = true;
                break;
            }
        } while (!(pathFound = goToPathFinding(point.first, point.second)));
        if (end) {
            status_ = finished;
            std::cout << "Finished exploration ! Returning to home : " << xHome_ << " " << yHome_ << std::endl;
            goToPathFinding(xHome_, yHome_);
        }
        else {
            status_ = exploring;
            std::cout << "Next point to explore :" << point.first << " " << point.second << std::endl;
        }
    }

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
            if (status_ == standBy) {
                continueExploration();
            }
            static int hello = 0;
            if (hello++ == HELLO_MESSAGE_RATE) {
                helloMessage();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    std::vector<std::pair<int, int>>

    getPointsBetween(int x1, int y1, int x2, int y2);

    enum direction {
        front,
        back,
        right,
        left,
    };

    std::pair<int, int> getPoint(direction dir);

    pointType getPointType(direction dir = front);

    void handleWallFollowing(bool collision);

    bool goToPathFinding(int x, int y);

public:
    Exp(string
        id,
        MailBox *netMailBox
    ) :

            id_(id), netMailBox_(netMailBox), runner_(&Exp::run, this) {

    }

    ~

    Exp() {
        run_ = false;
        runner_.join();
        std::cout << "Exp shutdown";
    }

    MailBox *getMailBox() {
        return &mailBox_;
    }


};


#endif //EXPLORER_EXP_H
