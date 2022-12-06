// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/motorcontrol/MotorControllerGroup.h"  // NOLINT(build/include_order)

#include <memory>
#include <vector>

#include "gtest/gtest.h"
#include "motorcontrol/MockMotorController.h"

using namespace frc;

enum MotorControllerGroupTestType { TEST_ONE, TEST_TWO, TEST_THREE };

std::ostream& operator<<(std::ostream& os,
                         const MotorControllerGroupTestType& type) {
  switch (type) {
    case TEST_ONE:
      os << "MotorControllerGroup with one motor controller";
      break;
    case TEST_TWO:
      os << "MotorControllerGroup with two motor controllers";
      break;
    case TEST_THREE:
      os << "MotorControllerGroup with three motor controllers";
      break;
  }

  return os;
}

/**
 * A fixture used for MotorControllerGroup testing.
 */
class MotorControllerGroupTest
    : public testing::TestWithParam<MotorControllerGroupTestType> {
 protected:
  std::vector<MockMotorController> m_motorControllers;
  std::unique_ptr<MotorControllerGroup> m_group;

  void SetUp() override {
    switch (GetParam()) {
      case TEST_ONE: {
        m_motorControllers.emplace_back();
        m_group = std::make_unique<MotorControllerGroup>(m_motorControllers[0]);
        break;
      }

      case TEST_TWO: {
        m_motorControllers.emplace_back();
        m_motorControllers.emplace_back();
        m_group = std::make_unique<MotorControllerGroup>(m_motorControllers[0],
                                                         m_motorControllers[1]);
        break;
      }

      case TEST_THREE: {
        m_motorControllers.emplace_back();
        m_motorControllers.emplace_back();
        m_motorControllers.emplace_back();
        m_group = std::make_unique<MotorControllerGroup>(m_motorControllers[0],
                                                         m_motorControllers[1],
                                                         m_motorControllers[2]);
        break;
      }
    }
  }
};

TEST_P(MotorControllerGroupTest, Set) {
  m_group->Set(1.0);

  for (auto& motorController : m_motorControllers) {
    EXPECT_FLOAT_EQ(motorController.Get(), 1.0);
  }
}

TEST_P(MotorControllerGroupTest, GetInverted) {
  m_group->SetInverted(true);

  EXPECT_TRUE(m_group->GetInverted());
}

TEST_P(MotorControllerGroupTest, SetInvertedDoesNotModifyMotorControllers) {
  for (auto& motorController : m_motorControllers) {
    motorController.SetInverted(false);
  }
  m_group->SetInverted(true);

  for (auto& motorController : m_motorControllers) {
    EXPECT_EQ(motorController.GetInverted(), false);
  }
}

TEST_P(MotorControllerGroupTest, SetInvertedDoesInvert) {
  m_group->SetInverted(true);
  m_group->Set(1.0);

  for (auto& motorController : m_motorControllers) {
    EXPECT_FLOAT_EQ(motorController.Get(), -1.0);
  }
}

TEST_P(MotorControllerGroupTest, Disable) {
  m_group->Set(1.0);
  m_group->Disable();

  for (auto& motorController : m_motorControllers) {
    EXPECT_FLOAT_EQ(motorController.Get(), 0.0);
  }
}

TEST_P(MotorControllerGroupTest, StopMotor) {
  m_group->Set(1.0);
  m_group->StopMotor();

  for (auto& motorController : m_motorControllers) {
    EXPECT_FLOAT_EQ(motorController.Get(), 0.0);
  }
}

INSTANTIATE_TEST_SUITE_P(Tests, MotorControllerGroupTest,
                         testing::Values(TEST_ONE, TEST_TWO, TEST_THREE));
