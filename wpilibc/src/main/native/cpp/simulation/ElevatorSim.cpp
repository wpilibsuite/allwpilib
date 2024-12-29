// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/ElevatorSim.h"

#include <wpi/MathExtras.h>

#include "frc/RobotController.h"
#include "frc/system/NumericalIntegration.h"
#include "frc/system/plant/LinearSystemId.h"

using namespace frc;
using namespace frc::sim;

ElevatorSim::ElevatorSim(const LinearSystem<2, 1, 2>& plant,
                         const DCMotor& gearbox, units::meter_t minHeight,
                         units::meter_t maxHeight, bool simulateGravity,
                         units::meter_t startingHeight,
                         const std::array<double, 2>& measurementStdDevs)
    : LinearSystemSim(plant, measurementStdDevs),
      m_gearbox(gearbox),
      m_minHeight(minHeight),
      m_maxHeight(maxHeight),
      m_simulateGravity(simulateGravity) {
  SetState(startingHeight, 0_mps);
}

ElevatorSim::ElevatorSim(const DCMotor& gearbox, double gearing,
                         units::kilogram_t carriageMass,
                         units::meter_t drumRadius, units::meter_t minHeight,
                         units::meter_t maxHeight, bool simulateGravity,
                         units::meter_t startingHeight,
                         const std::array<double, 2>& measurementStdDevs)
    : ElevatorSim(LinearSystemId::ElevatorSystem(gearbox, carriageMass,
                                                 drumRadius, gearing),
                  gearbox, minHeight, maxHeight, simulateGravity,
                  startingHeight, measurementStdDevs) {}

template <typename Distance>
  requires std::same_as<units::meter, Distance> ||
           std::same_as<units::radian, Distance>
ElevatorSim::ElevatorSim(decltype(1_V / Velocity_t<Distance>(1)) kV,
                         decltype(1_V / Acceleration_t<Distance>(1)) kA,
                         const DCMotor& gearbox, units::meter_t minHeight,
                         units::meter_t maxHeight, bool simulateGravity,
                         units::meter_t startingHeight,
                         const std::array<double, 2>& measurementStdDevs)
    : ElevatorSim(LinearSystemId::IdentifyPositionSystem(kV, kA), gearbox,
                  minHeight, maxHeight, simulateGravity, startingHeight,
                  measurementStdDevs) {}

void ElevatorSim::SetState(units::meter_t position,
                           units::meters_per_second_t velocity) {
  SetState(
      Vectord<2>{std::clamp(position, m_minHeight, m_maxHeight), velocity});
}

bool ElevatorSim::WouldHitLowerLimit(units::meter_t elevatorHeight) const {
  return elevatorHeight <= m_minHeight;
}

bool ElevatorSim::WouldHitUpperLimit(units::meter_t elevatorHeight) const {
  return elevatorHeight >= m_maxHeight;
}

bool ElevatorSim::HasHitLowerLimit() const {
  return WouldHitLowerLimit(units::meter_t{m_y(0)});
}

bool ElevatorSim::HasHitUpperLimit() const {
  return WouldHitUpperLimit(units::meter_t{m_y(0)});
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

  double kA = 1.0 / m_plant.B(1, 0);
  using Kv_t = units::unit_t<units::compound_unit<
      units::volt, units::inverse<units::meters_per_second>>>;
  Kv_t Kv = Kv_t{kA * m_plant.A(1, 1)};
  units::meters_per_second_t velocity{m_x(1)};
  units::radians_per_second_t motorVelocity = velocity * Kv * m_gearbox.Kv;

  // Perform calculation and return.
  return m_gearbox.Current(motorVelocity, units::volt_t{m_u(0)}) *
         wpi::sgn(m_u(0));
}

void ElevatorSim::SetInputVoltage(units::volt_t voltage) {
  SetInput(Vectord<1>{voltage.value()});
  ClampInput(frc::RobotController::GetBatteryVoltage().value());
}

Vectord<2> ElevatorSim::UpdateX(const Vectord<2>& currentXhat,
                                const Vectord<1>& u, units::second_t dt) {
  auto updatedXhat = RKDP(
      [&](const Vectord<2>& x, const Vectord<1>& u_) -> Vectord<2> {
        Vectord<2> xdot = m_plant.A() * x + m_plant.B() * u;

        if (m_simulateGravity) {
          xdot += Vectord<2>{0.0, -9.8};
        }
        return xdot;
      },
      currentXhat, u, dt);
  // Check for collision after updating x-hat.
  if (WouldHitLowerLimit(units::meter_t{updatedXhat(0)})) {
    return Vectord<2>{m_minHeight.value(), 0.0};
  }
  if (WouldHitUpperLimit(units::meter_t{updatedXhat(0)})) {
    return Vectord<2>{m_maxHeight.value(), 0.0};
  }
  return updatedXhat;
}
