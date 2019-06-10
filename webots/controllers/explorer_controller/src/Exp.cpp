//
// Created by luc on 03/06/19.
//

#include "Exp.h"

void Exp::handleRobMessage(AirplugMessage msg) {
    RobAck ra(msg.getValue("roback"));

    // Handling movement ack messages
    if (ra.getType() == RobAck::joined || ra.getType() == RobAck::moved ||
        ra.getType() == RobAck::turned) {

        // Getting robot position
        std::vector<string> tmp;
        boost::split(tmp, msg.getValue("robotpos"), [](char c) { return c == ','; });
        x_ = std::stoi(tmp[0]);
        y_ = std::stoi(tmp[1]);
        heading_ = std::stoi(tmp[2]);

        // Preparing message for monitor APP
        AirplugMessage mapMessage("ROB", "MAP", AirplugMessage::air);
        mapMessage.add("roback", "curr:" + std::to_string(x_) + "," + std::to_string(y_) + "," +
                                 std::to_string(heading_));

        // If a collision is detected
        if (!msg.getValue("robcol").empty()) {
            std::vector<std::pair<int, int>> points;

            // Main contact point
            int xWall = x_ + 14 * cos(heading_ * M_PI / 180);
            int yWall = y_ + 14 * sin(heading_ * M_PI / 180);

            // Calculating points adjacent to the contact main point
            for (int i = -10; i <= 10; i++) {
                std::pair<int, int> point(xWall + i * cos((heading_ + 90) * M_PI / 180),
                                          yWall + i * sin((heading_ + 90) * M_PI / 180));
                auto it = find(points.begin(), points.end(), point);
                if (it == points.end()) {
                    points.push_back(point);
                }
            }

            // Adding wall to map
            for (auto point : points) {
                map_[point] = pointType::wall;
                unsentMap_[point] = pointType::wall;
            }

            // Sending observations to monitoring APP
            mapMessage.add("obs", fromMapToString(unsentMap_));

            // Preparing message for other robots
            AirplugMessage expMessage("EXP", "EXP", AirplugMessage::air);
            expMessage.add("typemsg", "collisionDetection");
            expMessage.add("currentpos", std::to_string(x_) + "," + std::to_string(y_) + "," +
                                         std::to_string(heading_));
            expMessage.add("obs", fromMapToString(unsentMap_));
            netMailBox_->push(expMessage);
        }
        netMailBox_->push(mapMessage);

        // No more pending messages, we clear the temporary Map
        unsentMap_.clear();
    }
    else if (ra.getType() == RobAck::curr) {
        int previousX = x_;
        int previousY = y_;
        x_ = ra.getCommand()[0];
        y_ = ra.getCommand()[1];
        heading_ = ra.getCommand()[2];

        if (inited && (x_ != previousX || y_ != previousY)) {
            std::vector<std::pair<int, int>> points;
            auto passedBy = getPointsBetween(x_, y_, previousX, previousY);

            for (auto passedPoint : passedBy) {
                std::cout << passedPoint.first << " " << passedPoint.second << std::endl;
                for (int i = -10; i <= 10; i++) {
                    std::pair<int, int> point(passedPoint.first + i * cos((heading_ + 90) * M_PI / 180),
                                              passedPoint.second + i * sin((heading_ + 90) * M_PI / 180));
                    points.push_back(point);
                }
            }
            for (auto point : points) {
                unsentMap_[point] = pointType::explored;
                map_[point] = pointType::explored;
            }
        }
        else {
            inited = true;
        }
    }
}

void Exp::handleExpMessage(AirplugMessage msg) {
    if (msg.getValue("typemsg") == "collisionDetection") {
        Map receivedMap = fromStringToMap(msg.getValue("obs"));
        map_.insert(receivedMap.begin(), receivedMap.end());
    } else if(msg.getValue("typemsg") == "helloNeighbour") {
        // Update from a neighbour, sending us is position
        string sender = msg.getValue("sender");
        int x = std::stoi(msg.getValue("xpos"));
        int y = std::stoi(msg.getValue("ypos"));
        std::pair<int, int> pos(x, y);
        std::pair<int, std::pair<int, int>> neighbour(TTL_MAX, pos);
        neighbours_[sender] = neighbour;
    }
}

void Exp::handleMapMessage(AirplugMessage msg) {
    if (msg.getValue("typemsg") == "MAPCO") {
        AirplugMessage message("NET", "MAP", AirplugMessage::air);
        message.add("typemsg", "ROBCO");
        message.add("xpos", std::to_string(x_));
        message.add("ypos", std::to_string(y_));
        message.add("heading", std::to_string(heading_));
        message.add("obs", fromMapToString(map_));
        netMailBox_->push(message);
    }
}

void updateAndCheckNeighbours() {
    // First, we send a message to our neighbours telling them we're still here
    AirplugMessage msg("EXP", "EXP", AirplugMessage::air);
    msg.add("sender", id_);
    msg.add("dest", "neighbourhood");
    msg.add("typemsg", "helloNeighbour");
    msg.add("xpos", std::to_string(x_));
    msg.add("ypos", std::to_string(y_));
    netMailBox_->push(msg);

    // Then, for each neigbour we have, we update their TTL
    int disappearedNeighbours = 0;
    for(auto it = neighbours_.begin(); it != neighbours_.end(); ++it) {
        int ttlValue = --it->second->first;
        if(ttlValue == 0)
            disappearedNeighbours++;
    }

    // We check if we are still OK or too far away from other robots    
    if(disappearedNeighbours == neighbours_.size()) {
        // We don't have any neighbour anymore : we must come back to the closest one
        auto bestNeighbour = closestNeighbour();
        //TODO : get back towards him
    } else {
        // We still have at least 1 neighbour : we check if we are not too far away from the closest one
        auto closestNeighbor = closestNeighbour();
        //TODO : check if closestNeighbor->second > WARNING_DISTANCE (to define)
        //and, if so, get back towards him
    }

    // Finally, if some neighbours have disappeared, we clean them
    if(disappearedNeighbours) {
        // Some neighbours (but not all) have disappeared : we remove them
        for(auto it = neighbours_.begin(); it != neighbours_.end(); ) {
            if(it->second->first == 0) {
                it = neighbours_.erase(it);
            } else {
                ++it;
            }
        }
    }
}

std::pair<string, float> closestNeighbour() {
    string closestNeighbourName = "";
    float shortestDistance = std::numeric_limits<float>::max;
    for(auto it = neighbours_.begin(); it != neighbours_.end(); ++it) {
        float distance = sqrt(pow(it->second->second->first-x_, 2) + pow(it->second->second->second-y_, 2));
        if(distance < shortestDistance) {
            closestNeighbourName = it->first;
            shortestDistance = distance;
        }
    }
    pair<string, float> closest(closestNeighbourName, shortestDistance);
    return closest;
}

std::vector<std::pair<int, int>> Exp::getPointsBetween(int x1, int y1, int x2, int y2) {
    std::vector<std::pair<int, int>> points;
    if (x1 == x2) {
        if (y1 > y2) {
            int c = y1;
            y1 = y2;
            y2 = c;
        }
        for (int i = y1; i <= y2; i++) {
            points.emplace_back(x1, i);
        }

    }
    else {
        if (x1 > x2) {
            int c = x1;
            x1 = x2;
            x2 = c;
            c = y1;
            y1 = y2;
            y2 = c;
        }
        double a = (double) (y2 - y1) / (double) (x2 - x1);
        double b = y1 - a * x1;
        for (int i = x1; i <= x2; i++) {
            points.emplace_back(i, round(a * i + b));
        }

    }
    return points;
}
