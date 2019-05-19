//
// Created by luc on 17/05/19.
//

#ifndef STUDY_EXPLORER_H
#define STUDY_EXPLORER_H

#include <webots/Supervisor.hpp>
#include <webots/DistanceSensor.hpp>
#include <webots/Motor.hpp>
#include <webots/PositionSensor.hpp>
#include <webots/Node.hpp>

#include <iostream>
#include <limits>
#include <math.h>

#include "AirplugMessage.h"
#include "UDPClient.h"
#include "UDPServer.h"

using namespace webots;
using std::string;

class Explorer : public Supervisor {
    // Robot node (useful to get information as supervisor)
    Node *self_;

    // Com
    UDPClient udpClient_;
    UDPServer udpServer_;

    // Robot motors and sensors
    Motor *leftWheelMotor_;
    Motor *rightWheelMotor_;
    PositionSensor *leftPositionSensor_;
    PositionSensor *rightPositionSensor_;
    DistanceSensor *frontDistanceSensor_;

    // Variables for controllers
    double currentLeftRotation_ = 0.0;
    double previousLeftRotation_ = 0.0;

    bool translating_ = false;
    double currentTranslation_ = 0.0;
    double targetTranslation_ = 0.0;

    bool rotating_ = false;
    double currentRotation_ = 0.0;
    double targetRotation_ = 0.0;

    const double wheelDiameter_ = 0.0825;
    const double wheelEccentricity_ = 0.18;

    //TODO: PID Class

    /// Control loop for translation
    void controlTranslation();

    /// Control loop for rotation
    void controlRotation();

public:
    /// Constructor for the Explorer
    /// \param comPort Com Port to listen to
    explicit Explorer(int comPort) : Supervisor(),
                                     self_(this->getSelf()),
                                     udpClient_("localhost", "3000"),
                                     udpServer_(comPort),
                                     leftWheelMotor_(getMotor("left wheel motor")),
                                     rightWheelMotor_(getMotor("right wheel motor")),
                                     leftPositionSensor_(getPositionSensor("left wheel sensor")),
                                     rightPositionSensor_(getPositionSensor("right wheel sensor")),
                                     frontDistanceSensor_(getDistanceSensor("ds7")) {
        frontDistanceSensor_->enable((int) getBasicTimeStep());

        leftPositionSensor_->enable((int) getBasicTimeStep());
        previousLeftRotation_ = leftPositionSensor_->getValue();


        rightWheelMotor_->setPosition(std::numeric_limits<double>::infinity());
        leftWheelMotor_->setPosition(std::numeric_limits<double>::infinity());
        rightWheelMotor_->setVelocity(0);
        leftWheelMotor_->setVelocity(0);
    }

    /// Send information about the robot to master
    void reportToMaster() {
        auto position = self_->getPosition();
        std::stringstream message;
        message << getName() << " : " << position[0] << " " << position[1] << " " << position[2] << std::endl;
        udpClient_.sendMessage(message.str());
    }

    /// Reset the controllers for Rotation and Translation
    void resetController() {
        currentTranslation_ = 0.0;
        currentRotation_ = 0.0;
        targetTranslation_ = 0.0;
        targetRotation_ = 0.0;
        rightWheelMotor_->setVelocity(0);
        leftWheelMotor_->setVelocity(0);
        translating_ = false;
        rotating_ = false;
    }

    /// Ask the robot to translate to a specific distance.
    /// \param distance Distance to go to (+ forward, - backward)
    void translate(double distance) {
        resetController();
        targetTranslation_ = distance;
        translating_ = true;
    }

    /// Ask the robot to rotate to a specific angle
    /// \param angle Angle to rotate to (- right, + left)
    void rotate(double angle) {
        resetController();
        targetRotation_ = -angle;
        rotating_ = true;
    }

    /// Handles incoming messages from master
    void handleMessages() {
        while (udpServer_.getNumberOfMessages() > 0) {
            AirplugMessage msg(udpServer_.popMessage());
            string type = msg.getValue("type");
            if (type == "translate") {
                translate(std::stod(msg.getValue("distance")));
            }
            if (type == "rotate") {
                rotate(std::stod(msg.getValue("angle")));
            }
        }
    }

    /// Called every tick
    void run() {
        // Get sensor positions
        currentLeftRotation_ = leftPositionSensor_->getValue();

        // Com
        reportToMaster();
        handleMessages();

        // Movements
        if (translating_) {
            controlTranslation();
        }
        else if (rotating_){
            controlRotation();
        }

        // Update previous sensor values
        previousLeftRotation_ = currentLeftRotation_;
    }
};


#endif //STUDY_EXPLORER_H
