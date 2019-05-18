//
// Created by luc on 17/05/19.
//

#ifndef STUDY_EXPLORER_H
#define STUDY_EXPLORER_H

#include <webots/Supervisor.hpp>
#include <webots/DistanceSensor.hpp>
#include <webots/Motor.hpp>
#include <webots/Node.hpp>

#include <iostream>
#include <limits>

#include "UDPClient.h"

using namespace webots;

class Explorer : public Supervisor {
    Node *self_;
    Motor *leftWheelMotor_;
    Motor *rightWheelMotor_;
    DistanceSensor *frontDistanceSensor_;
    UDPClient udpClient;
public:
    Explorer() : Supervisor(),
        self_(this->getSelf()),
        udpClient("localhost", "3000") {
        leftWheelMotor_ = getMotor("left wheel motor");
        rightWheelMotor_ = getMotor("right wheel motor");
        frontDistanceSensor_ = getDistanceSensor("ds7");
        frontDistanceSensor_->enable((int) getBasicTimeStep());
    }

    const double *getPosition() {
        return self_->getPosition();
    }

    double getFrontDistance() {
        return frontDistanceSensor_->getValue();
    }

    void run() {
        static int i = 0;
        auto position = this->getPosition();
        std::stringstream message;
        message << position[0] << " " << position[1] << " " << position[2] << std::endl;
        udpClient.sendMessage(message.str());
    }
};


#endif //STUDY_EXPLORER_H
