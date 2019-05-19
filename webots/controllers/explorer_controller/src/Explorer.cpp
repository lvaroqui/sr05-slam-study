//
// Created by luc on 17/05/19.
//

#include "Explorer.h"

void Explorer::controlTranslation() {
    currentTranslation_ += (currentLeftRotation_ - previousLeftRotation_) * wheelDiameter_;
    double error = targetTranslation_ - currentTranslation_;

    double command = 15 * error;
    double sign = command / fabs(command);

    if (fabs(command) > 5) {
        command = 5 * sign;
    }

    rightWheelMotor_->setVelocity(command);
    leftWheelMotor_->setVelocity(command);
}

void Explorer::controlRotation() {
    currentRotation_ += atan2((currentLeftRotation_ - previousLeftRotation_) * wheelDiameter_, wheelEccentricity_);
    double error = targetRotation_ - currentRotation_;

    double command = 15 * error;
    double sign = command / fabs(command);

    if (fabs(command) > 2) {
        command = 2 * sign;
    }

    rightWheelMotor_->setVelocity(-command);
    leftWheelMotor_->setVelocity(command);
}
