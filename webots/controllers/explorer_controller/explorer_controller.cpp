#include "Explorer.h"
#include <iostream>


int main(int argc, char **argv) {
    // Create the Robot instance.
    auto *robot = new Explorer(std::stoi(argv[1]));
    int timeStep = (int) robot->getBasicTimeStep();

    // Main Loop
    while (robot->step(timeStep) != -1) {
        robot->run();
    }

    // Cleaning up
    delete robot;
    return 0;
}
