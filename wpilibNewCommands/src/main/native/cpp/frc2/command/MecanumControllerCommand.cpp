/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/MecanumControllerCommand.h"

using namespace frc2;
using namespace units;

MecanumControllerCommand::MecanumControllerCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    frc::SimpleMotorFeedforward<units::meters> feedforward,
    frc::MecanumDriveKinematics kinematics, frc2::PIDController xController,
    frc2::PIDController yController,
    frc::ProfiledPIDController<units::radians> thetaController,
    std::function<frc::Rotation2d()> desiredRotation,
    units::meters_per_second_t maxWheelVelocity,
    std::function<frc::MecanumDriveWheelSpeeds()> currentWheelSpeeds,
    frc2::PIDController frontLeftController,
    frc2::PIDController rearLeftController,
    frc2::PIDController frontRightController,
    frc2::PIDController rearRightController,
    std::function<void(units::volt_t, units::volt_t, units::volt_t,
                       units::volt_t)>
        output,
    std::initializer_list<Subsystem*> requirements)
    : m_trajectory(trajectory),
      m_pose(pose),
      m_feedforward(feedforward),
      m_kinematics(kinematics),
      m_controller(xController, yController, thetaController),
      m_desiredRotation(desiredRotation),
      m_maxWheelVelocity(maxWheelVelocity),
      m_frontLeftController(
          std::make_unique<frc2::PIDController>(frontLeftController)),
      m_rearLeftController(
          std::make_unique<frc2::PIDController>(rearLeftController)),
      m_frontRightController(
          std::make_unique<frc2::PIDController>(frontRightController)),
      m_rearRightController(
          std::make_unique<frc2::PIDController>(rearRightController)),
      m_currentWheelSpeeds(currentWheelSpeeds),
      m_outputVolts(output),
      m_usePID(true) {
  AddRequirements(requirements);
}

MecanumControllerCommand::MecanumControllerCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    frc::SimpleMotorFeedforward<units::meters> feedforward,
    frc::MecanumDriveKinematics kinematics, frc2::PIDController xController,
    frc2::PIDController yController,
    frc::ProfiledPIDController<units::radians> thetaController,
    units::meters_per_second_t maxWheelVelocity,
    std::function<frc::MecanumDriveWheelSpeeds()> currentWheelSpeeds,
    frc2::PIDController frontLeftController,
    frc2::PIDController rearLeftController,
    frc2::PIDController frontRightController,
    frc2::PIDController rearRightController,
    std::function<void(units::volt_t, units::volt_t, units::volt_t,
                       units::volt_t)>
        output,
    std::initializer_list<Subsystem*> requirements)
    : m_trajectory(trajectory),
      m_pose(pose),
      m_feedforward(feedforward),
      m_kinematics(kinematics),
      m_controller(xController, yController, thetaController),
      m_maxWheelVelocity(maxWheelVelocity),
      m_frontLeftController(
          std::make_unique<frc2::PIDController>(frontLeftController)),
      m_rearLeftController(
          std::make_unique<frc2::PIDController>(rearLeftController)),
      m_frontRightController(
          std::make_unique<frc2::PIDController>(frontRightController)),
      m_rearRightController(
          std::make_unique<frc2::PIDController>(rearRightController)),
      m_currentWheelSpeeds(currentWheelSpeeds),
      m_outputVolts(output),
      m_usePID(true) {
  AddRequirements(requirements);
  m_desiredRotation = [&] {
    return m_trajectory.States().back().pose.Rotation();
  };
}

MecanumControllerCommand::MecanumControllerCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    frc::SimpleMotorFeedforward<units::meters> feedforward,
    frc::MecanumDriveKinematics kinematics, frc2::PIDController xController,
    frc2::PIDController yController,
    frc::ProfiledPIDController<units::radians> thetaController,
    std::function<frc::Rotation2d()> desiredRotation,
    units::meters_per_second_t maxWheelVelocity,
    std::function<frc::MecanumDriveWheelSpeeds()> currentWheelSpeeds,
    frc2::PIDController frontLeftController,
    frc2::PIDController rearLeftController,
    frc2::PIDController frontRightController,
    frc2::PIDController rearRightController,
    std::function<void(units::volt_t, units::volt_t, units::volt_t,
                       units::volt_t)>
        output,
    wpi::ArrayRef<Subsystem*> requirements)
    : m_trajectory(trajectory),
      m_pose(pose),
      m_feedforward(feedforward),
      m_kinematics(kinematics),
      m_controller(xController, yController, thetaController),
      m_desiredRotation(desiredRotation),
      m_maxWheelVelocity(maxWheelVelocity),
      m_frontLeftController(
          std::make_unique<frc2::PIDController>(frontLeftController)),
      m_rearLeftController(
          std::make_unique<frc2::PIDController>(rearLeftController)),
      m_frontRightController(
          std::make_unique<frc2::PIDController>(frontRightController)),
      m_rearRightController(
          std::make_unique<frc2::PIDController>(rearRightController)),
      m_currentWheelSpeeds(currentWheelSpeeds),
      m_outputVolts(output),
      m_usePID(true) {
  AddRequirements(requirements);
}

MecanumControllerCommand::MecanumControllerCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    frc::SimpleMotorFeedforward<units::meters> feedforward,
    frc::MecanumDriveKinematics kinematics, frc2::PIDController xController,
    frc2::PIDController yController,
    frc::ProfiledPIDController<units::radians> thetaController,
    units::meters_per_second_t maxWheelVelocity,
    std::function<frc::MecanumDriveWheelSpeeds()> currentWheelSpeeds,
    frc2::PIDController frontLeftController,
    frc2::PIDController rearLeftController,
    frc2::PIDController frontRightController,
    frc2::PIDController rearRightController,
    std::function<void(units::volt_t, units::volt_t, units::volt_t,
                       units::volt_t)>
        output,
    wpi::ArrayRef<Subsystem*> requirements)
    : m_trajectory(trajectory),
      m_pose(pose),
      m_feedforward(feedforward),
      m_kinematics(kinematics),
      m_controller(xController, yController, thetaController),
      m_maxWheelVelocity(maxWheelVelocity),
      m_frontLeftController(
          std::make_unique<frc2::PIDController>(frontLeftController)),
      m_rearLeftController(
          std::make_unique<frc2::PIDController>(rearLeftController)),
      m_frontRightController(
          std::make_unique<frc2::PIDController>(frontRightController)),
      m_rearRightController(
          std::make_unique<frc2::PIDController>(rearRightController)),
      m_currentWheelSpeeds(currentWheelSpeeds),
      m_outputVolts(output),
      m_usePID(true) {
  AddRequirements(requirements);
  m_desiredRotation = [&] {
    return m_trajectory.States().back().pose.Rotation();
  };
}

MecanumControllerCommand::MecanumControllerCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    frc::MecanumDriveKinematics kinematics, frc2::PIDController xController,
    frc2::PIDController yController,
    frc::ProfiledPIDController<units::radians> thetaController,
    std::function<frc::Rotation2d()> desiredRotation,
    units::meters_per_second_t maxWheelVelocity,
    std::function<void(units::meters_per_second_t, units::meters_per_second_t,
                       units::meters_per_second_t, units::meters_per_second_t)>
        output,
    std::initializer_list<Subsystem*> requirements)
    : m_trajectory(trajectory),
      m_pose(pose),
      m_kinematics(kinematics),
      m_controller(xController, yController, thetaController),
      m_desiredRotation(desiredRotation),
      m_maxWheelVelocity(maxWheelVelocity),
      m_outputVel(output),
      m_usePID(false) {
  AddRequirements(requirements);
}

MecanumControllerCommand::MecanumControllerCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    frc::MecanumDriveKinematics kinematics, frc2::PIDController xController,
    frc2::PIDController yController,
    frc::ProfiledPIDController<units::radians> thetaController,
    units::meters_per_second_t maxWheelVelocity,
    std::function<void(units::meters_per_second_t, units::meters_per_second_t,
                       units::meters_per_second_t, units::meters_per_second_t)>
        output,
    std::initializer_list<Subsystem*> requirements)
    : m_trajectory(trajectory),
      m_pose(pose),
      m_kinematics(kinematics),
      m_controller(xController, yController, thetaController),
      m_maxWheelVelocity(maxWheelVelocity),
      m_outputVel(output),
      m_usePID(false) {
  AddRequirements(requirements);
  m_desiredRotation = [&] {
    return m_trajectory.States().back().pose.Rotation();
  };
}

MecanumControllerCommand::MecanumControllerCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    frc::MecanumDriveKinematics kinematics, frc2::PIDController xController,
    frc2::PIDController yController,
    frc::ProfiledPIDController<units::radians> thetaController,
    std::function<frc::Rotation2d()> desiredRotation,
    units::meters_per_second_t maxWheelVelocity,
    std::function<void(units::meters_per_second_t, units::meters_per_second_t,
                       units::meters_per_second_t, units::meters_per_second_t)>
        output,
    wpi::ArrayRef<Subsystem*> requirements)
    : m_trajectory(trajectory),
      m_pose(pose),
      m_kinematics(kinematics),
      m_controller(xController, yController, thetaController),
      m_desiredRotation(desiredRotation),
      m_maxWheelVelocity(maxWheelVelocity),
      m_outputVel(output),
      m_usePID(false) {
  AddRequirements(requirements);
}

MecanumControllerCommand::MecanumControllerCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    frc::MecanumDriveKinematics kinematics, frc2::PIDController xController,
    frc2::PIDController yController,
    frc::ProfiledPIDController<units::radians> thetaController,
    units::meters_per_second_t maxWheelVelocity,
    std::function<void(units::meters_per_second_t, units::meters_per_second_t,
                       units::meters_per_second_t, units::meters_per_second_t)>
        output,
    wpi::ArrayRef<Subsystem*> requirements)
    : m_trajectory(trajectory),
      m_pose(pose),
      m_kinematics(kinematics),
      m_controller(xController, yController, thetaController),
      m_maxWheelVelocity(maxWheelVelocity),
      m_outputVel(output),
      m_usePID(false) {
  AddRequirements(requirements);
  m_desiredRotation = [&] {
    return m_trajectory.States().back().pose.Rotation();
  };
}

void MecanumControllerCommand::Initialize() {
  m_prevTime = 0_s;
  auto initialState = m_trajectory.Sample(0_s);

  auto initialXVelocity =
      initialState.velocity * initialState.pose.Rotation().Cos();
  auto initialYVelocity =
      initialState.velocity * initialState.pose.Rotation().Sin();

  m_prevSpeeds = m_kinematics.ToWheelSpeeds(
      frc::ChassisSpeeds{initialXVelocity, initialYVelocity, 0_rad_per_s});

  m_timer.Reset();
  m_timer.Start();
  if (m_usePID) {
    m_frontLeftController->Reset();
    m_rearLeftController->Reset();
    m_frontRightController->Reset();
    m_rearRightController->Reset();
  }
}

void MecanumControllerCommand::Execute() {
  auto curTime = second_t(m_timer.Get());
  auto dt = curTime - m_prevTime;

  auto m_desiredState = m_trajectory.Sample(curTime);

  auto targetChassisSpeeds =
      m_controller.Calculate(m_pose(), m_desiredState, m_desiredRotation());
  auto targetWheelSpeeds = m_kinematics.ToWheelSpeeds(targetChassisSpeeds);

  targetWheelSpeeds.Normalize(m_maxWheelVelocity);

  auto frontLeftSpeedSetpoint = targetWheelSpeeds.frontLeft;
  auto rearLeftSpeedSetpoint = targetWheelSpeeds.rearLeft;
  auto frontRightSpeedSetpoint = targetWheelSpeeds.frontRight;
  auto rearRightSpeedSetpoint = targetWheelSpeeds.rearRight;

  if (m_usePID) {
    auto frontLeftFeedforward = m_feedforward.Calculate(
        frontLeftSpeedSetpoint,
        (frontLeftSpeedSetpoint - m_prevSpeeds.frontLeft) / dt);

    auto rearLeftFeedforward = m_feedforward.Calculate(
        rearLeftSpeedSetpoint,
        (rearLeftSpeedSetpoint - m_prevSpeeds.rearLeft) / dt);

    auto frontRightFeedforward = m_feedforward.Calculate(
        frontRightSpeedSetpoint,
        (frontRightSpeedSetpoint - m_prevSpeeds.frontRight) / dt);

    auto rearRightFeedforward = m_feedforward.Calculate(
        rearRightSpeedSetpoint,
        (rearRightSpeedSetpoint - m_prevSpeeds.rearRight) / dt);

    auto frontLeftOutput = volt_t(m_frontLeftController->Calculate(
                               m_currentWheelSpeeds().frontLeft.to<double>(),
                               frontLeftSpeedSetpoint.to<double>())) +
                           frontLeftFeedforward;
    auto rearLeftOutput = volt_t(m_rearLeftController->Calculate(
                              m_currentWheelSpeeds().rearLeft.to<double>(),
                              rearLeftSpeedSetpoint.to<double>())) +
                          rearLeftFeedforward;
    auto frontRightOutput = volt_t(m_frontRightController->Calculate(
                                m_currentWheelSpeeds().frontRight.to<double>(),
                                frontRightSpeedSetpoint.to<double>())) +
                            frontRightFeedforward;
    auto rearRightOutput = volt_t(m_rearRightController->Calculate(
                               m_currentWheelSpeeds().rearRight.to<double>(),
                               rearRightSpeedSetpoint.to<double>())) +
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

void MecanumControllerCommand::End(bool interrupted) { m_timer.Stop(); }

bool MecanumControllerCommand::IsFinished() {
  return m_timer.HasElapsed(m_trajectory.TotalTime());
}
