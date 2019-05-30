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
#include "RobOrd.h"
#include "Network.h"
#include "RobAck.h"

using namespace webots;
using std::string;

class Explorer : public Supervisor {
    // Robot node (useful to get information as supervisor)
    Node *self_;

    // Network
    Network net_;

    // Robot motors and sensors
    Motor *leftWheelMotor_;
    Motor *rightWheelMotor_;
    PositionSensor *leftPositionSensor_;
    PositionSensor *rightPositionSensor_;
    DistanceSensor *frontDistanceSensor_;

    // Current position
    double x_ = 0.0;
    double y_ = 0.0;
    double heading_ = 0.0;

    // Variables for controllers
    double currentLeftRotation_ = 0.0;
    double previousLeftRotation_ = 0.0;

    bool translating_ = false;
    double currentTranslation_ = 0.0;
    double targetTranslation_ = 0.0;

    bool rotating_ = false;
    double initialHeading_ = 0.0;
    double targetHeading_ = 0.0;

    const double wheelDiameter_ = 0.0825;
    const double wheelEccentricity_ = 0.18;

    /// Control loop for translation
    void controlTranslation();

    /// Control loop for rotation
    void controlRotation();

public:
    /// Constructor for the Explorer
    /// \param comPort Com Port to listen to
    explicit Explorer(int comPort) : Supervisor(),
                                     self_(this->getSelf()),
                                     net_(getName(), comPort),
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

private:
    /// Reset the controllers for Rotation and Translation
    void resetController() {
        currentTranslation_ = 0.0;
        targetTranslation_ = 0.0;
        targetHeading_ = 0.0;
        rightWheelMotor_->setVelocity(0);
        leftWheelMotor_->setVelocity(0);
        translating_ = false;
        rotating_ = false;
    }

    /// Ask the robot to translate to a specific distance.
    /// \param distance Distance in meters to go to (+ forward, - backward)
    void translate(double distance) {
        resetController();
        targetTranslation_ = distance;
        translating_ = true;
    }

    /// Ask the robot to rotate to a specific angle.
    /// \param angle Angle in radians to rotate to (- right, + left)
    void rotate(double angle) {
        resetController();
        initialHeading_ = heading_;
        targetHeading_ = heading_ - angle;
        rotating_ = true;
    }

    void stop() {
        rightWheelMotor_->setVelocity(0);
        leftWheelMotor_->setVelocity(0);
    }

    /// Handles incoming messages
    void handleMessages() {
        while (net_.getNumberOfRobotMessages() > 0) {
            RobOrd robord(net_.popRobotMessage().getValue("robord"));
            switch (robord.getType()) {
                case RobOrd::Type::move :
                    translate(static_cast<double>(robord.getCommand()[0])/100.0);
                    break;
                case RobOrd::Type::turn :
                    rotate(static_cast<double>(robord.getCommand()[0]) * M_PI / 180.0);
                    break;
                case RobOrd::tune:break;
                case RobOrd::lacc:break;
                case RobOrd::init:
                    x_ = static_cast<double>(robord.getCommand()[0])/100.0;
                    y_ = static_cast<double>(robord.getCommand()[1])/100.0;
                    heading_ = static_cast<double>(robord.getCommand()[2]) * M_PI / 180.0;
                    break;
                case RobOrd::curr:break;
                case RobOrd::join:break;
                case RobOrd::undefinied:
                    break;
            }
        }
    }

public:
    /// Called every tick
    void run() {
        // Get sensor positions
        currentLeftRotation_ = leftPositionSensor_->getValue();

        // Estimate position
        if (rotating_) {
            heading_ += atan2((currentLeftRotation_ - previousLeftRotation_) * wheelDiameter_, wheelEccentricity_);
            if (abs(targetHeading_ - heading_) < 0.001) {
                net_.sendMessage(RobAck::turnedMsg(int(round((initialHeading_ - targetHeading_) * 180.0 / M_PI))));
                rotating_ = false;
                stop();
            }
            else {
                controlRotation();
            }
        }
        else if (translating_) {
            double distance = (currentLeftRotation_ - previousLeftRotation_) * wheelDiameter_;
            x_ += cos(heading_) * distance;
            y_ += sin(heading_) * distance;
            if (abs(currentTranslation_ - targetTranslation_) < 0.001) {
                net_.sendMessage(RobAck::turnedMsg(int(round(currentTranslation_*100))));
                translating_ = false;
                stop();
            }
            else {
                controlTranslation();
            }
        }

        // Com
        handleMessages();

        // Update previous sensor values
        previousLeftRotation_ = currentLeftRotation_;
    }
};


#endif //STUDY_EXPLORER_H
