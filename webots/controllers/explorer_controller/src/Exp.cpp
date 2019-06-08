//
// Created by luc on 03/06/19.
//

#include "Exp.h"

void Exp::handleRobMessage(AirplugMessage msg) {
    RobAck ra(msg.getValue("roback"));
    if (ra.getType() == RobAck::joined || ra.getType() == RobAck::moved ||
        ra.getType() == RobAck::turned) {

        // Getting robot position
        std::vector<string> tmp;
        boost::split(tmp, msg.getValue("robotpos"), [](char c) { return c == ','; });
        x_ = std::stoi(tmp[0]);
        y_ = std::stoi(tmp[1]);
        heading_ = std::stoi(tmp[2]);

        AirplugMessage mapMessage("ROB", "MAP", AirplugMessage::air);
        mapMessage.add("roback", "curr:" + std::to_string(x_) + "," + std::to_string(y_) + "," +
                                 std::to_string(heading_));

        // If a collision is detected
        if (!msg.getValue("robcol").empty()) {
            std::vector<std::pair<int, int>> points;
            int xWall = x_ + 14 * cos(heading_ * M_PI / 180);
            int yWall = y_ + 14 * sin(heading_ * M_PI / 180);
            for (int i = -10; i <= 10; i++) {
                std::pair<int, int> point(xWall + i * cos((heading_ + 90) * M_PI / 180),
                                          yWall + i * sin((heading_ + 90) * M_PI / 180));
                auto it = find(points.begin(), points.end(), point);
                if (it == points.end()) {
                    points.push_back(point);
                }
            }

            // Adding points to local map
            addPoints(points);

            mapMessage.add("obs", fromVectorOfPairsToString(points));
            AirplugMessage expMessage("EXP", "EXP", AirplugMessage::air);
            expMessage.add("typemsg", "collisionDetection");
            expMessage.add("currentpos", std::to_string(x_) + "," + std::to_string(y_) + "," +
                                         std::to_string(heading_));
            expMessage.add("obs", fromVectorOfPairsToString(points));
            netMailBox_->push(expMessage);
        }
        netMailBox_->push(mapMessage);
    }
    else if (ra.getType() == RobAck::curr) {
        x_ = ra.getCommand()[0];
        y_ = ra.getCommand()[1];
        heading_ = ra.getCommand()[2];
    }
}

void Exp::handleExpMessage(AirplugMessage msg) {
    if (msg.getValue("typemsg") == "collisionDetection") {
        addPoints(fromStringToVectorOfPairs(msg.getValue("obs")));
    }
}

void Exp::addPoints(std::vector<std::pair<int, int>> points) {
    points_.reserve(points.size());
    points_.insert(points_.end(), points.begin(), points.end());
    sort(points_.begin(), points_.end());
    points_.erase(unique(points_.begin(), points_.end()), points_.end());
}
