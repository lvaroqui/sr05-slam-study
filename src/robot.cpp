//
// Created by luc on 14/05/19.
//

#include "../include/robot.h"

void Robot::moveForward(double distance) {
    x_ += distance * cos(direction_);
    y_ += distance * sin(direction_);
}

void Robot::rotate(double angle) {
    direction_ = fmod((direction_ + angle), (2 * M_PI));
}

std::ostream &operator<<(std::ostream &os, const Robot &robot) {
    os << "Position: (" << robot.x_ << ", " << robot.y_ << ")" << std::endl;
    os << "Direction: " << robot.direction_ << " rad (" << robot.direction_ * (180 / M_PI) << " deg)" << std::endl;
    return os;
}
