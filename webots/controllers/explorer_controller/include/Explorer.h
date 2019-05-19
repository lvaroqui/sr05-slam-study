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
#include "UDPServer.h"

using namespace webots;

class Explorer : public Supervisor {
    Node *self_;
    UDPClient udpClient_;
    UDPServer udpServer_;
    Motor *leftWheelMotor_;
    Motor *rightWheelMotor_;
    DistanceSensor *frontDistanceSensor_;

public:
    explicit Explorer(int comPort) : Supervisor(),
                                     self_(this->getSelf()),
                                     udpClient_("localhost", "3000"),
                                     udpServer_(comPort),
                                     leftWheelMotor_(getMotor("left wheel motor")),
                                     rightWheelMotor_(getMotor("right wheel motor")),
                                     frontDistanceSensor_(getDistanceSensor("ds7")) {
        frontDistanceSensor_->enable((int) getBasicTimeStep());
    }

    double getFrontDistance() {
        return frontDistanceSensor_->getValue();
    }

    void reportToMaster() {
        auto position = self_->getPosition();
        std::stringstream message;
        message << getName() << " : " << position[0] << " " << position[1] << " " << position[2] << std::endl;
        udpClient_.sendMessage(message.str());
    }

    void run() {
        reportToMaster();
        while (udpServer_.getNumberOfMessages() > 0) {
            std::cout << udpServer_.popMessage();
        }
    }
};


#endif //STUDY_EXPLORER_H
