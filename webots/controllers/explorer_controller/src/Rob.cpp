//
// Created by luc on 17/05/19.
//

#include "Rob.h"

void Rob::controlTranslation() {
    currentTranslation_ += (currentLeftRotation_ - previousLeftRotation_) * wheelDiameter_;
    double error = targetTranslation_ - currentTranslation_;

    double command = 25 * error;
    if (careful_) {
        command = saturate(command, 2);
    }
    else {
        command = saturate(command, 5);
    }


    rightWheelMotor_->setVelocity(command);
    leftWheelMotor_->setVelocity(command);
}

void Rob::controlRotation() {
    double error = targetHeading_ - heading_;
    error = modAngle(error);;

    double command = 15 * error;

    command = saturate(command, 2);

    rightWheelMotor_->setVelocity(-command);
    leftWheelMotor_->setVelocity(command);
}

void Rob::handleMessages() {
    while (mailBox_.size() > 0) {
        RobOrd robord(mailBox_.pop().getValue("robord"));
        switch (robord.getType()) {
            case RobOrd::Type::move :
                translate(static_cast<double>(robord.getCommand()[0]) / 100.0);
                break;
            case RobOrd::Type::turn :
                rotate(static_cast<double>(robord.getCommand()[0]) * M_PI / 180.0);
                break;
            case RobOrd::tune:
                break;
            case RobOrd::lacc:
                break;
            case RobOrd::init:
                x_ = static_cast<double>(robord.getCommand()[0]) / 100.0;
                y_ = static_cast<double>(robord.getCommand()[1]) / 100.0;
                heading_ = static_cast<double>(robord.getCommand()[2]) * M_PI / 180.0;
                break;
            case RobOrd::curr:
                netMailBox_->push(RobAck::currMsg(static_cast<int>(x_ * 100), static_cast<int>(y_ * 100),
                                                  static_cast<int>(heading_ * 180.0 / M_PI)));
                break;
            case RobOrd::join:
                join(static_cast<double>(robord.getCommand()[0]) / 100.0,
                     static_cast<double>(robord.getCommand()[1]) / 100.0);
                break;
            case RobOrd::undefined:
                break;
        }
    }
}
