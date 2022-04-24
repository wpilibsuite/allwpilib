// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/ElevatorSim.h"

#include <wpi/MathExtras.h>

#include "frc/system/NumericalIntegration.h"
#include "frc/system/plant/LinearSystemId.h"

using namespace frc;
using namespace frc::sim;

ElevatorSim::ElevatorSim(const LinearSystem<2, 1, 1>& plant,
                         const DCMotor& gearbox, double gearing,
                         units::meter_t drumRadius, units::meter_t minHeight,
                         units::meter_t maxHeight, bool simulateGravity,
                         const std::array<double, 1>& measurementStdDevs)
    : LinearSystemSim(plant, measurementStdDevs),
      m_gearbox(gearbox),
      m_drumRadius(drumRadius),
      m_minHeight(minHeight),
      m_maxHeight(maxHeight),
      m_gearing(gearing),
      m_simulateGravity(simulateGravity) {}

ElevatorSim::ElevatorSim(const DCMotor& gearbox, double gearing,
                         units::kilogram_t carriageMass,
                         units::meter_t drumRadius, units::meter_t minHeight,
                         units::meter_t maxHeight, bool simulateGravity,
                         const std::array<double, 1>& measurementStdDevs)
    : LinearSystemSim(LinearSystemId::ElevatorSystem(gearbox, carriageMass,
                                                     drumRadius, gearing),
                      measurementStdDevs),
      m_gearbox(gearbox),
      m_drumRadius(drumRadius),
      m_minHeight(minHeight),
      m_maxHeight(maxHeight),
      m_gearing(gearing),
      m_simulateGravity(simulateGravity) {}

bool ElevatorSim::WouldHitLowerLimit(units::meter_t elevatorHeight) const {
  return elevatorHeight < m_minHeight;
}

bool ElevatorSim::WouldHitUpperLimit(units::meter_t elevatorHeight) const {
  return elevatorHeight > m_maxHeight;
}

bool ElevatorSim::HasHitLowerLimit() const {
  return WouldHitLowerLimit(units::meter_t(m_y(0)));
}

bool ElevatorSim::HasHitUpperLimit() const {
  return WouldHitUpperLimit(units::meter_t(m_y(0)));
}

units::meter_t ElevatorSim::GetPosition() const {
  return units::meter_t{m_y(0)};
}

units::meters_per_second_t ElevatorSim::GetVelocity() const {
  return units::meters_per_second_t{m_x(1)};
}

units::ampere_t ElevatorSim::GetCurrentDraw() const {
  // I = V / R - omega / (Kv * R)
  // Reductions are greater than 1, so a reduction of 10:1 would mean the motor
  // is spinning 10x faster than the output.

  // v = r w, so w = v / r
  units::meters_per_second_t velocity{m_x(1)};
  units::radians_per_second_t motorVelocity =
      velocity / m_drumRadius * m_gearing * 1_rad;

  // Perform calculation and return.
  return m_gearbox.Current(motorVelocity, units::volt_t{m_u(0)}) *
         wpi::sgn(m_u(0));
}

void ElevatorSim::SetInputVoltage(units::volt_t voltage) {
  SetInput(Eigen::Vector<double, 1>{voltage.value()});
}

Eigen::Vector<double, 2> ElevatorSim::UpdateX(
    const Eigen::Vector<double, 2>& currentXhat,
    const Eigen::Vector<double, 1>& u, units::second_t dt) {
  auto updatedXhat = RKDP(
      [&](const Eigen::Vector<double, 2>& x,
          const Eigen::Vector<double, 1>& u_) -> Eigen::Vector<double, 2> {
        Eigen::Vector<double, 2> xdot = m_plant.A() * x + m_plant.B() * u;

        if (m_simulateGravity) {
          xdot += Eigen::Vector<double, 2>{0.0, -9.8};
        }
        return xdot;
      },
      currentXhat, u, dt);
  // Check for collision after updating x-hat.
  if (WouldHitLowerLimit(units::meter_t(updatedXhat(0)))) {
    return Eigen::Vector<double, 2>{m_minHeight.value(), 0.0};
  }
  if (WouldHitUpperLimit(units::meter_t(updatedXhat(0)))) {
    return Eigen::Vector<double, 2>{m_maxHeight.value(), 0.0};
  }
  return updatedXhat;
}
