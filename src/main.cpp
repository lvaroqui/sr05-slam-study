#include <iostream>
#include "../include/robot.h"

int main() {
    Robot myRobot(0, 0, 0);
    std::cout << myRobot;
    myRobot.moveForward(10);
    std::cout << myRobot;
    myRobot.rotate(M_PI/2);
    myRobot.moveForward(10);
    myRobot.rotate(M_PI/2);
    myRobot.moveForward(10);
    myRobot.rotate(M_PI/2);
    myRobot.moveForward(10);
    std::cout << myRobot;
    return 0;
}