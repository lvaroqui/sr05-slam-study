#include "Rob.h"
#include <iostream>


int main(int argc, char **argv) {
    // Create the Robot instance.

    // Net
    Net network(argv[1], 3000 + std::stoi(argv[1]));

    // Exp
    Exp explorer(argv[1], network.getMailBox());

    // Robot
    Rob *robot = new Rob(network.getMailBox(), network.getAirInMailBox());

    // Configure Network
    network.addAirOutMailBox(robot->getAirOutMailBox());
    network.addSubscriber("ROB", robot->getMailBox());
    network.addSubscriber("EXP", explorer.getMailBox());
    network.launch();

    int timeStep = (int) robot->getBasicTimeStep();

    // Main Loop
    while (robot->step(timeStep) != -1) {
        robot->run();
    }

    // Cleaning up
    std::cout << "Clean Up" << std::endl;
    delete robot;
    return 0;
}
