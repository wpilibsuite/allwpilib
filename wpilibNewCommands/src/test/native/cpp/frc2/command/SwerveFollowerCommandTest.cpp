/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <frc/controller/PIDController.h>
#include <frc/controller/ProfiledPIDController.h>
#include <frc/geometry/Rotation2d.h>
#include <frc/geometry/Translation2d.h>
#include <frc/kinematics/SwerveDriveKinematics.h>
#include <frc/kinematics/SwerveDriveOdometry.h>
#include <frc/kinematics/SwerveModuleState.h>
#include <frc2/Timer.h>
#include <frc2/command/Subsystem.h>
#include <frc2/command/SwerveFollowerCommand.h>

#include <iostream>

#include <wpi/math>

#include "frc/trajectory/TrajectoryGenerator.h"
#include "gtest/gtest.h"

#define EXPECT_NEAR_UNITS(val1, val2, eps) \
  EXPECT_LE(units::math::abs(val1 - val2), eps)

class SwerveFollowerCommandTest : public ::testing::Test {
 protected:
  frc2::Timer m_rotTimer;
  frc::Rotation2d m_angle{0_rad};

  std::array<frc::SwerveModuleState, 4> m_moduleStates{
      frc::SwerveModuleState{}, frc::SwerveModuleState{},
      frc::SwerveModuleState{}, frc::SwerveModuleState{}};

  frc::ProfiledPIDController m_rotController{
      1, 0, 0, frc::TrapezoidProfile::Constraints{9_mps, 3_mps_sq}};

  static constexpr units::meter_t kxTolerance{1 / 12.0};
  static constexpr units::meter_t kyTolerance{1 / 12.0};
  static constexpr units::radian_t kAngularTolerance{1 / 12.0};

  static constexpr units::meter_t kTrackLength{0.5};
  static constexpr units::meter_t kTrackWidth{0.5};

  frc::SwerveDriveKinematics<4> m_kinematics{
      frc::Translation2d{kTrackLength / 2, kTrackWidth / 2},
      frc::Translation2d{kTrackLength / 2, -kTrackWidth / 2},
      frc::Translation2d{-kTrackLength / 2, kTrackWidth / 2},
      frc::Translation2d{-kTrackLength / 2, -kTrackWidth / 2}};

  frc::SwerveDriveOdometry<4> m_odometry{m_kinematics, 0_rad,
                                         frc::Pose2d{0_m, 0_m, 0_rad}};

  std::array<frc::SwerveModuleState, 4> getCurrentWheelSpeeds() {
    return m_moduleStates;
  }

  frc::Pose2d getRobotPose() {
    m_odometry.Update(m_angle, getCurrentWheelSpeeds());
    return m_odometry.GetPose();
  }
};

TEST_F(SwerveFollowerCommandTest, ReachesReference) {
  frc2::Subsystem subsystem{};

  auto waypoints =
      std::vector{frc::Pose2d{0_m, 0_m, 0_rad}, frc::Pose2d{1_m, 5_m, 3_rad}};
  auto trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      waypoints, {8.8_mps, 0.1_mps_sq});

  auto endState = trajectory.Sample(trajectory.TotalTime());

  auto command = frc2::SwerveFollowerCommand<4>(
      trajectory, [&]() { return getRobotPose(); }, m_kinematics,

      frc2::PIDController(0.6, 0, 0), frc2::PIDController(0.6, 0, 0),
      m_rotController,
      [&](auto moduleStates) { m_moduleStates = moduleStates; }, {&subsystem});

  m_rotTimer.Reset();
  m_rotTimer.Start();
  command.Initialize();
  while (!command.IsFinished()) {
    command.Execute();
    m_angle = trajectory.Sample(m_rotTimer.Get()).pose.Rotation();
  }
  m_rotTimer.Stop();
  command.End(false);

  EXPECT_NEAR_UNITS(endState.pose.Translation().X(),
                    getRobotPose().Translation().X(), kxTolerance);
  EXPECT_NEAR_UNITS(endState.pose.Translation().Y(),
                    getRobotPose().Translation().Y(), kyTolerance);
  EXPECT_NEAR_UNITS(endState.pose.Rotation().Radians(),
                    getRobotPose().Rotation().Radians(), kAngularTolerance);
}
