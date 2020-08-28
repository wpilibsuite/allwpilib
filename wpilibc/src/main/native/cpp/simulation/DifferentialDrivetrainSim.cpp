/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/simulation/DifferentialDrivetrainSim.h"

#include "frc/system/RungeKutta.h"

using namespace frc;
using namespace frc::sim;

DifferentialDrivetrainSim::DifferentialDrivetrainSim(
    LinearSystem<2, 2, 2>& plant, DifferentialDriveKinematics& kinematics,
    DCMotor leftGearbox, double leftGearing, DCMotor rightGearbox,
    double rightGearing, units::meter_t wheelRadius)
    : m_plant(plant),
      m_rb(kinematics.trackWidth / 2.0),
      m_wheelRadius(wheelRadius),
      m_leftMotor(leftGearbox),
      m_rightMotor(rightGearbox),
      m_leftGearing(leftGearing),
      m_rightGearing(rightGearing) {}

void DifferentialDrivetrainSim::SetInputs(units::volt_t leftVoltage,
                                          units::volt_t rightVoltage) {
  m_u << leftVoltage.to<double>(), rightVoltage.to<double>();
}

void DifferentialDrivetrainSim::Update(units::second_t dt) {
  m_x = RungeKutta([this](auto& x, auto& u) { return Dynamics(x, u); }, m_x,
                   m_u, dt);
}

double DifferentialDrivetrainSim::GetState(int state) const {
  return m_x(state);
}

Eigen::Matrix<double, 10, 1> DifferentialDrivetrainSim::GetState() const {
  return m_x;
}

Rotation2d DifferentialDrivetrainSim::GetHeading() {
  return Rotation2d(units::radian_t(m_x(State::kHeading)));
}

Pose2d DifferentialDrivetrainSim::GetEstimatedPosition() const {
  return Pose2d(units::meter_t(m_x(State::kX)), units::meter_t(m_x(State::kY)),
                Rotation2d(units::radian_t(m_x(State::kHeading))));
}

units::ampere_t DifferentialDrivetrainSim::GetCurrentDrawAmps() {
  auto loadIleft = m_leftMotor.Current(
                       units::radians_per_second_t(m_x(State::kLeftVelocity) *
                                                   m_leftGearing /
                                                   m_wheelRadius.to<double>()),
                       units::volt_t(m_u(0))) *
                   wpi::sgn(m_u(0));

  auto loadIRight = m_rightMotor.Current(
                        units::radians_per_second_t(m_x(State::kRightVelocity) *
                                                    m_rightGearing /
                                                    m_wheelRadius.to<double>()),
                        units::volt_t(m_u(1))) *
                    wpi::sgn(m_u(1));

  return loadIleft + loadIRight;
}

Eigen::Matrix<double, 10, 1> DifferentialDrivetrainSim::Dynamics(
    const Eigen::Matrix<double, 10, 1>& x,
    const Eigen::Matrix<double, 2, 1>& u) {
  Eigen::Matrix<double, 4, 2> B;
  B.block<2, 2>(0, 0) = m_plant.B();
  B.block<2, 2>(2, 0).setZero();
  Eigen::Matrix<double, 4, 7> A;
  A.block<2, 2>(0, 0) = m_plant.A();

  A.block<2, 2>(2, 0).setIdentity();
  A.block<4, 2>(0, 2).setZero();
  A.block<4, 2>(0, 4) = B;
  A.block<4, 1>(0, 6) << 0, 0, 1, -1;

  double v = (x(State::kLeftVelocity) + x(State::kRightVelocity)) / 2.0;

  Eigen::Matrix<double, 10, 1> result;
  result(0) = v * std::cos(x(State::kHeading));
  result(1) = v * std::sin(x(State::kHeading));
  result(2) =
      ((x(State::kRightVelocity) - x(State::kLeftVelocity)) / (2.0 * m_rb))
          .to<double>();
  result.block<4, 1>(3, 0) = A * x.block<7, 1>(3, 0) + B * u;
  result.block<3, 1>(7, 0).setZero();
  return result;
}
