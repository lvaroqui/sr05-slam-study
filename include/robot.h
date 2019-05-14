//
// Created by luc on 14/05/19.
//

#ifndef ETUDE_ROBOT_H
#define ETUDE_ROBOT_H

#include <iostream>
#include <cmath>

class Robot {
    double x_;
    double y_;
    double direction_;

public:
    /// Constructor of Robot class
    /// \param X Initial X position
    /// \param Y Initial Y position
    /// \param Direction Initial Direction
    Robot(double X, double Y, double Direction): x_(X), y_(Y), direction_(Direction) {}

    /// Move the robot forward
    /// \param distance Distance to travel
    void moveForward(double distance);

    /// Rotate the robot
    /// \param angle Angle to rotate (gradient)
    void rotate(double angle);

    /// Display information about the robot to output stream
    /// \param os Output Stream
    /// \param robot The robot
    /// \return New Output Stream
    friend std::ostream& operator<<(std::ostream& os, const Robot& robot);
};

#endif //ETUDE_ROBOT_H
