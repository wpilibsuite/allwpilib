/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/MecanumFollowerCommand.h"

using namespace frc2;
using namespace units;

template <typename T>
int sgn(T val) {
  return (T(0) < val) - (val < T(0));
}

MecanumFollowerCommand::MecanumFollowerCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    units::voltage::volt_t ks, units::unit_t<voltsecondspermeter> kv,
    units::unit_t<voltsecondssquaredpermeter> ka,
    frc::MecanumDriveKinematics kinematics, frc2::PIDController xController,
    frc2::PIDController yController, frc::ProfiledPIDController thetaController,
    std::function<frc::MecanumDriveWheelSpeeds()> currentWheelSpeeds,
    frc2::PIDController frontLeftController,
    frc2::PIDController rearLeftController,
    frc2::PIDController frontRightController,
    frc2::PIDController rearRightController,
    std::function<void(units::voltage::volt_t, units::voltage::volt_t,
                       units::voltage::volt_t, units::voltage::volt_t)>
        output,
    std::initializer_list<Subsystem*> requirements)
    : m_trajectory(trajectory),
      m_pose(pose),
      m_ks(ks),
      m_kv(kv),
      m_ka(ka),
      m_kinematics(kinematics),
      m_xController(std::make_unique<frc2::PIDController>(xController)),
      m_yController(std::make_unique<frc2::PIDController>(yController)),
      m_thetaController(
          std::make_unique<frc::ProfiledPIDController>(thetaController)),
      m_frontLeftController(
          std::make_unique<frc2::PIDController>(frontLeftController)),
      m_rearLeftController(
          std::make_unique<frc2::PIDController>(rearLeftController)),
      m_frontRightController(
          std::make_unique<frc2::PIDController>(frontRightController)),
      m_rearRightController(
          std::make_unique<frc2::PIDController>(rearRightController)),
      m_currentWheelSpeeds(currentWheelSpeeds),
      m_outputVolts(output) {
  AddRequirements(requirements);
}

MecanumFollowerCommand::MecanumFollowerCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    frc::MecanumDriveKinematics kinematics, frc2::PIDController xController,
    frc2::PIDController yController, frc::ProfiledPIDController thetaController,
    std::function<void(units::meters_per_second_t, units::meters_per_second_t,
                       units::meters_per_second_t, units::meters_per_second_t)>
        output,
    std::initializer_list<Subsystem*> requirements)
    : m_trajectory(trajectory),
      m_pose(pose),
      m_ks(0),
      m_kv(0),
      m_ka(0),
      m_kinematics(kinematics),
      m_xController(std::make_unique<frc2::PIDController>(xController)),
      m_yController(std::make_unique<frc2::PIDController>(yController)),
      m_thetaController(
          std::make_unique<frc::ProfiledPIDController>(thetaController)),
      m_outputVel(output) {
  AddRequirements(requirements);
}

void MecanumFollowerCommand::Initialize() {
  m_prevTime = 0_s;
  auto initialState = m_trajectory.Sample(0_s);

  auto initialXVelocity =
      initialState.velocity *
      std::sin(initialState.pose.Rotation().Radians().to<double>());
  auto initialYVelocity =
      initialState.velocity *
      std::cos(initialState.pose.Rotation().Radians().to<double>());

  m_prevSpeeds = m_kinematics.ToWheelSpeeds(
      frc::ChassisSpeeds{initialXVelocity, initialYVelocity,
                         initialState.curvature * initialState.velocity});

  m_timer.Reset();
  m_timer.Start();
}

void MecanumFollowerCommand::Execute() {
  auto curTime = second_t(m_timer.Get());
  auto dt = curTime - m_prevTime;

  auto m_desiredState = m_trajectory.Sample(curTime);
  auto m_desiredPose = m_desiredState.pose;

  auto m_poseError = m_desiredPose.RelativeTo(m_pose());

  auto targetXVel = velocity::meters_per_second_t(
      m_xController->Calculate((m_pose().Translation().X().to<double>()),
                               (m_desiredPose.Translation().X().to<double>())));
  auto targetYVel = velocity::meters_per_second_t(
      m_yController->Calculate((m_pose().Translation().Y().to<double>()),
                               (m_desiredPose.Translation().Y().to<double>())));
  // auto targetAngularVel =
  // angular_velocity::radians_per_second_t(m_thetaController->Calculate((m_pose().Rotation().Radians().to<double>()),
  // (m_finalPose.Rotation().Radians().to<double>())));

  auto vRef = m_desiredState.velocity;

  targetXVel += vRef * std::sin(m_poseError.Rotation().Radians().to<double>());
  targetYVel += vRef * std::cos(m_poseError.Rotation().Radians().to<double>());

  auto targetChassisSpeeds = frc::ChassisSpeeds{
      targetXVel, targetYVel,
      angular_velocity::radians_per_second_t(0) /*targetAngularVel*/};

  auto targetWheelSpeeds = m_kinematics.ToWheelSpeeds(targetChassisSpeeds);

  auto frontLeftSpeedSetpoint = targetWheelSpeeds.frontLeft;
  auto rearLeftSpeedSetpoint = targetWheelSpeeds.rearLeft;
  auto frontRightSpeedSetpoint = targetWheelSpeeds.frontRight;
  auto rearRightSpeedSetpoint = targetWheelSpeeds.rearRight;

  if (m_frontLeftController.get() != nullptr) {
    auto frontLeftFeedforward =
        m_ks * sgn(frontLeftSpeedSetpoint) + m_kv * frontLeftSpeedSetpoint +
        m_ka * (frontLeftSpeedSetpoint - m_prevSpeeds.frontLeft) / dt;

    auto rearLeftFeedforward =
        m_ks * sgn(rearLeftSpeedSetpoint) + m_kv * rearLeftSpeedSetpoint +
        m_ka * (rearLeftSpeedSetpoint - m_prevSpeeds.rearLeft) / dt;

    auto frontRightFeedforward =
        m_ks * sgn(frontRightSpeedSetpoint) + m_kv * frontRightSpeedSetpoint +
        m_ka * (frontRightSpeedSetpoint - m_prevSpeeds.frontRight) / dt;

    auto rearRightFeedforward =
        m_ks * sgn(rearRightSpeedSetpoint) + m_kv * rearRightSpeedSetpoint +
        m_ka * (rearRightSpeedSetpoint - m_prevSpeeds.rearRight) / dt;

    auto frontLeftOutput = voltage::volt_t(m_frontLeftController->Calculate(
                               m_currentWheelSpeeds().frontLeft.to<double>(),
                               frontLeftSpeedSetpoint.to<double>())) +
                           frontLeftFeedforward;
    auto rearLeftOutput = voltage::volt_t(m_rearLeftController->Calculate(
                              m_currentWheelSpeeds().rearLeft.to<double>(),
                              rearLeftSpeedSetpoint.to<double>())) +
                          rearLeftFeedforward;
    auto frontRightOutput = voltage::volt_t(m_frontRightController->Calculate(
                                m_currentWheelSpeeds().frontRight.to<double>(),
                                frontRightSpeedSetpoint.to<double>())) +
                            frontRightFeedforward;
    auto rearRightOutput = voltage::volt_t(m_rearRightController->Calculate(
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

void MecanumFollowerCommand::End(bool interrupted) { m_timer.Stop(); }

bool MecanumFollowerCommand::IsFinished() {
  return second_t(m_timer.Get()) - m_trajectory.TotalTime() >= 0_s;
}
