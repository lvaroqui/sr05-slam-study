//
// Created by luc on 17/05/19.
//

#include "Explorer.h"

void Explorer::controlTranslation() {
    currentTranslation_ += (currentLeftRotation_ - previousLeftRotation_) * wheelDiameter_;
    double error = targetTranslation_ - currentTranslation_;

    double command = 15 * error;

    command = saturate(command, 5);

    rightWheelMotor_->setVelocity(command);
    leftWheelMotor_->setVelocity(command);
}

void Explorer::controlRotation() {
    double error = targetHeading_ - heading_;
    error = modAngle(error);;

    double command = 15 * error;

    command = saturate(command, 2);

    rightWheelMotor_->setVelocity(-command);
    leftWheelMotor_->setVelocity(command);
}
