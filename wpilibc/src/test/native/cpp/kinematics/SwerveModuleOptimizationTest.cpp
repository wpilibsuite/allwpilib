#include "frc/kinematics/SwerveModuleState.h"
#include "gtest/gtest.h"

using namespace frc;

static constexpr double kEpsilon = 0.01;

class SwerveModuleOptimizationTest : public ::testing::Test {
 protected:
  SwerveModuleState m_state{1_mps,Rotation2d()};
};

TEST_F(SwerveModuleOptimizationTest, NoOptimization) {

  SwerveModuleState newState{1_mps,Rotation2d()};

  SwerveModuleState finalState = m_state.OptimizeModuleAngle(newState);

  EXPECT_NEAR(0.0, finalState.angle.Degrees().to<double>(), kEpsilon);
  EXPECT_NEAR(1.0, finalState.speed.to<double>(), kEpsilon);
}

TEST_F(SwerveModuleOptimizationTest, Turn180) {
  SwerveModuleState newState{1_mps,Rotation2d(180_deg)};

  SwerveModuleState finalState = m_state.OptimizeModuleAngle(newState);

  EXPECT_NEAR(0.0, finalState.angle.Degrees().to<double>(), kEpsilon);
  EXPECT_NEAR(-1.0, finalState.speed.to<double>(), kEpsilon);
}

TEST_F(SwerveModuleOptimizationTest, Turn45) {
  SwerveModuleState newState{1_mps,Rotation2d(45_deg)};

  SwerveModuleState finalState = m_state.OptimizeModuleAngle(newState);

  EXPECT_NEAR(45.0, finalState.angle.Degrees().to<double>(), kEpsilon);
  EXPECT_NEAR(1.0, finalState.speed.to<double>(), kEpsilon);
}

TEST_F(SwerveModuleOptimizationTest, Turn135) {
  SwerveModuleState newState{1_mps,Rotation2d(135_deg)};

  SwerveModuleState finalState = m_state.OptimizeModuleAngle(newState);

  EXPECT_NEAR(45.0, finalState.angle.Degrees().to<double>(), kEpsilon);
  EXPECT_NEAR(1.0, finalState.speed.to<double>(), kEpsilon);
}