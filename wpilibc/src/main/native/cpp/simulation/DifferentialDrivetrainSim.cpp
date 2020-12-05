/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/simulation/DifferentialDrivetrainSim.h"

#include <frc/system/plant/LinearSystemId.h>

#include "frc/system/RungeKutta.h"

using namespace frc;
using namespace frc::sim;

DifferentialDrivetrainSim::DifferentialDrivetrainSim(
    const LinearSystem<2, 2, 2>& plant, units::meter_t trackWidth,
    DCMotor driveMotor, double gearRatio, units::meter_t wheelRadius,
    const std::array<double, 7>& measurementStdDevs)
    : m_plant(plant),
      m_rb(trackWidth / 2.0),
      m_wheelRadius(wheelRadius),
      m_motor(driveMotor),
      m_originalGearing(gearRatio),
      m_currentGearing(gearRatio),
      m_measurementStdDevs(measurementStdDevs) {
  m_x.setZero();
  m_u.setZero();
  m_y.setZero();
}

DifferentialDrivetrainSim::DifferentialDrivetrainSim(
    frc::DCMotor driveMotor, double gearing, units::kilogram_square_meter_t J,
    units::kilogram_t mass, units::meter_t wheelRadius,
    units::meter_t trackWidth, const std::array<double, 7>& measurementStdDevs)
    : DifferentialDrivetrainSim(
          frc::LinearSystemId::DrivetrainVelocitySystem(
              driveMotor, mass, wheelRadius, trackWidth / 2.0, J, gearing),
          trackWidth, driveMotor, gearing, wheelRadius, measurementStdDevs) {}

void DifferentialDrivetrainSim::SetInputs(units::volt_t leftVoltage,
                                          units::volt_t rightVoltage) {
  m_u << leftVoltage.to<double>(), rightVoltage.to<double>();
}

void DifferentialDrivetrainSim::SetGearing(double newGearing) {
  m_currentGearing = newGearing;
}

void DifferentialDrivetrainSim::Update(units::second_t dt) {
  m_x = RungeKutta([this](auto& x, auto& u) { return Dynamics(x, u); }, m_x,
                   m_u, dt);
  m_y = m_x + frc::MakeWhiteNoiseVector<7>(m_measurementStdDevs);
}

double DifferentialDrivetrainSim::GetGearing() const {
  return m_currentGearing;
}

Eigen::Matrix<double, 7, 1> DifferentialDrivetrainSim::GetOutput() const {
  return m_y;
}

Eigen::Matrix<double, 7, 1> DifferentialDrivetrainSim::GetState() const {
  return m_x;
}

double DifferentialDrivetrainSim::GetOutput(int output) const {
  return m_y(output);
}

double DifferentialDrivetrainSim::GetState(int state) const {
  return m_x(state);
}

Rotation2d DifferentialDrivetrainSim::GetHeading() const {
  return Rotation2d(units::radian_t(GetOutput(State::kHeading)));
}

Pose2d DifferentialDrivetrainSim::GetPose() const {
  return Pose2d(units::meter_t(GetOutput(State::kX)),
                units::meter_t(GetOutput(State::kY)), GetHeading());
}

units::ampere_t DifferentialDrivetrainSim::GetCurrentDraw() const {
  auto loadIleft =
      m_motor.Current(units::radians_per_second_t(m_x(State::kLeftVelocity) *
                                                  m_currentGearing /
                                                  m_wheelRadius.to<double>()),
                      units::volt_t(m_u(0))) *
      wpi::sgn(m_u(0));

  auto loadIRight =
      m_motor.Current(units::radians_per_second_t(m_x(State::kRightVelocity) *
                                                  m_currentGearing /
                                                  m_wheelRadius.to<double>()),
                      units::volt_t(m_u(1))) *
      wpi::sgn(m_u(1));

  return loadIleft + loadIRight;
}

void DifferentialDrivetrainSim::SetState(
    const Eigen::Matrix<double, 7, 1>& state) {
  m_x = state;
}

void DifferentialDrivetrainSim::SetPose(const frc::Pose2d& pose) {
  m_x(State::kX) = pose.X().to<double>();
  m_x(State::kY) = pose.Y().to<double>();
  m_x(State::kHeading) = pose.Rotation().Radians().to<double>();
  m_x(State::kLeftPosition) = 0;
  m_x(State::kRightPosition) = 0;
}

Eigen::Matrix<double, 7, 1> DifferentialDrivetrainSim::Dynamics(
    const Eigen::Matrix<double, 7, 1>& x,
    const Eigen::Matrix<double, 2, 1>& u) {
  // Because G^2 can be factored out of A, we can divide by the old ratio
  // squared and multiply by the new ratio squared to get a new drivetrain
  // model.
  Eigen::Matrix<double, 4, 2> B;
  B.block<2, 2>(0, 0) = m_plant.B() * m_currentGearing * m_currentGearing /
                        m_originalGearing / m_originalGearing;
  B.block<2, 2>(2, 0).setZero();

  // Because G can be factored out of B, we can divide by the old ratio and
  // multiply by the new ratio to get a new drivetrain model.
  Eigen::Matrix<double, 4, 4> A;
  A.block<2, 2>(0, 0) = m_plant.A() * m_currentGearing / m_originalGearing;

  A.block<2, 2>(2, 0).setIdentity();
  A.block<4, 2>(0, 2).setZero();

  double v = (x(State::kLeftVelocity) + x(State::kRightVelocity)) / 2.0;

  Eigen::Matrix<double, 7, 1> xdot;
  xdot(0) = v * std::cos(x(State::kHeading));
  xdot(1) = v * std::sin(x(State::kHeading));
  xdot(2) =
      ((x(State::kRightVelocity) - x(State::kLeftVelocity)) / (2.0 * m_rb))
          .to<double>();
  xdot.block<4, 1>(3, 0) = A * x.block<4, 1>(3, 0) + B * u;
  return xdot;
}
