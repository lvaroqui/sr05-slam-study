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
#include <webots/Emitter.hpp>
#include <webots/Receiver.hpp>

#include <iostream>
#include <limits>
#include <math.h>

#include "Net.h"
#include "Exp.h"
#include "com/AirplugMessage.h"
#include "com/UDPClient.h"
#include "com/UDPServer.h"
#include "com/RobOrd.h"
#include "com/RobAck.h"

using namespace webots;
using std::string;

class Rob : public Supervisor {
    // Robot node (useful to get information as supervisor)
    Node *self_;

    //MailBox
    MailBox *netMailBox_;
    MailBox *airInMailBox_;
    MailBox airOutMailBox_;
    MailBox mailBox_;

    // Robot motors and sensors
    Motor *leftWheelMotor_;
    Motor *rightWheelMotor_;
    PositionSensor *leftPositionSensor_;
    PositionSensor *rightPositionSensor_;
    Emitter *emitter_;
    Receiver *receiver_;

    std::vector<DistanceSensor *> frontDistanceSensors_;
    // Current position
    double x_ = 0.0;
    double y_ = 0.0;
    double heading_ = 0.0;
    bool collision_ = false;

    // Variables for controllers
    double currentLeftRotation_ = 0.0;
    double previousLeftRotation_ = 0.0;

    double currentRightRotation_ = 0.0;
    double previousRightRotation_ = 0.0;

    bool translating_ = false;
    double currentTranslation_ = 0.0;
    double targetTranslation_ = 0.0;

    bool rotating_ = false;
    double initialHeading_ = 0.0;
    double targetHeading_ = 0.0;

    bool joining_ = false;

    const double wheelDiameter_ = 0.0825;
    const double wheelEccentricity_ = 0.18;

    /// Control loop for translation
    void controlTranslation();

    /// Control loop for rotation
    void controlRotation();

public:
    /// Constructor for the Explorer
    explicit Rob(MailBox *netMailBox, MailBox *airInMailBox) : Supervisor(),
                                        self_(this->getSelf()),
                                        netMailBox_(netMailBox),
                                        airInMailBox_(airInMailBox),
                                        emitter_(getEmitter("emitter")),
                                        receiver_(getReceiver("receiver")),
                                        leftWheelMotor_(getMotor("left wheel motor")),
                                        rightWheelMotor_(getMotor("right wheel motor")),
                                        leftPositionSensor_(getPositionSensor("left wheel sensor")),
                                        rightPositionSensor_(getPositionSensor("right wheel sensor")) {
        frontDistanceSensors_.push_back(getDistanceSensor("ds0"));
        frontDistanceSensors_.push_back(getDistanceSensor("ds1"));
        frontDistanceSensors_.push_back(getDistanceSensor("ds2"));
        frontDistanceSensors_.push_back(getDistanceSensor("ds13"));
        frontDistanceSensors_.push_back(getDistanceSensor("ds14"));
        frontDistanceSensors_.push_back(getDistanceSensor("ds15"));

        for (auto distanceSensor : frontDistanceSensors_) {
            distanceSensor->enable((int) getBasicTimeStep());
        }

        leftPositionSensor_->enable((int) getBasicTimeStep());
        rightPositionSensor_->enable((int) getBasicTimeStep());
        receiver_->enable((int) getBasicTimeStep());

        rightWheelMotor_->setPosition(std::numeric_limits<double>::infinity());
        leftWheelMotor_->setPosition(std::numeric_limits<double>::infinity());
        rightWheelMotor_->setVelocity(0);
        leftWheelMotor_->setVelocity(0);
        auto position = self_->getPosition();
        x_ = position[2];
        y_ = position[0];
        heading_ = 0;
    }

private:
    double modAngle(double angle) {
        if (angle > M_PI) {
            return angle - 2 * M_PI;
        } else if (angle < -M_PI) {
            return angle + 2 * M_PI;
        }
        return angle;
    }

    double saturate(double value, double saturation) {
        if (value > saturation) {
            return saturation;
        } else if (value < -saturation) {
            return -saturation;
        }
        return value;
    }

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
        targetHeading_ = modAngle(targetHeading_);
        rotating_ = true;
    }

    void join(double x, double y) {
        resetController();
        targetHeading_ = atan2(y - y_, x - x_);
        targetHeading_ = modAngle(targetHeading_);
        targetTranslation_ = sqrt(pow(x - x_, 2) + pow(y - y_, 2));
        joining_ = true;
        rotating_ = true;
    }

    void stop() {
        rightWheelMotor_->setVelocity(0);
        leftWheelMotor_->setVelocity(0);
    }

    /// Handles incoming messages
    void handleMessages();

    void handleInterRobotCommunications() {
        while(receiver_->getQueueLength() > 0) {
            const char * messageRaw = (const char *)receiver_->getData();
            AirplugMessage message((string(messageRaw)));
            airInMailBox_->push(message);
            receiver_->nextPacket();
        }
        if (airOutMailBox_.size() > 0) {
            string message = airOutMailBox_.pop().serialize();
            emitter_->send(message.c_str(), message.length() + 1);
        }
    }

public:
    MailBox *getMailBox() {
        return &mailBox_;
    }

    MailBox *getAirOutMailBox() {
        return &airOutMailBox_;
    }

    /// Called every tick
    void run() {
        // Get sensor positions
        currentLeftRotation_ = leftPositionSensor_->getValue();
        currentRightRotation_ = rightPositionSensor_->getValue();

        double speedLeft = currentLeftRotation_ - previousLeftRotation_;
        double speedRight = currentRightRotation_ - previousRightRotation_;

        // Translating
        if (speedLeft * speedRight >= 0) {
            double distance = (currentLeftRotation_ - previousLeftRotation_) * wheelDiameter_;
            x_ += cos(heading_) * distance;
            y_ += sin(heading_) * distance;
        } else { // Rotating
            heading_ += atan2((currentLeftRotation_ - previousLeftRotation_) * wheelDiameter_, wheelEccentricity_);
            heading_ = modAngle(heading_);
        }


        //  If robot is executing a rotating order
        if (rotating_) {
            // Checking if we reached the desired heading_
            if (abs(targetHeading_ - heading_) < 0.001) {
                // If we are in a joining operation we then go to the translating part
                if (joining_) {
                    translating_ = true;
                }
                    // If we were rotating, acknowledging with turned
                else {
                    auto msg = RobAck::turnedMsg(int(round(modAngle(initialHeading_ - targetHeading_) * 180.0 / M_PI)));
                    RobAck::addRobotPosMsg(msg, static_cast<int>(x_ * 100), static_cast<int>(y_ * 100),
                                           static_cast<int>(heading_ * 180.0 / M_PI));
                    netMailBox_->push(msg);
                }
                rotating_ = false;
                stop();
            } else {
                // Controller of the rotation
                controlRotation();
            }
        }
            // If robot is executing a translating order
        else if (translating_) {
            // Finding minimum distance from obstacles
            double minDistance = 0.0;
            for (auto distanceSensor : frontDistanceSensors_) {
                if (distanceSensor->getValue() > minDistance) {
                    minDistance = distanceSensor->getValue();
                }
            }

            // Detecting collisions
            if (minDistance > 950 && currentTranslation_ < targetTranslation_ && !collision_) {
                targetTranslation_ = currentTranslation_ + 0.1;
                collision_ = true;
            }

            // Checking if we reached the desired position
            if (abs(currentTranslation_ - targetTranslation_) < 0.001 && speedLeft < 0.01) {
                AirplugMessage msg;

                // If we were joining, acknowledging with joined
                if (joining_) {
                    msg = RobAck::joinedMsg(static_cast<int>(x_ * 100), static_cast<int>(y_ * 100));
                    joining_ = false;
                }
                    // If we were moving, acknowledging with moved
                else {
                    msg = RobAck::movedMsg(int(round(currentTranslation_ * 100)));
                }
                if (collision_) {
                    msg.add("robcol", "1");
                    collision_ = false;
                }
                RobAck::addRobotPosMsg(msg, static_cast<int>(x_ * 100), static_cast<int>(y_ * 100),
                                       static_cast<int>(heading_ * 180.0 / M_PI));
                netMailBox_->push(msg);
                translating_ = false;
                stop();
            } else {
                // Control translation
                controlTranslation();
            }
        }

        // Com
        static int i = 30;
        if (i++ == 30) {
            netMailBox_->push(RobAck::currMsg(static_cast<int>(x_ * 100), static_cast<int>(y_ * 100),
                                        static_cast<int>(heading_ * 180.0 / M_PI)));
            i = 0;
        }
        handleInterRobotCommunications();
        handleMessages();

        // Update previous sensor values
        previousLeftRotation_ = currentLeftRotation_;
        previousRightRotation_ = currentRightRotation_;
    }
};


#endif //STUDY_EXPLORER_H
