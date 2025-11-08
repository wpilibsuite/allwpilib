// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/DifferentialDrivetrainSim.hpp"

#include <utility>

#include "wpi/math/system/NumericalIntegration.hpp"
#include "wpi/math/system/plant/LinearSystemId.hpp"
#include "wpi/math/util/StateSpaceUtil.hpp"
#include "wpi/system/RobotController.hpp"
#include "wpi/util/MathExtras.hpp"

using namespace wpi;
using namespace wpi::sim;

DifferentialDrivetrainSim::DifferentialDrivetrainSim(
    wpi::math::LinearSystem<2, 2, 2> plant, wpi::units::meter_t trackwidth, wpi::math::DCMotor driveMotor,
    double gearRatio, wpi::units::meter_t wheelRadius,
    const std::array<double, 7>& measurementStdDevs)
    : m_plant(std::move(plant)),
      m_rb(trackwidth / 2.0),
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
    wpi::math::DCMotor driveMotor, double gearing, wpi::units::kilogram_square_meter_t J,
    wpi::units::kilogram_t mass, wpi::units::meter_t wheelRadius,
    wpi::units::meter_t trackwidth, const std::array<double, 7>& measurementStdDevs)
    : DifferentialDrivetrainSim(
          wpi::math::LinearSystemId::DrivetrainVelocitySystem(
              driveMotor, mass, wheelRadius, trackwidth / 2.0, J, gearing),
          trackwidth, driveMotor, gearing, wheelRadius, measurementStdDevs) {}

Eigen::Vector2d DifferentialDrivetrainSim::ClampInput(
    const Eigen::Vector2d& u) {
  return wpi::math::DesaturateInputVector<2>(u,
                                       wpi::RobotController::GetInputVoltage());
}

void DifferentialDrivetrainSim::SetInputs(wpi::units::volt_t leftVoltage,
                                          wpi::units::volt_t rightVoltage) {
  m_u << leftVoltage.value(), rightVoltage.value();
  m_u = ClampInput(m_u);
}

void DifferentialDrivetrainSim::SetGearing(double newGearing) {
  m_currentGearing = newGearing;
}

void DifferentialDrivetrainSim::Update(wpi::units::second_t dt) {
  m_x = wpi::math::RKDP([this](auto& x, auto& u) { return Dynamics(x, u); }, m_x, m_u, dt);
  m_y = m_x + wpi::math::MakeWhiteNoiseVector<7>(m_measurementStdDevs);
}

double DifferentialDrivetrainSim::GetGearing() const {
  return m_currentGearing;
}

wpi::math::Vectord<7> DifferentialDrivetrainSim::GetOutput() const {
  return m_y;
}

wpi::math::Vectord<7> DifferentialDrivetrainSim::GetState() const {
  return m_x;
}

double DifferentialDrivetrainSim::GetOutput(int output) const {
  return m_y(output);
}

double DifferentialDrivetrainSim::GetState(int state) const {
  return m_x(state);
}

wpi::math::Rotation2d DifferentialDrivetrainSim::GetHeading() const {
  return wpi::units::radian_t{GetOutput(State::kHeading)};
}

wpi::math::Pose2d DifferentialDrivetrainSim::GetPose() const {
  return wpi::math::Pose2d{wpi::units::meter_t{GetOutput(State::kX)},
                wpi::units::meter_t{GetOutput(State::kY)}, GetHeading()};
}

wpi::units::ampere_t DifferentialDrivetrainSim::GetLeftCurrentDraw() const {
  return m_motor.Current(wpi::units::radians_per_second_t{m_x(State::kLeftVelocity) *
                                                     m_currentGearing /
                                                     m_wheelRadius.value()},
                         wpi::units::volt_t{m_u(0)}) *
         wpi::util::sgn(m_u(0));
}

wpi::units::ampere_t DifferentialDrivetrainSim::GetRightCurrentDraw() const {
  return m_motor.Current(
             wpi::units::radians_per_second_t{m_x(State::kRightVelocity) *
                                         m_currentGearing /
                                         m_wheelRadius.value()},
             wpi::units::volt_t{m_u(1)}) *
         wpi::util::sgn(m_u(1));
}

wpi::units::ampere_t DifferentialDrivetrainSim::GetCurrentDraw() const {
  return GetLeftCurrentDraw() + GetRightCurrentDraw();
}

void DifferentialDrivetrainSim::SetState(const wpi::math::Vectord<7>& state) {
  m_x = state;
}

void DifferentialDrivetrainSim::SetPose(const wpi::math::Pose2d& pose) {
  m_x(State::kX) = pose.X().value();
  m_x(State::kY) = pose.Y().value();
  m_x(State::kHeading) = pose.Rotation().Radians().value();
  m_x(State::kLeftPosition) = 0;
  m_x(State::kRightPosition) = 0;
}

wpi::math::Vectord<7> DifferentialDrivetrainSim::Dynamics(const wpi::math::Vectord<7>& x,
                                               const Eigen::Vector2d& u) {
  // Because G² can be factored out of A, we can divide by the old ratio
  // squared and multiply by the new ratio squared to get a new drivetrain
  // model.
  wpi::math::Matrixd<4, 2> B;
  B.block<2, 2>(0, 0) = m_plant.B() * m_currentGearing * m_currentGearing /
                        m_originalGearing / m_originalGearing;
  B.block<2, 2>(2, 0).setZero();

  // Because G can be factored out of B, we can divide by the old ratio and
  // multiply by the new ratio to get a new drivetrain model.
  wpi::math::Matrixd<4, 4> A;
  A.block<2, 2>(0, 0) = m_plant.A() * m_currentGearing / m_originalGearing;

  A.block<2, 2>(2, 0).setIdentity();
  A.block<4, 2>(0, 2).setZero();

  double v = (x(State::kLeftVelocity) + x(State::kRightVelocity)) / 2.0;

  wpi::math::Vectord<7> xdot;
  xdot(0) = v * std::cos(x(State::kHeading));
  xdot(1) = v * std::sin(x(State::kHeading));
  xdot(2) =
      ((x(State::kRightVelocity) - x(State::kLeftVelocity)) / (2.0 * m_rb))
          .value();
  xdot.block<4, 1>(3, 0) = A * x.block<4, 1>(3, 0) + B * u;
  return xdot;
}
