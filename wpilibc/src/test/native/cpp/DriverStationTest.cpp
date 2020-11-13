/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <tuple>

#include "frc/DriverStation.h"
#include "frc/simulation/DriverStationSim.h"
#include "gtest/gtest.h"

class IsJoystickConnectedParametersTests
    : public ::testing::TestWithParam<std::tuple<int, int, int, bool>> {};

TEST_P(IsJoystickConnectedParametersTests, IsJoystickConnected) {
  frc::sim::DriverStationSim::SetJoystickAxisCount(1, std::get<0>(GetParam()));
  frc::sim::DriverStationSim::SetJoystickButtonCount(1,
                                                     std::get<1>(GetParam()));
  frc::sim::DriverStationSim::SetJoystickPOVCount(1, std::get<2>(GetParam()));
  frc::sim::DriverStationSim::NotifyNewData();

  ASSERT_EQ(std::get<3>(GetParam()),
            frc::DriverStation::GetInstance().IsJoystickConnected(1));
}

INSTANTIATE_TEST_SUITE_P(IsConnectedTests, IsJoystickConnectedParametersTests,
                         ::testing::Values(std::make_tuple(0, 0, 0, false),
                                           std::make_tuple(1, 0, 0, true),
                                           std::make_tuple(0, 1, 0, true),
                                           std::make_tuple(0, 0, 1, true),
                                           std::make_tuple(1, 1, 1, true),
                                           std::make_tuple(4, 10, 1, true)));
