// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/interfaces/Gyro.h>

#include <units/angle.h>

#include "frc/geometry/Rotation3d.h"

namespace frc {

/** Gyro that supports representation in the 3D space. */
class Gyro3D: virtual public Gyro {
    public:
        /**
         * Return the Skew angle from the Gyroscope.
         *
         * <p>The angle is continuous, that is it will continue from 360 to 361
         * degrees. This allows algorithms that wouldn't want to see a discontinuity
         * in the gyro output as it sweeps past from 360 to 0 on the second time
         * around.
         *
         * <p>The angle is expected to increase as the gyro turns clockwise. It needs
         * to follow the NED axis convention.
         *
         * @return skew angle.
         */
        virtual double GetRoll() const = 0;

        /**
         * Return the Pitch angle from the Gyroscope.
         *
         * <p>The angle is continuous, that is it will continue from 360 to 361
         * degrees. This allows algorithms that wouldn't want to see a discontinuity
         * in the gyro output as it sweeps past from 360 to 0 on the second time
         * around.
         *
         * <p>The angle is expected to increase as the gyro turns clockwise. It needs
         * to follow the NED axis convention.
         *
         * @return pitch angle.
         */
        virtual double GetPitch() const = 0;

        /**
         * Return the Yaw angle from the Gyroscope.
         *
         * <p>The angle is continuous, that is it will continue from 360 to 361
         * degrees. This allows algorithms that wouldn't want to see a discontinuity
         * in the gyro output as it sweeps past from 360 to 0 on the second time
         * around.
         *
         * <p>The angle is expected to increase as the gyro turns clockwise. It needs
         * to follow the NED axis convention.
         *
         * @return yaw angle.
         */
        virtual double GetYaw() const = 0;

        /**
         * Return the heading of the robot as a Rotation3d.
         *
         * @return the current heading of the robot as a Rotation3d.
         */
        virtual Rotation3d GetRotation3d() const {
            return units::degree_t{GetRoll(), GetPitch(), GetYaw()};
        }
};

}  // namespace frc
