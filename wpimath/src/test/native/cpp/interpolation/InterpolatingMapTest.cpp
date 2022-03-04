// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/interpolation/InterpolatingMap.h"  // NOLINT(build/include_order)

#include <gtest/gtest.h>
#include <units/angular_velocity.h>
#include <units/length.h>

TEST(InterpolatingMap, Insert) {
    frc3512::InterpolatingMap<units::meter_t, units::radians_per_second_t> table;

    table.Insert(125_in, 450_rad_per_s);
    table.Insert(200_in, 510_rad_per_s);
    table.Insert(268_in, 525_rad_per_s);
    table.Insert(312_in, 550_rad_per_s);
    table.Insert(326_in, 650_rad_per_s);

    // Key below minimum gives smallest value
    EXPECT_EQ(450_rad_per_s, table[100_in]);

    // Minimum key gives exact value
    EXPECT_EQ(450_rad_per_s, table[125_in]);

    // Key gives interpolated value
    EXPECT_EQ(480_rad_per_s, table[162.5_in]);

    // Key at right of interpolation range gives exact value
    EXPECT_EQ(510_rad_per_s, table[200_in]);

    // Maximum key gives exact value
    EXPECT_EQ(650_rad_per_s, table[326_in]);

    // Key above maximum gives largest value
    EXPECT_EQ(650_rad_per_s, table[400_in]);
}

TEST(InterpolatingMap, Clear) {
    frc3512::InterpolatingMap<units::meter_t, units::radians_per_second_t> table;

    table.Insert(125_in, 450_rad_per_s);
    table.Insert(200_in, 510_rad_per_s);
    table.Insert(268_in, 525_rad_per_s);
    table.Insert(312_in, 550_rad_per_s);
    table.Insert(326_in, 650_rad_per_s);

    table.Clear();

    table.Insert(100_in, 250_rad_per_s);
    table.Insert(200_in, 500_rad_per_s);

    EXPECT_EQ(375_rad_per_s, table[150_in]);
}