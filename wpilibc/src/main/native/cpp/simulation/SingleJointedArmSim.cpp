// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/SingleJointedArmSim.h"

#include <cmath>

#include <units/voltage.h>
#include <wpi/MathExtras.h>

#include "frc/system/NumericalIntegration.h"
#include "frc/system/plant/LinearSystemId.h"

using namespace frc;
using namespace frc::sim;

SingleJointedArmSim::SingleJointedArmSim(
    const LinearSystem<2, 1, 1>& system, const DCMotor& gearbox, double gearing,
    units::meter_t armLength, units::radian_t minAngle,
    units::radian_t maxAngle, units::kilogram_t mass, bool simulateGravity,
    const std::array<double, 1>& measurementStdDevs)
    : LinearSystemSim<2, 1, 1>(system, measurementStdDevs),
      m_r(armLength),
      m_minAngle(minAngle),
      m_maxAngle(maxAngle),
      m_mass(mass),
      m_gearbox(gearbox),
      m_gearing(gearing),
      m_simulateGravity(simulateGravity) {}

SingleJointedArmSim::SingleJointedArmSim(
    const DCMotor& gearbox, double gearing, units::kilogram_square_meter_t moi,
    units::meter_t armLength, units::radian_t minAngle,
    units::radian_t maxAngle, units::kilogram_t mass, bool simulateGravity,
    const std::array<double, 1>& measurementStdDevs)
    : SingleJointedArmSim(
          LinearSystemId::SingleJointedArmSystem(gearbox, moi, gearing),
          gearbox, gearing, armLength, minAngle, maxAngle, mass,
          simulateGravity, measurementStdDevs) {}

bool SingleJointedArmSim::WouldHitLowerLimit(units::radian_t armAngle) const {
  return armAngle < m_minAngle;
}

bool SingleJointedArmSim::WouldHitUpperLimit(units::radian_t armAngle) const {
  return armAngle > m_maxAngle;
}

bool SingleJointedArmSim::HasHitLowerLimit() const {
  return WouldHitLowerLimit(units::radian_t(m_y(0)));
}

bool SingleJointedArmSim::HasHitUpperLimit() const {
  return WouldHitUpperLimit(units::radian_t(m_y(0)));
}

units::radian_t SingleJointedArmSim::GetAngle() const {
  return units::radian_t{m_y(0)};
}

units::radians_per_second_t SingleJointedArmSim::GetVelocity() const {
  return units::radians_per_second_t{m_x(1)};
}

units::ampere_t SingleJointedArmSim::GetCurrentDraw() const {
  // Reductions are greater than 1, so a reduction of 10:1 would mean the motor
  // is spinning 10x faster than the output
  units::radians_per_second_t motorVelocity{m_x(1) * m_gearing};
  return m_gearbox.Current(motorVelocity, units::volt_t{m_u(0)}) *
         wpi::sgn(m_u(0));
}

void SingleJointedArmSim::SetInputVoltage(units::volt_t voltage) {
  SetInput(frc::MakeMatrix<1, 1>(voltage.to<double>()));
}

Eigen::Matrix<double, 2, 1> SingleJointedArmSim::UpdateX(
    const Eigen::Matrix<double, 2, 1>& currentXhat,
    const Eigen::Matrix<double, 1, 1>& u, units::second_t dt) {
  // Horizontal case:
  // Torque = F * r = I * alpha
  // alpha = F * r / I
  // Since F = mg,
  // alpha = m * g * r / I
  // Finally, multiply RHS by cos(theta) to account for the arm angle
  // This acceleration is added to the linear system dynamics x-dot = Ax + Bu
  // We therefore find that f(x, u) = Ax + Bu + [[0] [m * g * r / I *
  // std::cos(theta)]]

  Eigen::Matrix<double, 2, 1> updatedXhat = RKF45(
      [&](const auto& x, const auto& u) -> Eigen::Matrix<double, 2, 1> {
        Eigen::Matrix<double, 2, 1> xdot = m_plant.A() * x + m_plant.B() * u;

        if (m_simulateGravity) {
          xdot += MakeMatrix<2, 1>(0.0, (m_mass * m_r * -9.8 * 3.0 /
                                         (m_mass * m_r * m_r) * std::cos(x(0)))
                                            .template to<double>());
        }
        return xdot;
      },
      currentXhat, u, dt);

  // Check for collisions.
  if (WouldHitLowerLimit(units::radian_t(updatedXhat(0)))) {
    return MakeMatrix<2, 1>(m_minAngle.to<double>(), 0.0);
  } else if (WouldHitUpperLimit(units::radian_t(updatedXhat(0)))) {
    return MakeMatrix<2, 1>(m_maxAngle.to<double>(), 0.0);
  }
  return updatedXhat;
}
