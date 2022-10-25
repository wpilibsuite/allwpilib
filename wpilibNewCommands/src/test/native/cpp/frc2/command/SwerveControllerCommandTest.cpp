// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc2/command/Subsystem.h>
#include <frc2/command/SwerveControllerCommand.h>

#include <numbers>

#include <frc/Timer.h>
#include <frc/controller/PIDController.h>
#include <frc/controller/ProfiledPIDController.h>
#include <frc/geometry/Rotation2d.h>
#include <frc/geometry/Translation2d.h>
#include <frc/kinematics/SwerveDriveKinematics.h>
#include <frc/kinematics/SwerveDriveOdometry.h>
#include <frc/kinematics/SwerveModuleState.h>
#include <frc/simulation/SimHooks.h>
#include <frc/trajectory/TrajectoryGenerator.h>

#include "gtest/gtest.h"

#define EXPECT_NEAR_UNITS(val1, val2, eps) \
  EXPECT_LE(units::math::abs(val1 - val2), eps)

class SwerveControllerCommandTest : public ::testing::Test {
  using radians_per_second_squared_t =
      units::compound_unit<units::radians,
                           units::inverse<units::squared<units::second>>>;

 protected:
  frc::Timer m_timer;
  frc::Rotation2d m_angle{0_rad};

  wpi::array<frc::SwerveModuleState, 4> m_moduleStates{
      frc::SwerveModuleState{}, frc::SwerveModuleState{},
      frc::SwerveModuleState{}, frc::SwerveModuleState{}};

  wpi::array<frc::SwerveModulePosition, 4> m_modulePositions{
      frc::SwerveModulePosition{}, frc::SwerveModulePosition{},
      frc::SwerveModulePosition{}, frc::SwerveModulePosition{}};

  frc::ProfiledPIDController<units::radians> m_rotController{
      1, 0, 0,
      frc::TrapezoidProfile<units::radians>::Constraints{
          9_rad_per_s, units::unit_t<radians_per_second_squared_t>(3)}};

  static constexpr units::meter_t kxTolerance{1 / 12.0};
  static constexpr units::meter_t kyTolerance{1 / 12.0};
  static constexpr units::radian_t kAngularTolerance{1 / 12.0};

  static constexpr units::meter_t kWheelBase{0.5};
  static constexpr units::meter_t kTrackWidth{0.5};

  frc::SwerveDriveKinematics<4> m_kinematics{
      frc::Translation2d{kWheelBase / 2, kTrackWidth / 2},
      frc::Translation2d{kWheelBase / 2, -kTrackWidth / 2},
      frc::Translation2d{-kWheelBase / 2, kTrackWidth / 2},
      frc::Translation2d{-kWheelBase / 2, -kTrackWidth / 2}};

  frc::SwerveDriveOdometry<4> m_odometry{m_kinematics, 0_rad, m_modulePositions,
                                         frc::Pose2d{0_m, 0_m, 0_rad}};

  void SetUp() override { frc::sim::PauseTiming(); }

  void TearDown() override { frc::sim::ResumeTiming(); }

  frc::Pose2d getRobotPose() {
    m_odometry.Update(m_angle, m_modulePositions);
    return m_odometry.GetPose();
  }
};

TEST_F(SwerveControllerCommandTest, ReachesReference) {
  frc2::Subsystem subsystem;

  auto waypoints =
      std::vector{frc::Pose2d{0_m, 0_m, 0_rad}, frc::Pose2d{1_m, 5_m, 3_rad}};
  auto trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      waypoints, {8.8_mps, 0.1_mps_sq});

  auto endState = trajectory.Sample(trajectory.TotalTime());

  auto command = frc2::SwerveControllerCommand<4>(
      trajectory, [&]() { return getRobotPose(); }, m_kinematics,

      frc2::PIDController(0.6, 0, 0), frc2::PIDController(0.6, 0, 0),
      m_rotController,
      [&](auto moduleStates) { m_moduleStates = moduleStates; }, {&subsystem});

  m_timer.Reset();
  m_timer.Start();

  command.Initialize();
  while (!command.IsFinished()) {
    command.Execute();
    m_angle = trajectory.Sample(m_timer.Get()).pose.Rotation();

    for (size_t i = 0; i < m_modulePositions.size(); i++) {
      m_modulePositions[i].distance += m_moduleStates[i].speed * 5_ms;
      m_modulePositions[i].angle = m_moduleStates[i].angle;
    }

    frc::sim::StepTiming(5_ms);
  }
  m_timer.Stop();
  command.End(false);

  EXPECT_NEAR_UNITS(endState.pose.X(), getRobotPose().X(), kxTolerance);
  EXPECT_NEAR_UNITS(endState.pose.Y(), getRobotPose().Y(), kyTolerance);
  EXPECT_NEAR_UNITS(endState.pose.Rotation().Radians(),
                    getRobotPose().Rotation().Radians(), kAngularTolerance);
}
