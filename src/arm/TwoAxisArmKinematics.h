#ifndef TWO_AXIS_ARM_KINEMATICS_H
#define TWO_AXIS_ARM_KINEMATICS_H
#include <Arduino.h>
// The inverse kinematics formulas in this file are based on work by Benjamin, @rowanberry, @carlosdperezruiz, and @joshua-8
// https://github.com/page-turner/page-turner and before that https://github.com/P-B-and-J/arm-testing

/**
 * @brief  reverse kinematics formula for a 2D arm
 * @note  angles are in degrees
 * @param  x: (float) x coordinate
 * @param  y: (float) y coordinate
 * @param  theta1: (float&) angle for first servo
 * @param  theta2: (float&) angle for second servo
 * @param  d1: (float) length of first arm segment
 * @param  d2: (float) length of second arm segment
 * @param  theta1Min: (float) low range of servo1
 * @param  theta1Max: (float) high range of servo1
 * @param  theta2Min: (float) low range of servo2
 * @param  theta2Max: (float) high range of servo2
 * @retval (bool) true if coordinates possible to reach and angles updated, false if angles not updated
 */
bool cartToAngles(float x, float y, float& theta1, float& theta2, float d1, float d2, float theta1Min, float theta1Max, float theta2Min, float theta2Max)
{
    float potential_theta2 = PI - acos((-sq(x) - sq(y) + sq(d1) + sq(d2)) / (2 * d1 * d2));
    if (x > 0) {
        potential_theta2 = -potential_theta2;
    }
    float potential_theta1 = asin((d2 * -sin(potential_theta2)) / sqrt(sq(x) + sq(y))) + atan(y / x) - PI / 2;
    if (x < 0) {
        potential_theta1 = potential_theta1 + PI;
    }
    potential_theta1 = degrees(potential_theta1);
    potential_theta2 = degrees(potential_theta2);
    if (isnan(potential_theta1) || isnan(potential_theta2) || potential_theta1 < theta1Min || potential_theta1 > theta1Max || potential_theta2 < theta2Min || potential_theta2 > theta2Max) {
        return false;
    }
    theta1 = potential_theta1;
    theta2 = potential_theta2;
    return true;
}

#endif // TWO_AXIS_ARM_KINEMATICS_H
