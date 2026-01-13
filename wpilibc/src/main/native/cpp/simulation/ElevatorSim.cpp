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
                         wpi::units::meter_t minHeight,
                         wpi::units::meter_t maxHeight, bool simulateGravity,
                         wpi::units::meter_t startingHeight,
                         const std::array<double, 2>& measurementStdDevs)
    : LinearSystemSim(plant, measurementStdDevs),
      m_gearbox(gearbox),
      m_minHeight(minHeight),
      m_maxHeight(maxHeight),
      m_simulateGravity(simulateGravity) {
  SetState(startingHeight, 0_mps);
}

ElevatorSim::ElevatorSim(const wpi::math::DCMotor& gearbox, double gearing,
                         wpi::units::kilogram_t carriageMass,
                         wpi::units::meter_t drumRadius,
                         wpi::units::meter_t minHeight,
                         wpi::units::meter_t maxHeight, bool simulateGravity,
                         wpi::units::meter_t startingHeight,
                         const std::array<double, 2>& measurementStdDevs)
    : ElevatorSim(wpi::math::Models::ElevatorFromPhysicalConstants(
                      gearbox, carriageMass, drumRadius, gearing),
                  gearbox, minHeight, maxHeight, simulateGravity,
                  startingHeight, measurementStdDevs) {}

template <typename Distance>
  requires std::same_as<wpi::units::meter, Distance> ||
           std::same_as<wpi::units::radian, Distance>
ElevatorSim::ElevatorSim(decltype(1_V / Velocity_t<Distance>(1)) kV,
                         decltype(1_V / Acceleration_t<Distance>(1)) kA,
                         const wpi::math::DCMotor& gearbox,
                         wpi::units::meter_t minHeight,
                         wpi::units::meter_t maxHeight, bool simulateGravity,
                         wpi::units::meter_t startingHeight,
                         const std::array<double, 2>& measurementStdDevs)
    : ElevatorSim(wpi::math::Models::ElevatorFromSysId(kV, kA), gearbox,
                  minHeight, maxHeight, simulateGravity, startingHeight,
                  measurementStdDevs) {}

void ElevatorSim::SetState(wpi::units::meter_t position,
                           wpi::units::meters_per_second_t velocity) {
  SetState(wpi::math::Vectord<2>{std::clamp(position, m_minHeight, m_maxHeight),
                                 velocity});
}

bool ElevatorSim::WouldHitLowerLimit(wpi::units::meter_t elevatorHeight) const {
  return elevatorHeight <= m_minHeight;
}

bool ElevatorSim::WouldHitUpperLimit(wpi::units::meter_t elevatorHeight) const {
  return elevatorHeight >= m_maxHeight;
}

bool ElevatorSim::HasHitLowerLimit() const {
  return WouldHitLowerLimit(wpi::units::meter_t{m_y(0)});
}

bool ElevatorSim::HasHitUpperLimit() const {
  return WouldHitUpperLimit(wpi::units::meter_t{m_y(0)});
}

wpi::units::meter_t ElevatorSim::GetPosition() const {
  return wpi::units::meter_t{m_y(0)};
}

wpi::units::meters_per_second_t ElevatorSim::GetVelocity() const {
  return wpi::units::meters_per_second_t{m_x(1)};
}

wpi::units::ampere_t ElevatorSim::GetCurrentDraw() const {
  // I = V / R - omega / (Kv * R)
  // Reductions are greater than 1, so a reduction of 10:1 would mean the motor
  // is spinning 10x faster than the output.

  double kA = 1.0 / m_plant.B(1, 0);
  using Kv_t = wpi::units::unit_t<wpi::units::compound_unit<
      wpi::units::volt, wpi::units::inverse<wpi::units::meters_per_second>>>;
  Kv_t Kv = Kv_t{-kA * m_plant.A(1, 1)};
  wpi::units::meters_per_second_t velocity{m_x(1)};
  wpi::units::radians_per_second_t motorVelocity = velocity * Kv * m_gearbox.Kv;

  // Perform calculation and return.
  return m_gearbox.Current(motorVelocity, wpi::units::volt_t{m_u(0)}) *
         wpi::util::sgn(m_u(0));
}

void ElevatorSim::SetInputVoltage(wpi::units::volt_t voltage) {
  SetInput(wpi::math::Vectord<1>{voltage.value()});
  ClampInput(wpi::RobotController::GetBatteryVoltage().value());
}

wpi::math::Vectord<2> ElevatorSim::UpdateX(
    const wpi::math::Vectord<2>& currentXhat, const wpi::math::Vectord<1>& u,
    wpi::units::second_t dt) {
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
  if (WouldHitLowerLimit(wpi::units::meter_t{updatedXhat(0)})) {
    return wpi::math::Vectord<2>{m_minHeight.value(), 0.0};
  }
  if (WouldHitUpperLimit(wpi::units::meter_t{updatedXhat(0)})) {
    return wpi::math::Vectord<2>{m_maxHeight.value(), 0.0};
  }
  return updatedXhat;
}
