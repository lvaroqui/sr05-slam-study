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

        bool collision = !msg.getValue("robcol").empty();

        // If a collision is detected
        if (collision) {

            std::vector<std::pair<int, int>> points;

            // Wall
            std::pair<int, int> wall = getPoint(front);

            // Adding wall to map
            map_[wall] = pointType::wall;

            // Sending observations to monitoring APP and other robots
            reportPoint(wall.first, wall.second, pointType::wall);
        } else {
            reportPosToMap();
        }
        handleWallFollowing(collision);
        if (status_ == exploring && actionsQueue.empty()) {
            std::cout << "Back to stand by" << std::endl;
            std::cout << "Position is: " << x_ << " " << y_ << std::endl;
            status_ = standBy;
        }

    } else if (ra.getType() == RobAck::curr) {
        x_ = coordToMap(ra.getCommand()[0]);
        y_ = coordToMap(ra.getCommand()[1]);
        heading_ = coordToMap(ra.getCommand()[2]);

        auto point = std::make_pair(x_, y_);
        if (map_.find(point) == map_.end() || map_[point] != pointType::explored) {
            reportPoint(point.first, point.second, pointType::explored);
            map_[std::make_pair(x_, y_)] = pointType::explored;
        }

        if (msg.getValue("inAction") == "0") {
            popActionsQueue();
        }
    }
}

void Exp::handleExpMessage(AirplugMessage msg) {
    if (msg.getValue("typemsg") == "infos") {
        Map receivedMap = fromStringToMap(msg.getValue("obs"));
        for (auto points : receivedMap) {
            map_[points.first] = points.second;
        }

        AirplugMessage mapMessage("ROB", "MAP", AirplugMessage::air);
        mapMessage.add("roback", "curr:" + std::to_string(x_) + "," + std::to_string(y_) + "," +
                                 std::to_string(heading_));
        mapMessage.add("obs", msg.getValue("obs"));
        netMailBox_->push(mapMessage);
    } else if (msg.getValue("typemsg") == "helloNeighbour") {
        // Update from a neighbour, sending us his position
        string sender = msg.getValue("sender");
        int x = std::stoi(msg.getValue("xpos"));
        int y = std::stoi(msg.getValue("ypos"));
        Neighbour nr(TTL_MAX, x, y);
        neighbours_[sender] = nr;
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

    // The remaining operations are only done if we have at least 1 neighbour registered
    if (!neighbours_.empty()) {
        // For each neigbour we have, we update their TTL
        int disappearedNeighbours = 0;
        for (auto &neighbour : neighbours_) {
            int ttlValue = --neighbour.second.ttl;
            if (ttlValue == 0)
                disappearedNeighbours++;
        }

        // We check if we are still OK or too far away from other robots
        Neighbour& bestNeighbour = closestNeighbour();

        if (disappearedNeighbours == neighbours_.size()) {
            // We don't have any neighbour anymore : we must come back to the closest one
            // Note: we artificially reset its TTL in order to keep it in the list
//            bestNeighbour.ttl = TTL_MAX;
            status_ = joiningNeighboor;
            goToPathFinding(bestNeighbour.x, bestNeighbour.y);
        }

        // Finally, if some neighbours have disappeared, we clean them
        if (disappearedNeighbours) {
            for (auto it = neighbours_.begin(); it != neighbours_.end();) {
                if (it->second.ttl == 0) {
                    it = neighbours_.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }
}

Exp::Neighbour & Exp::closestNeighbour() {
    string closestNeighbourName;
    float shortestDistance = std::numeric_limits<float>::max();
    for (auto & neighbour : neighbours_) {
        float distance = sqrt(pow(neighbour.second.x - x_, 2) + pow(neighbour.second.y - y_, 2));
        if (distance < shortestDistance) {
            closestNeighbourName = neighbour.first;
            shortestDistance = distance;
        }
    }
    return neighbours_[closestNeighbourName];
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

void Exp::reportPosToMap() {
    AirplugMessage mapMessage("ROB", "MAP", AirplugMessage::air);
    mapMessage.add("roback", "curr:" + std::to_string(x_) + "," + std::to_string(y_) + "," +
                             std::to_string(heading_));
    mapMessage.add("obs", "");
    netMailBox_->push(mapMessage);
}

void Exp::reportPointToMap(int x, int y, pointType type) {
    AirplugMessage mapMessage("ROB", "MAP", AirplugMessage::air);
    mapMessage.add("roback", "curr:" + std::to_string(x_) + "," + std::to_string(y_) + "," +
                             std::to_string(heading_));
    mapMessage.add("obs", "");
    netMailBox_->push(mapMessage);
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

void Exp::popActionsQueue() {
    if (!actionsQueue.empty()) {
        AirplugMessage msgOrd("EXP", "ROB", AirplugMessage::local);
        msgOrd.add("robord", actionsQueue.front());
        actionsQueue.pop();
        netMailBox_->push(msgOrd);
    }
}

void Exp::clearActionsQueue() {
    while (!actionsQueue.empty()) {
        actionsQueue.pop();
    }
}

pointType Exp::getPointType(Exp::direction dir) {
    std::pair<int, int> point = getPoint(dir);
    if (map_.find(point) == map_.end()) {
        return pointType::unexplored;
    } else {
        return map_[point];
    }
}

std::pair<int, int> Exp::getPoint(Exp::direction dir) {
    double angle = heading_ * M_PI / 180;
    switch (dir) {
        case front:
            break;
        case back:
            angle = -angle;
            break;
        case right:
            angle += (M_PI / 2);
            break;
        case left:
            angle -= (M_PI / 2);
            break;
    }
    std::pair<int, int> point = std::make_pair(
            x_ + floorNeg(1.5 * cos(angle)),
            y_ + floorNeg(1.5 * sin(angle)));
    return point;
}

void Exp::handleWallFollowing(bool collision) {
    if (status_ == followingWall) {
        // On a longé le mur "tout droit"
        if (collision && actionsQueue.empty() && getPointType(right) != wall) {
            if (getPointType(left) != unexplored) {
                status_ = standBy;
            } else {
                actionsQueue.push("turn:90");
                actionsQueue.push("move:50");
                actionsQueue.push("turn:-90");
                actionsQueue.push("move:50");
            }
        }
            // On est arrivé à un bord "sortant" du mur
        else if (actionsQueue.empty() && getPointType(right) == wall) {
            status_ = standBy;
        }
            // On est dans un coin
        else if (collision && actionsQueue.size() == 2) {
            auto wall = getPoint(right);
            reportPoint(wall.first, wall.second, pointType::wall);
            clearActionsQueue();
            if (getPointType(left) != unexplored) {
                status_ = standBy;
            } else {
                actionsQueue.push("turn:90");
                actionsQueue.push("move:50");
                actionsQueue.push("turn:-90");
                actionsQueue.push("move:50");
            }
        }
    }
}

bool Exp::goToPathFinding(int x, int y) {
    clearActionsQueue();
    Pathfinder pathfinder;
    pathfinder.mapToNodeMap(map_);
    auto path = pathfinder.findPath(std::make_pair(x_, y_), std::make_pair(x, y));
    if (path.empty()) {
        return false;
    }
    for (auto point : path) {
        if (x_ != point.first || y_ != point.second) {
            actionsQueue.push("join:" + std::to_string(point.first * 50) + "," + std::to_string(point.second * 50));
        }
    }
    return true;
}
