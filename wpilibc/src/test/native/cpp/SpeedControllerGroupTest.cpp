// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/motorcontrol/SpeedControllerGroup.h"  // NOLINT(build/include_order)

#include <memory>
#include <vector>

#include "MockSpeedController.h"
#include "gtest/gtest.h"

using namespace frc;

enum SpeedControllerGroupTestType { TEST_ONE, TEST_TWO, TEST_THREE };

std::ostream& operator<<(std::ostream& os,
                         const SpeedControllerGroupTestType& type) {
  switch (type) {
    case TEST_ONE:
      os << "SpeedControllerGroup with one speed controller";
      break;
    case TEST_TWO:
      os << "SpeedControllerGroup with two speed controllers";
      break;
    case TEST_THREE:
      os << "SpeedControllerGroup with three speed controllers";
      break;
  }

  return os;
}

/**
 * A fixture used for SpeedControllerGroup testing.
 */
class SpeedControllerGroupTest
    : public testing::TestWithParam<SpeedControllerGroupTestType> {
 protected:
  std::vector<MockSpeedController> m_speedControllers;
  std::unique_ptr<SpeedControllerGroup> m_group;

  void SetUp() override {
    switch (GetParam()) {
      case TEST_ONE: {
        m_speedControllers.emplace_back();
        m_group = std::make_unique<SpeedControllerGroup>(m_speedControllers[0]);
        break;
      }

      case TEST_TWO: {
        m_speedControllers.emplace_back();
        m_speedControllers.emplace_back();
        m_group = std::make_unique<SpeedControllerGroup>(m_speedControllers[0],
                                                         m_speedControllers[1]);
        break;
      }

      case TEST_THREE: {
        m_speedControllers.emplace_back();
        m_speedControllers.emplace_back();
        m_speedControllers.emplace_back();
        m_group = std::make_unique<SpeedControllerGroup>(m_speedControllers[0],
                                                         m_speedControllers[1],
                                                         m_speedControllers[2]);
        break;
      }
    }
  }
};

TEST_P(SpeedControllerGroupTest, Set) {
  m_group->Set(1.0);

  for (auto& speedController : m_speedControllers) {
    EXPECT_FLOAT_EQ(speedController.Get(), 1.0);
  }
}

TEST_P(SpeedControllerGroupTest, GetInverted) {
  m_group->SetInverted(true);

  EXPECT_TRUE(m_group->GetInverted());
}

TEST_P(SpeedControllerGroupTest, SetInvertedDoesNotModifySpeedControllers) {
  for (auto& speedController : m_speedControllers) {
    speedController.SetInverted(false);
  }
  m_group->SetInverted(true);

  for (auto& speedController : m_speedControllers) {
    EXPECT_EQ(speedController.GetInverted(), false);
  }
}

TEST_P(SpeedControllerGroupTest, SetInvertedDoesInvert) {
  m_group->SetInverted(true);
  m_group->Set(1.0);

  for (auto& speedController : m_speedControllers) {
    EXPECT_FLOAT_EQ(speedController.Get(), -1.0);
  }
}

TEST_P(SpeedControllerGroupTest, Disable) {
  m_group->Set(1.0);
  m_group->Disable();

  for (auto& speedController : m_speedControllers) {
    EXPECT_FLOAT_EQ(speedController.Get(), 0.0);
  }
}

TEST_P(SpeedControllerGroupTest, StopMotor) {
  m_group->Set(1.0);
  m_group->StopMotor();

  for (auto& speedController : m_speedControllers) {
    EXPECT_FLOAT_EQ(speedController.Get(), 0.0);
  }
}

INSTANTIATE_TEST_SUITE_P(Test, SpeedControllerGroupTest,
                         testing::Values(TEST_ONE, TEST_TWO, TEST_THREE));
