#include "frc/controller/PIDController.h"
#include "frc/controller/PIDControllerRunner.h"
#include "gtest/gtest.h"

using namespace frc2;
using namespace frc;

TEST(PIDCopyMoveTest, ControllerCopyConstructible) {
  EXPECT_TRUE(std::is_copy_constructible_v<PIDController>);
}

TEST(PIDCopyMoveTest, ControllerMoveConstructible) {
  EXPECT_TRUE(std::is_move_constructible_v<PIDController>);
}

TEST(PIDCopyMoveTest, ControllerCopyAssignable) {
  EXPECT_TRUE(std::is_copy_assignable_v<PIDController>);
}

TEST(PIDCopyMoveTest, ControllerMoveAssignable) {
  EXPECT_TRUE(std::is_move_assignable_v<PIDController>);
}

TEST(PIDCopyMoveTest, RunnerMoveConstructible) {
  EXPECT_TRUE(std::is_move_constructible_v<PIDControllerRunner>);
}

TEST(PIDCopyMoveTest, RunnerMoveAssignable) {
  EXPECT_TRUE(std::is_move_assignable_v<PIDControllerRunner>);
}