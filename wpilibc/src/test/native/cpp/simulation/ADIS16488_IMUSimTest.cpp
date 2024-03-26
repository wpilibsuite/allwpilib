// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <hal/HAL.h>

#include "frc/simulation/ADIS16448_IMUSim.h"
#include "frc/ADIS16448_IMU.h"

namespace frc::sim {
    TEST(ADIS16448_IMUSimTest, SetAttributes) {
        HAL_Initialize(500, 0);

        ADIS16448_IMU gyro;
        ADIS16448_IMUSim sim{gyro};

        EXPECT_EQ(0, gyro.GetAngle().value());
        EXPECT_EQ(0, gyro.GetRate().value());

        constexpr units::degree_t TEST_ANGLE{123.456};
        constexpr units::degrees_per_second_t TEST_RATE{229.3504};
        sim.SetGyroAngleZ(TEST_ANGLE);
        sim.SetGyroRateZ(TEST_RATE);
        EXPECT_EQ(TEST_ANGLE, gyro.GetAngle());
        EXPECT_EQ(TEST_RATE, gyro.GetGyroRateZ());
    }
}  // namespace frc::sim