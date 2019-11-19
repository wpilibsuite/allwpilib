/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
#include <frc2/command/Subsystem.h>
#include <frc2/command/MecanumFollowerCommand.h>
#include <frc/controller/PIDController.h>
#include <frc/controller/ProfiledPIDController.h>
#include <frc/kinematics/MecanumDriveKinematics.h>
#include <frc/kinematics/MecanumDriveOdometry.h>
#include <frc/geometry/Translation2d.h>
#include <frc/geometry/Rotation2d.h>
#include <frc/geometry/Rotation2d.h>
#include <frc2/Timer.h>
#include <wpi/math>
#include "frc/trajectory/TrajectoryGenerator.h"
#include "gtest/gtest.h"
#include <iostream>

#define EXPECT_NEAR_UNITS(val1, val2, eps) \
  EXPECT_LE(units::math::abs(val1 - val2), eps)

static constexpr units::meter_t kTolerance{1 / 12.0};

frc2::Timer m_rotTimer;
frc::Rotation2d m_angle{0_rad};

units::meters_per_second_t  m_frontLeftSpeed;
units::meters_per_second_t  m_rearLeftSpeed;
units::meters_per_second_t  m_frontRightSpeed;
units::meters_per_second_t  m_rearRightSpeed;

frc::ProfiledPIDController m_rotController{1, 0, 0,
    frc::TrapezoidProfile::Constraints{9_mps, 3_mps_sq}};

static constexpr units::meter_t kxTolerance{1 / 12.0};
static constexpr units::meter_t kyTolerance{1 / 12.0};


static constexpr units::meter_t m_trackLength{0.5};
static constexpr units::meter_t m_trackWidth{0.5};

frc::MecanumDriveKinematics m_kinematics{
    frc::Translation2d{m_trackLength / 2, m_trackWidth / 2},
    frc::Translation2d{m_trackLength / 2, -m_trackWidth / 2},
    frc::Translation2d{-m_trackLength / 2, m_trackWidth / 2},
    frc::Translation2d{-m_trackLength / 2, -m_trackWidth / 2}
};

frc::MecanumDriveOdometry m_odometry{
    m_kinematics,
    frc::Rotation2d{0_rad},
    frc::Pose2d{0_m, 0_m, frc::Rotation2d{0_rad}}
};

frc::MecanumDriveWheelSpeeds getCurrentWheelSpeeds() {
  return frc::MecanumDriveWheelSpeeds{m_frontLeftSpeed,
      m_frontRightSpeed, m_rearLeftSpeed, m_rearRightSpeed};
}

frc::Pose2d getRobotPose() {
  m_odometry.Update(m_angle, getCurrentWheelSpeeds());
  return m_odometry.GetPose();
}

TEST(MecanumFollowerCommandTest, ReachesReference) {

  frc2::Subsystem subsystem{};

  auto waypoints = std::vector{frc::Pose2d{0_m, 0_m, 0_rad},
                               frc::Pose2d{1_m, 5_m, 3_rad}};
  auto trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      waypoints, {8.8_mps, 0.1_mps_sq});

  auto endState = trajectory.Sample(trajectory.TotalTime());

  auto command = frc2::MecanumFollowerCommand(trajectory,
      [&]() { return getRobotPose(); },
      m_kinematics,

      frc2::PIDController(0.6, 0, 0),
      frc2::PIDController(0.6, 0, 0),
      m_rotController,
      units::meters_per_second_t(8.8),
      [&](units::meters_per_second_t frontLeft, units::meters_per_second_t rearLeft,
             units::meters_per_second_t frontRight,
             units::meters_per_second_t rearRight) {
            m_frontLeftSpeed = frontLeft;
            m_rearLeftSpeed = rearLeft;
            m_frontRightSpeed = frontRight;
            m_rearRightSpeed = rearRight;
      },
      {&subsystem}
  );

  m_rotTimer.Reset();
  m_rotTimer.Start();
  command.Initialize();
  while ( !command.IsFinished() ) {
    command.Execute();
    m_angle = trajectory.Sample(m_rotTimer.Get()).pose.Rotation();
  }
  m_rotTimer.Stop();
  command.End(false);


  EXPECT_NEAR_UNITS(endState.pose.Translation().X(), getRobotPose().Translation().X(),
                    kxTolerance);

}