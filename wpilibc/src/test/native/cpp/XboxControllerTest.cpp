/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/XboxController.h"  // NOLINT(build/include_order)

#include "frc/simulation/XboxControllerSim.h"
#include "gtest/gtest.h"

using namespace frc;

TEST(XboxControllerTests, GetX) {
  XboxController joy{2};
  sim::XboxControllerSim joysim{joy};

  joysim.SetX(XboxController::kLeftHand, 0.35);
  joysim.SetX(XboxController::kRightHand, 0.45);
  joysim.NotifyNewData();
  ASSERT_NEAR(joy.GetX(XboxController::kLeftHand), 0.35, 0.001);
  ASSERT_NEAR(joy.GetX(XboxController::kRightHand), 0.45, 0.001);
}

TEST(XboxControllerTests, GetBumper) {
  XboxController joy{1};
  sim::XboxControllerSim joysim{joy};

  joysim.SetBumper(XboxController::kLeftHand, false);
  joysim.SetBumper(XboxController::kRightHand, true);
  joysim.NotifyNewData();
  ASSERT_FALSE(joy.GetBumper(XboxController::kLeftHand));
  ASSERT_TRUE(joy.GetBumper(XboxController::kRightHand));
  // need to call pressed and released to clear flags
  joy.GetBumperPressed(XboxController::kLeftHand);
  joy.GetBumperReleased(XboxController::kLeftHand);
  joy.GetBumperPressed(XboxController::kRightHand);
  joy.GetBumperReleased(XboxController::kRightHand);

  joysim.SetBumper(XboxController::kLeftHand, true);
  joysim.SetBumper(XboxController::kRightHand, false);
  joysim.NotifyNewData();
  ASSERT_TRUE(joy.GetBumper(XboxController::kLeftHand));
  ASSERT_TRUE(joy.GetBumperPressed(XboxController::kLeftHand));
  ASSERT_FALSE(joy.GetBumperReleased(XboxController::kLeftHand));
  ASSERT_FALSE(joy.GetBumper(XboxController::kRightHand));
  ASSERT_FALSE(joy.GetBumperPressed(XboxController::kRightHand));
  ASSERT_TRUE(joy.GetBumperReleased(XboxController::kRightHand));
}

TEST(XboxControllerTests, GetAButton) {
  XboxController joy{1};
  sim::XboxControllerSim joysim{joy};

  joysim.SetAButton(false);
  joysim.NotifyNewData();
  ASSERT_FALSE(joy.GetAButton());
  // need to call pressed and released to clear flags
  joy.GetAButtonPressed();
  joy.GetAButtonReleased();

  joysim.SetAButton(true);
  joysim.NotifyNewData();
  ASSERT_TRUE(joy.GetAButton());
  ASSERT_TRUE(joy.GetAButtonPressed());
  ASSERT_FALSE(joy.GetAButtonReleased());

  joysim.SetAButton(false);
  joysim.NotifyNewData();
  ASSERT_FALSE(joy.GetAButton());
  ASSERT_FALSE(joy.GetAButtonPressed());
  ASSERT_TRUE(joy.GetAButtonReleased());
}
