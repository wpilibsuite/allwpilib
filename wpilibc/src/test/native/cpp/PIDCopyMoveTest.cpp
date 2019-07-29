#include "frc/controller/PIDController.h"
#include "gtest/gtest.h"

using namespace frc2;

TEST(PIDCopyMoveTest, CopyConstructible) {
  EXPECT_TRUE(std::is_copy_constructible_v<PIDController>);
}

TEST(PIDCopyMoveTest, MoveConstructible) {
  EXPECT_TRUE(std::is_move_constructible_v<PIDController>);
}

TEST(PIDCopyMoveTest, CopyAssignable) {
  EXPECT_TRUE(std::is_copy_assignable_v<PIDController>);
}

TEST(PIDCopyMoveTest, MoveAssignable) {
  EXPECT_TRUE(std::is_move_assignable_v<PIDController>);
}
