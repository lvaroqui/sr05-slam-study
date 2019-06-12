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
		    clock_[id_]++;
       
            std::vector<std::pair<int, int>> points;

            // Wall
            std::pair<int, int> wall = getPoint(front);

            // Adding wall to map
            map_[wall] = pointType::wall;

            // Sending observations to monitoring APP and other robots
            reportPoint(wall.first, wall.second, pointType::wall);
        }
        else {
            reportPosToMap();
        }
        handleWallFollowing(collision);

    }
    else if (ra.getType() == RobAck::curr) {
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
    

	//Update your clocks according to the received clock
	std::map<std::string, int> receivedClock = fromStringToMapClock(msg.getValue("clk"));
	for (auto clk : receivedClock) {
		if (clock_.find(clk.first) != clock_.end() && clock_[clk.first] < clk.second){
			clock_[clk.first] = clk.second;
		}
		else if(clock_.find(clk.first) == clock_.end()){
			clock_[clk.first] = clk.second;
		}
	}	
    if (msg.getValue("typemsg") == "infos") {
		//Increment your clock
		clock_[id_]++;
        Map receivedMap = fromStringToMap(msg.getValue("obs"));
        map_.insert(receivedMap.begin(), receivedMap.end());
    } else if(msg.getValue("typemsg") == "helloNeighbour") {
        // Update from a neighbour, sending us his position
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
    msg.add("clk", fromMapToStringClock(clock_));

    netMailBox_->push(msg);

    // The remaining operations are only done if we have at least 1 neighbour registered
    if(neighbours_.size() > 0) {
        // For each neigbour we have, we update their TTL
        int disappearedNeighbours = 0;
        for(auto it = neighbours_.begin(); it != neighbours_.end(); ++it) {
            int ttlValue = --it->second.first;
            if(ttlValue == 0)
                disappearedNeighbours++;
        }

        // We check if we are still OK or too far away from other robots
        auto bestNeighbour = closestNeighbour();
        if(disappearedNeighbours == neighbours_.size()) {
            // We don't have any neighbour anymore : we must come back to the closest one
            // Note: we artificially reset its TTL in order to keep it in the list
            neighbours_[bestNeighbour.first].second.first = TTL_MAX;
            goTowardsNeighbour(bestNeighbour);
        } else {
            // We still have at least 1 neighbour : we check if we are not too far away from the closest one
            if(bestNeighbour.second > WARNING_DISTANCE) {
                // We are too far away : we have to go back towards him
                goTowardsNeighbour(bestNeighbour);
            }
        }

        // Finally, if some neighbours have disappeared, we clean them
        if(disappearedNeighbours) {
            for(auto it = neighbours_.begin(); it != neighbours_.end(); ) {
                if(it->second.first == 0) {
                    it = neighbours_.erase(it);
                } else {
                    ++it;
                }
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

void Exp::goTowardsNeighbour(std::pair<string, float> neighbour) {
    // We collect the neighbour's last known coordinates
    int neighbourX = neighbours_[neighbour.first].second.first;
    int neighbourY = neighbours_[neighbour.first].second.second;

    // We compute the coordinates we want to go to (because of the safe distance to avoid collision)
    float currentDistance = neighbour.second;
    float ratio = SAFE_DISTANCE / currentDistance;
    int safeXDistance = static_cast<int>(round((x_-neighbourX) * ratio));
    int safeYDistance = static_cast<int>(round((y_-neighbourY) * ratio));
    int safeX = neighbourX + safeXDistance;
    int safeY = neighbourY + safeYDistance;

    // We create the message then send it to ROB
    AirplugMessage msg("EXP", "ROB", AirplugMessage::local);
    string command = "join:" + std::to_string(safeX) + "," + std::to_string(safeY);
    msg.add("robord", command);
    netMailBox_->push(msg);
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
    }
    else {
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
    if (followWall_) {
        // On a longé le mur "tout droit"
        if (collision && actionsQueue.empty() && getPointType(right) != wall) {
            if (getPointType(left) != unexplored) {
                followWall_ = false;
            }
            else {
                actionsQueue.push("turn:90");
                actionsQueue.push("move:50");
                actionsQueue.push("turn:-90");
                actionsQueue.push("move:50");
            }
        }
            // On est arrivé à un bord "sortant" du mur
        else if (actionsQueue.empty() && getPointType(right) == wall) {
            if (getPointType(front) != unexplored) {
                followWall_ = false;
            }
            else {
                actionsQueue.push("turn:-90");
                actionsQueue.push("move:50");
            }

        }
            // On est dans un coin
        else if (collision && actionsQueue.size() == 2) {
            auto wall = getPoint(right);
            reportPoint(wall.first, wall.second, pointType::wall);
            clearActionsQueue();
            if (getPointType(left) != unexplored) {
                followWall_ = false;
            }
            else {
                actionsQueue.push("turn:90");
                actionsQueue.push("move:50");
                actionsQueue.push("turn:-90");
                actionsQueue.push("move:50");
            }
        }
    }
}
