// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/HDrivetrainSim.h"

#include <frc/system/plant/LinearSystemId.h>

#include <utility>

#include <wpi/MathExtras.h>

#include "frc/RobotController.h"
#include "frc/system/NumericalIntegration.h"

using namespace frc;
using namespace frc::sim;

HDrivetrainSim::HDrivetrainSim(
    LinearSystem<2, 2, 2> differentialPlant, LinearSystem<1, 1, 1> lateralPlant, units::meter_t trackWidth, DCMotor driveMotor,
    double gearRatio, units::meter_t wheelRadius,
    const std::array<double, 9>& measurementStdDevs)
    : m_differentialPlant(std::move(differentialPlant)),
      m_lateralPlant(std::move(lateralPlant)),
      m_rb(trackWidth / 2.0),
      m_wheelRadius(wheelRadius),
      m_motor(driveMotor),
      m_currentGearing(gearRatio),
      m_measurementStdDevs(measurementStdDevs) {
  m_x.setZero();
  m_u.setZero();
  m_y.setZero();
}

HDrivetrainSim::HDrivetrainSim(
    frc::DCMotor driveMotor, double gearing, units::kilogram_square_meter_t J,
    units::kilogram_t mass, units::meter_t wheelRadius,
    units::meter_t trackWidth, units::meter_t lateralWheelOffsetFromCenterOfGravity,
    const std::array<double, 9>& measurementStdDevs)
    : HDrivetrainSim(
          frc::LinearSystemId::DrivetrainVelocitySystem(
              driveMotor, mass, wheelRadius, trackWidth / 2.0, J, gearing),
          frc::LinearSystemId::CreateHDriveLateralVelocitySystem(
            driveMotor, mass, wheelRadius, lateralWheelOffsetFromCenterOfGravity, J, gearing
          ),
          trackWidth, driveMotor, gearing, wheelRadius,
          measurementStdDevs) {}

Vectord<3> HDrivetrainSim::ClampInput(const Vectord<3>& u) {
  return frc::DesaturateInputVector<3>(u,
                                       frc::RobotController::GetInputVoltage());
}

void HDrivetrainSim::SetInputs(units::volt_t leftVoltage,
                               units::volt_t rightVoltage,
                               units::volt_t lateralVoltage) {
  m_u << leftVoltage.value(), rightVoltage.value(), lateralVoltage.value();
  m_u = ClampInput(m_u);
}

void HDrivetrainSim::SetGearing(double newGearing) {
  m_currentGearing = newGearing;
}

void HDrivetrainSim::Update(units::second_t dt) {
  m_x = RK4([this](auto& x, auto& u) { return Dynamics(x, u); }, m_x, m_u, dt);
  m_y = m_x + frc::MakeWhiteNoiseVector<9>(m_measurementStdDevs);
}

double HDrivetrainSim::GetGearing() const {
  return m_currentGearing;
}

Vectord<9> HDrivetrainSim::GetOutput() const {
  return m_y;
}

Vectord<9> HDrivetrainSim::GetState() const {
  return m_x;
}

double HDrivetrainSim::GetOutput(int output) const {
  return m_y(output);
}

double HDrivetrainSim::GetState(int state) const {
  return m_x(state);
}

Rotation2d HDrivetrainSim::GetHeading() const {
  return units::radian_t{GetOutput(State::kHeading)};
}

Pose2d HDrivetrainSim::GetPose() const {
  return Pose2d{units::meter_t{GetOutput(State::kX)},
                units::meter_t{GetOutput(State::kY)}, GetHeading()};
}

units::ampere_t HDrivetrainSim::GetLeftCurrentDraw() const {
  auto loadIleft =
      m_motor.Current(
          units::radians_per_second_t{m_x(State::kLeftVelocity) *
                                      m_currentGearing / m_wheelRadius.value()},
          units::volt_t{m_u(0)}) *
      wpi::sgn(m_u(0));

  return loadIleft;
}

units::ampere_t HDrivetrainSim::GetRightCurrentDraw() const {
  auto loadIRight =
      m_motor.Current(
          units::radians_per_second_t{m_x(State::kRightVelocity) *
                                      m_currentGearing / m_wheelRadius.value()},
          units::volt_t{m_u(1)}) *
      wpi::sgn(m_u(1));

  return loadIRight;
}

units::ampere_t HDrivetrainSim::GetLateralCurrentDraw() const {
  auto loadILateral =
      m_motor.Current(
          units::radians_per_second_t{m_x(State::kLateralVelocity) *
                                      m_currentGearing / m_wheelRadius.value()},
          units::volt_t{m_u(1)}) *
      wpi::sgn(m_u(1));

  return loadILateral;
}

units::ampere_t HDrivetrainSim::GetCurrentDraw() const {
  return GetLeftCurrentDraw() + GetRightCurrentDraw();
}

void HDrivetrainSim::SetState(const Vectord<9>& state) {
  m_x = state;
}

void HDrivetrainSim::SetPose(const frc::Pose2d& pose) {
  m_x(State::kX) = pose.X().value();
  m_x(State::kY) = pose.Y().value();
  m_x(State::kHeading) = pose.Rotation().Radians().value();
  m_x(State::kLeftPosition) = 0;
  m_x(State::kRightPosition) = 0;
}

Vectord<9> HDrivetrainSim::Dynamics(const Vectord<9>& x,
                                               const Vectord<3>& u) {
  // Because G² can be factored out of A, we can divide by the old ratio
  // squared and multiply by the new ratio squared to get a new drivetrain
  // model.
  Matrixd<6, 3, 0 ,6 , 3> B;
  
  B.block<2, 2>(0, 0) = m_differentialPlant.B();
  B.block<1, 1>(2, 0) = m_lateralPlant.B();

  // Because G can be factored out of B, we can divide by the old ratio and
  // multiply by the new ratio to get a new drivetrain model.
  Matrixd<6, 6, 0, 6 ,6> A;
  A.setIdentity();
  A.block<2, 2>(0, 0) = m_differentialPlant.A();

  A.block<1, 1>(2, 2) = m_lateralPlant.A();
  A.block<3, 3>(3, 0).setIdentity();

  double v = (x(State::kLeftVelocity) + x(State::kRightVelocity)) / 2.0;

  Vectord<9> xdot;
  xdot.setIdentity();
  xdot(0, 0) = v * std::cos(x(State::kHeading)) + (x(State::kLateralVelocity) * std::sin(x(State::kHeading)));
  xdot(1, 0) = v * std::sin(x(State::kHeading)) + (x(State::kLateralVelocity) * std::cos(x(State::kHeading)));
  xdot(2, 0) =
      ((x(State::kRightVelocity) - x(State::kLeftVelocity)) / (2.0 * m_rb))
          .value();
  xdot.block<6, 1>(3, 0) = A * x.block<6, 1>(3, 0) + B * u;
  return xdot;
}
