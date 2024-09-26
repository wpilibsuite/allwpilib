// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/MecanumControllerCommand.h"

#include <memory>
#include <utility>

#include <units/velocity.h>
#include <units/voltage.h>

using namespace frc2;
using kv_unit = units::compound_unit<units::volts,
                                     units::inverse<units::meters_per_second>>;

MecanumControllerCommand::MecanumControllerCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    frc::SimpleMotorFeedforward<units::meters> feedforward,
    frc::MecanumDriveKinematics kinematics, frc::PIDController xController,
    frc::PIDController yController,
    frc::ProfiledPIDController<units::radians> thetaController,
    std::function<frc::Rotation2d()> desiredRotation,
    units::meters_per_second_t maxWheelVelocity,
    std::function<frc::MecanumDriveWheelSpeeds()> currentWheelSpeeds,
    frc::PIDController frontLeftController,
    frc::PIDController rearLeftController,
    frc::PIDController frontRightController,
    frc::PIDController rearRightController,
    std::function<void(units::volt_t, units::volt_t, units::volt_t,
                       units::volt_t)>
        output,
    Requirements requirements)
    : m_trajectory(std::move(trajectory)),
      m_pose(std::move(pose)),
      m_feedforward(feedforward),
      m_kinematics(kinematics),
      m_controller(xController, yController, thetaController),
      m_desiredRotation(std::move(desiredRotation)),
      m_maxWheelVelocity(maxWheelVelocity),
      m_frontLeftController(
          std::make_unique<frc::PIDController>(frontLeftController)),
      m_rearLeftController(
          std::make_unique<frc::PIDController>(rearLeftController)),
      m_frontRightController(
          std::make_unique<frc::PIDController>(frontRightController)),
      m_rearRightController(
          std::make_unique<frc::PIDController>(rearRightController)),
      m_currentWheelSpeeds(std::move(currentWheelSpeeds)),
      m_outputVolts(std::move(output)),
      m_usePID(true) {
  AddRequirements(requirements);
}

MecanumControllerCommand::MecanumControllerCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    frc::SimpleMotorFeedforward<units::meters> feedforward,
    frc::MecanumDriveKinematics kinematics, frc::PIDController xController,
    frc::PIDController yController,
    frc::ProfiledPIDController<units::radians> thetaController,
    units::meters_per_second_t maxWheelVelocity,
    std::function<frc::MecanumDriveWheelSpeeds()> currentWheelSpeeds,
    frc::PIDController frontLeftController,
    frc::PIDController rearLeftController,
    frc::PIDController frontRightController,
    frc::PIDController rearRightController,
    std::function<void(units::volt_t, units::volt_t, units::volt_t,
                       units::volt_t)>
        output,
    Requirements requirements)
    : m_trajectory(std::move(trajectory)),
      m_pose(std::move(pose)),
      m_feedforward(feedforward),
      m_kinematics(kinematics),
      m_controller(xController, yController, thetaController),
      m_maxWheelVelocity(maxWheelVelocity),
      m_frontLeftController(
          std::make_unique<frc::PIDController>(frontLeftController)),
      m_rearLeftController(
          std::make_unique<frc::PIDController>(rearLeftController)),
      m_frontRightController(
          std::make_unique<frc::PIDController>(frontRightController)),
      m_rearRightController(
          std::make_unique<frc::PIDController>(rearRightController)),
      m_currentWheelSpeeds(std::move(currentWheelSpeeds)),
      m_outputVolts(std::move(output)),
      m_usePID(true) {
  AddRequirements(requirements);
}

MecanumControllerCommand::MecanumControllerCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    frc::MecanumDriveKinematics kinematics, frc::PIDController xController,
    frc::PIDController yController,
    frc::ProfiledPIDController<units::radians> thetaController,
    std::function<frc::Rotation2d()> desiredRotation,
    units::meters_per_second_t maxWheelVelocity,
    std::function<void(units::meters_per_second_t, units::meters_per_second_t,
                       units::meters_per_second_t, units::meters_per_second_t)>
        output,
    Requirements requirements)
    : m_trajectory(std::move(trajectory)),
      m_pose(std::move(pose)),
      m_feedforward(0_V, units::unit_t<kv_unit>{0}),
      m_kinematics(kinematics),
      m_controller(xController, yController, thetaController),
      m_desiredRotation(std::move(desiredRotation)),
      m_maxWheelVelocity(maxWheelVelocity),
      m_outputVel(std::move(output)),
      m_usePID(false) {
  AddRequirements(requirements);
}

MecanumControllerCommand::MecanumControllerCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    frc::MecanumDriveKinematics kinematics, frc::PIDController xController,
    frc::PIDController yController,
    frc::ProfiledPIDController<units::radians> thetaController,
    units::meters_per_second_t maxWheelVelocity,
    std::function<void(units::meters_per_second_t, units::meters_per_second_t,
                       units::meters_per_second_t, units::meters_per_second_t)>
        output,
    Requirements requirements)
    : m_trajectory(std::move(trajectory)),
      m_pose(std::move(pose)),
      m_feedforward(0_V, units::unit_t<kv_unit>{0}),
      m_kinematics(kinematics),
      m_controller(xController, yController, thetaController),
      m_maxWheelVelocity(maxWheelVelocity),
      m_outputVel(std::move(output)),
      m_usePID(false) {
  AddRequirements(requirements);
}

void MecanumControllerCommand::Initialize() {
  if (m_desiredRotation == nullptr) {
    m_desiredRotation = [&] {
      return m_trajectory.States().back().pose.Rotation();
    };
  }
  m_prevTime = 0_s;
  auto initialState = m_trajectory.Sample(0_s);

  auto initialXVelocity =
      initialState.velocity * initialState.pose.Rotation().Cos();
  auto initialYVelocity =
      initialState.velocity * initialState.pose.Rotation().Sin();

  m_prevSpeeds = m_kinematics.ToWheelSpeeds(
      frc::ChassisSpeeds{initialXVelocity, initialYVelocity, 0_rad_per_s});

  m_timer.Restart();
  if (m_usePID) {
    m_frontLeftController->Reset();
    m_rearLeftController->Reset();
    m_frontRightController->Reset();
    m_rearRightController->Reset();
  }
}

void MecanumControllerCommand::Execute() {
  auto curTime = m_timer.Get();

  auto m_desiredState = m_trajectory.Sample(curTime);

  auto targetChassisSpeeds =
      m_controller.Calculate(m_pose(), m_desiredState, m_desiredRotation());
  auto targetWheelSpeeds = m_kinematics.ToWheelSpeeds(targetChassisSpeeds);

  targetWheelSpeeds.Desaturate(m_maxWheelVelocity);

  auto frontLeftSpeedSetpoint = targetWheelSpeeds.frontLeft;
  auto rearLeftSpeedSetpoint = targetWheelSpeeds.rearLeft;
  auto frontRightSpeedSetpoint = targetWheelSpeeds.frontRight;
  auto rearRightSpeedSetpoint = targetWheelSpeeds.rearRight;

  if (m_usePID) {
    auto frontLeftFeedforward =
        m_feedforward.Calculate(m_prevSpeeds.frontLeft, frontLeftSpeedSetpoint);

    auto rearLeftFeedforward =
        m_feedforward.Calculate(m_prevSpeeds.rearLeft, rearLeftSpeedSetpoint);

    auto frontRightFeedforward = m_feedforward.Calculate(
        m_prevSpeeds.frontRight, frontRightSpeedSetpoint);

    auto rearRightFeedforward =
        m_feedforward.Calculate(m_prevSpeeds.rearRight, rearRightSpeedSetpoint);

    auto frontLeftOutput = units::volt_t{m_frontLeftController->Calculate(
                               m_currentWheelSpeeds().frontLeft.value(),
                               frontLeftSpeedSetpoint.value())} +
                           frontLeftFeedforward;
    auto rearLeftOutput = units::volt_t{m_rearLeftController->Calculate(
                              m_currentWheelSpeeds().rearLeft.value(),
                              rearLeftSpeedSetpoint.value())} +
                          rearLeftFeedforward;
    auto frontRightOutput = units::volt_t{m_frontRightController->Calculate(
                                m_currentWheelSpeeds().frontRight.value(),
                                frontRightSpeedSetpoint.value())} +
                            frontRightFeedforward;
    auto rearRightOutput = units::volt_t{m_rearRightController->Calculate(
                               m_currentWheelSpeeds().rearRight.value(),
                               rearRightSpeedSetpoint.value())} +
                           rearRightFeedforward;

    m_outputVolts(frontLeftOutput, rearLeftOutput, frontRightOutput,
                  rearRightOutput);
  } else {
    m_outputVel(frontLeftSpeedSetpoint, rearLeftSpeedSetpoint,
                frontRightSpeedSetpoint, rearRightSpeedSetpoint);

    m_prevTime = curTime;
    m_prevSpeeds = targetWheelSpeeds;
  }
}

void MecanumControllerCommand::End(bool interrupted) {
  m_timer.Stop();
}

bool MecanumControllerCommand::IsFinished() {
  return m_timer.HasElapsed(m_trajectory.TotalTime());
}
