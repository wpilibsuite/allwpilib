// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/ElevatorSim.hpp"

#include "wpi/math/system/Models.hpp"
#include "wpi/math/system/NumericalIntegration.hpp"
#include "wpi/system/RobotController.hpp"
#include "wpi/util/MathExtras.hpp"

using namespace wpi;
using namespace wpi::sim;

ElevatorSim::ElevatorSim(const wpi::math::LinearSystem<2, 1, 2>& plant,
                         const wpi::math::DCMotor& gearbox,
                         wpi::units::meters<> minHeight,
                         wpi::units::meters<> maxHeight, bool simulateGravity,
                         wpi::units::meters<> startingHeight,
                         const std::array<double, 2>& measurementStdDevs)
    : LinearSystemSim(plant, measurementStdDevs),
      m_gearbox(gearbox),
      m_minHeight(minHeight),
      m_maxHeight(maxHeight),
      m_simulateGravity(simulateGravity) {
  SetState(startingHeight, 0_mps);
}

ElevatorSim::ElevatorSim(const wpi::math::DCMotor& gearbox, double gearing,
                         wpi::units::kilograms<> carriageMass,
                         wpi::units::meters<> drumRadius,
                         wpi::units::meters<> minHeight,
                         wpi::units::meters<> maxHeight, bool simulateGravity,
                         wpi::units::meters<> startingHeight,
                         const std::array<double, 2>& measurementStdDevs)
    : ElevatorSim(wpi::math::Models::ElevatorFromPhysicalConstants(
                      gearbox, carriageMass, drumRadius, gearing),
                  gearbox, minHeight, maxHeight, simulateGravity,
                  startingHeight, measurementStdDevs) {}

template <typename Distance>
  requires std::same_as<wpi::units::meters_, Distance> ||
           std::same_as<wpi::units::radians_, Distance>
ElevatorSim::ElevatorSim(decltype(1_V / Velocity_t<Distance>(1)) kV,
                         decltype(1_V / Acceleration_t<Distance>(1)) kA,
                         const wpi::math::DCMotor& gearbox,
                         wpi::units::meters<> minHeight,
                         wpi::units::meters<> maxHeight, bool simulateGravity,
                         wpi::units::meters<> startingHeight,
                         const std::array<double, 2>& measurementStdDevs)
    : ElevatorSim(wpi::math::Models::ElevatorFromSysId(kV, kA), gearbox,
                  minHeight, maxHeight, simulateGravity, startingHeight,
                  measurementStdDevs) {}

void ElevatorSim::SetState(wpi::units::meters<> position,
                           wpi::units::meters_per_second<> velocity) {
  SetState(wpi::math::Vectord<2>{std::clamp(position, m_minHeight, m_maxHeight),
                                 velocity});
}

bool ElevatorSim::WouldHitLowerLimit(
    wpi::units::meters<> elevatorHeight) const {
  return elevatorHeight <= m_minHeight;
}

bool ElevatorSim::WouldHitUpperLimit(
    wpi::units::meters<> elevatorHeight) const {
  return elevatorHeight >= m_maxHeight;
}

bool ElevatorSim::HasHitLowerLimit() const {
  return WouldHitLowerLimit(wpi::units::meters<>{m_y(0)});
}

bool ElevatorSim::HasHitUpperLimit() const {
  return WouldHitUpperLimit(wpi::units::meters<>{m_y(0)});
}

wpi::units::meters<> ElevatorSim::GetPosition() const {
  return wpi::units::meters<>{m_y(0)};
}

wpi::units::meters_per_second<> ElevatorSim::GetVelocity() const {
  return wpi::units::meters_per_second<>{m_x(1)};
}

wpi::units::amperes<> ElevatorSim::GetCurrentDraw() const {
  // I = V / R - omega / (Kv * R)
  // Reductions are greater than 1, so a reduction of 10:1 would mean the motor
  // is spinning 10x faster than the output.

  double kA = 1.0 / m_plant.B(1, 0);
  using Kv_t = wpi::units::unit<wpi::units::compound_conversion_factor<
      wpi::units::volts_, wpi::units::inverse<wpi::units::meters_per_second_>>>;
  Kv_t Kv = Kv_t{-kA * m_plant.A(1, 1)};
  wpi::units::meters_per_second<> velocity{m_x(1)};
  wpi::units::radians_per_second<> motorVelocity = velocity * Kv * m_gearbox.Kv;

  // Perform calculation and return.
  return m_gearbox.Current(motorVelocity, wpi::units::volts<>{m_u(0)}) *
         wpi::util::sgn(m_u(0));
}

void ElevatorSim::SetInputVoltage(wpi::units::volts<> voltage) {
  SetInput(wpi::math::Vectord<1>{voltage.value()});
  ClampInput(wpi::RobotController::GetBatteryVoltage().value());
}

wpi::math::Vectord<2> ElevatorSim::UpdateX(
    const wpi::math::Vectord<2>& currentXhat, const wpi::math::Vectord<1>& u,
    wpi::units::seconds<> dt) {
  auto updatedXhat = wpi::math::RKDP(
      [&](const wpi::math::Vectord<2>& x,
          const wpi::math::Vectord<1>& u_) -> wpi::math::Vectord<2> {
        wpi::math::Vectord<2> xdot = m_plant.A() * x + m_plant.B() * u;

        if (m_simulateGravity) {
          xdot += wpi::math::Vectord<2>{0.0, -9.8};
        }
        return xdot;
      },
      currentXhat, u, dt);
  // Check for collision after updating x-hat.
  if (WouldHitLowerLimit(wpi::units::meters<>{updatedXhat(0)})) {
    return wpi::math::Vectord<2>{m_minHeight.value(), 0.0};
  }
  if (WouldHitUpperLimit(wpi::units::meters<>{updatedXhat(0)})) {
    return wpi::math::Vectord<2>{m_maxHeight.value(), 0.0};
  }
  return updatedXhat;
}
