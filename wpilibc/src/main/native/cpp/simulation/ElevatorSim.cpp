/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/simulation/ElevatorSim.h"

#include "frc/StateSpaceUtil.h"
#include "frc/system/RungeKutta.h"
#include "frc/system/plant/LinearSystemId.h"

using namespace frc;
using namespace frc::sim;

ElevatorSim::ElevatorSim(const DCMotor& gearbox, units::kilogram_t carriageMass,
                         double gearing, units::meter_t drumRadius,
                         units::meter_t minHeight, units::meter_t maxHeight,
                         bool addNoise,
                         const std::array<double, 1>& m_measurementStdDevs)
    : LinearSystemSim(LinearSystemId::ElevatorSystem(gearbox, carriageMass,
                                                     drumRadius, gearing),
                      addNoise, m_measurementStdDevs),
      m_motor(gearbox),
      m_drumRadius(drumRadius),
      m_minHeight(minHeight),
      m_maxHeight(maxHeight),
      m_gearing(gearing) {}

bool ElevatorSim::HasHitLowerLimit(const Eigen::Matrix<double, 2, 1>& x) const {
  return x(0, 0) < m_minHeight.to<double>();
}

bool ElevatorSim::HasHitUpperLimit(const Eigen::Matrix<double, 2, 1>& x) const {
  return x(0, 0) > m_maxHeight.to<double>();
}

units::meter_t ElevatorSim::GetElevatorPosition() const {
  return units::meter_t(Y()(0, 0));
}

units::meters_per_second_t ElevatorSim::GetElevatorVelocity() const {
  return units::meters_per_second_t(m_x(1, 0));
}

units::ampere_t ElevatorSim::GetCurrentDraw() const {
  // I = V / R - omega / (Kv * R)
  // Reductions are greater than 1, so a reduction of 10:1 would mean the motor
  // is spinning 10x faster than the output.

  // v = r w, so w = v / r
  units::meters_per_second_t velocity = m_x(1, 0) * 1_mps;
  units::radians_per_second_t motorVelocity =
      velocity / m_drumRadius * m_gearing * 1_rad;

  // Perform calculation and return.
  return units::volt_t(m_u(0, 0)) / m_motor.R -
         motorVelocity / (m_motor.Kv * m_motor.R);
}

Eigen::Matrix<double, 2, 1> ElevatorSim::UpdateX(
    const Eigen::Matrix<double, 2, 1>& currentXhat,
    const Eigen::Matrix<double, 1, 1>& u, units::second_t dt) {
  auto updatedXhat = RungeKutta(
      [&](const Eigen::Matrix<double, 2, 1>& x,
          const Eigen::Matrix<double, 1, 1>& u_)
          -> Eigen::Matrix<double, 2, 1> {
        return m_plant.A() * x + m_plant.B() * u_ + MakeMatrix<2, 1>(0.0, -9.8);
      },
      currentXhat, u, dt);
  // Check for collision after updating x-hat.
  if (HasHitLowerLimit(updatedXhat)) {
    return MakeMatrix<2, 1>(m_minHeight.to<double>(), 0.0);
  } else if (HasHitUpperLimit(updatedXhat)) {
    return MakeMatrix<2, 1>(m_maxHeight.to<double>(), 0.0);
  }
  return updatedXhat;
}
