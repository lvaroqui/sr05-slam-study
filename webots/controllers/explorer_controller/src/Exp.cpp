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
        x_ = coordToMap(std::stoi(tmp[0]));
        y_ = coordToMap(std::stoi(tmp[1]));
        heading_ = std::stoi(tmp[2]);

        // Preparing message for monitor APP

        // If a collision is detected
        if (!msg.getValue("robcol").empty()) {
            std::vector<std::pair<int, int>> points;

            // Wall
            std::pair<int, int> wall = std::make_pair(
                    x_ + floorNeg(1.5 * cos(heading_ * M_PI / 180)),
                    y_ + floorNeg(1.5 * sin(heading_ * M_PI / 180))
            );

            // Adding wall to map
            map_[wall] = pointType::wall;

            // Sending observations to monitoring APP and other robots
            reportPoint(wall.first, wall.second, pointType::wall);
        }
        else {
            AirplugMessage mapMessage("ROB", "MAP", AirplugMessage::air);
            mapMessage.add("roback", "curr:" + std::to_string(x_) + "," + std::to_string(y_) + "," +
                                     std::to_string(heading_));
            mapMessage.add("obs", "");
            netMailBox_->push(mapMessage);
        }


        // No more pending messages, we clear the temporary Map
        unsentMap_.clear();
    } else if (ra.getType() == RobAck::curr) {
        x_ = coordToMap(ra.getCommand()[0]);
        y_ = coordToMap(ra.getCommand()[1]);
        heading_ = coordToMap(ra.getCommand()[2]);

        auto point = std::make_pair(x_, y_);
        if (map_.find(point) == map_.end() || map_[point] != pointType::explored) {
            reportPoint(point.first, point.second, pointType::explored);
            map_[std::make_pair(x_, y_)] = pointType::explored;
        }
    }
}

void Exp::handleExpMessage(AirplugMessage msg) {
    if (msg.getValue("typemsg") == "infos") {
        Map receivedMap = fromStringToMap(msg.getValue("obs"));
        map_.insert(receivedMap.begin(), receivedMap.end());
    } else if (msg.getValue("typemsg") == "helloNeighbour") {
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

void Exp::updateAndCheckNeighbours() {
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
    for (auto it = neighbours_.begin(); it != neighbours_.end(); ++it) {
        int ttlValue = --it->second.first;
        if (ttlValue == 0)
            disappearedNeighbours++;
    }

    // We check if we are still OK or too far away from other robots    
    if (disappearedNeighbours == neighbours_.size()) {
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
    if (disappearedNeighbours) {
        // Some neighbours (but not all) have disappeared : we remove them
        for (auto it = neighbours_.begin(); it != neighbours_.end();) {
            if (it->second.first == 0) {
                it = neighbours_.erase(it);
            } else {
                ++it;
            }
        }
    }
}

std::pair<string, float> Exp::closestNeighbour() {
    string closestNeighbourName = "";
    float shortestDistance = std::numeric_limits<float>::max();
    for (auto it = neighbours_.begin(); it != neighbours_.end(); ++it) {
        float distance = sqrt(pow(it->second.second.first - x_, 2) + pow(it->second.second.second - y_, 2));
        if (distance < shortestDistance) {
            closestNeighbourName = it->first;
            shortestDistance = distance;
        }
    }
    std::pair<string, float> closest(closestNeighbourName, shortestDistance);
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

    } else {
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

void Exp::reportPoint(int x, int y, pointType type) {
    AirplugMessage mapMessage("ROB", "MAP", AirplugMessage::air);
    mapMessage.add("roback", "curr:" + std::to_string(x_) + "," + std::to_string(y_) + "," +
                             std::to_string(heading_));
    mapMessage.add("obs", std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(type));
    netMailBox_->push(mapMessage);

    AirplugMessage expMessage("EXP", "EXP", AirplugMessage::air);
    expMessage.add("typemsg", "infos");
    expMessage.add("obs", std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(type));
    netMailBox_->push(expMessage);
}
