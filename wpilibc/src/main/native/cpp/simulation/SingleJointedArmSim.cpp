/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/simulation/SingleJointedArmSim.h"

#include <cmath>

#include "frc/system/RungeKutta.h"
#include "frc/system/plant/LinearSystemId.h"

using namespace frc;
using namespace frc::sim;

SingleJointedArmSim::SingleJointedArmSim(
    const LinearSystem<2, 1, 1>& system, units::kilogram_t mass,
    units::meter_t armLength, units::radian_t minAngle,
    units::radian_t maxAngle, bool addNoise,
    const std::array<double, 1>& measurementStdDevs)
    : LinearSystemSim<2, 1, 1>(system, addNoise, measurementStdDevs),
      m_r(armLength),
      m_minAngle(minAngle),
      m_maxAngle(maxAngle),
      m_mass(mass) {}

SingleJointedArmSim::SingleJointedArmSim(
    const DCMotor& motor, units::kilogram_square_meter_t j, double G,
    units::kilogram_t mass, units::meter_t armLength, units::radian_t minAngle,
    units::radian_t maxAngle, bool addNoise,
    const std::array<double, 1>& measurementStdDevs)
    : SingleJointedArmSim(LinearSystemId::SingleJointedArmSystem(motor, j, G),
                          mass, armLength, minAngle, maxAngle, addNoise,
                          measurementStdDevs) {}

SingleJointedArmSim::SingleJointedArmSim(
    const DCMotor& motor, double G, units::kilogram_t mass,
    units::meter_t armLength, units::radian_t minAngle,
    units::radian_t maxAngle, bool addNoise,
    const std::array<double, 1>& measurementStdDevs)
    : SingleJointedArmSim(
          LinearSystemId::SingleJointedArmSystem(
              motor, 1.0 / 3.0 * mass * armLength * armLength, G),
          mass, armLength, minAngle, maxAngle, addNoise, measurementStdDevs) {}

bool SingleJointedArmSim::HasHitLowerLimit(
    const Eigen::Matrix<double, 2, 1>& x) const {
  return x(0, 0) < m_minAngle.to<double>();
}

bool SingleJointedArmSim::HasHitUpperLimit(
    const Eigen::Matrix<double, 2, 1>& x) const {
  return x(0, 0) > m_maxAngle.to<double>();
}

units::radian_t SingleJointedArmSim::GetAngle() const {
  return units::radian_t(m_x(0, 0));
}

units::radians_per_second_t SingleJointedArmSim::GetVelocity() const {
  return units::radians_per_second_t(m_x(1, 0));
}

Eigen::Matrix<double, 2, 1> SingleJointedArmSim::UpdateX(
    const Eigen::Matrix<double, 2, 1>& currentXhat,
    const Eigen::Matrix<double, 1, 1>& u, units::second_t dt) {
  // Horizontal case:
  // Torque = F * r = I * alpha
  // alpha = F * r / I
  // Since F = mg,
  // alpha = m * g * r / I

  auto updatedXhat = RungeKutta(
      [&](const auto& x, const auto& u) -> Eigen::Matrix<double, 2, 1> {
        return m_plant.A() * x + m_plant.B() * u +
               MakeMatrix<2, 1>(0.0, (m_mass * m_r * -9.8 * 3.0 /
                                      (m_mass * m_r * m_r) * std::cos(x(0, 0)))
                                         .template to<double>());
      },
      currentXhat, u, dt);

  // Check for collisions.
  if (HasHitLowerLimit(updatedXhat)) {
    return MakeMatrix<2, 1>(m_minAngle.to<double>(), 0.0);
  } else if (HasHitUpperLimit(updatedXhat)) {
    return MakeMatrix<2, 1>(m_maxAngle.to<double>(), 0.0);
  }
  return updatedXhat;
}
