// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/ElevatorSim.hpp"

#include <algorithm>
#include <stdexcept>

#include "wpi/system/RobotController.hpp"

using namespace wpi;
using namespace wpi::sim;

ElevatorSim::ElevatorSim(const wpi::math::LinearSystem<2, 1, 2>& plant,
                         const wpi::math::DCMotor& gearbox, double gearing,
                         wpi::units::volt_t kG, wpi::units::meter_t minHeight,
                         wpi::units::meter_t maxHeight,
                         wpi::units::meter_t startingHeight,
                         const std::array<double, 2>& measurementStdDevs)
    : LinearSystemSim(plant, measurementStdDevs),
      m_gearbox(gearbox),
      m_gearing(gearing),
      m_kG(kG),
      // A₁,₁ = -G²Kₜ/(KᵥRr²m), B₁,₀ = GKₜ/(Rrm). G and r appear only as the
      // ratio G/r, so the plant alone can't identify them separately; G is
      // taken from the caller.
      //
      //   A/B = -G/(Kᵥr)  →  r/G = -B/KᵥA  →  r = -GB/KᵥA
      //   B = GKₜ/(Rrm)   →  m = GKₜ/(RrB)
      m_drumRadius(wpi::units::meter_t{-gearing * plant.B(1, 0) /
                                       (gearbox.Kv.value() * plant.A(1, 1))}),
      m_carriageMass(wpi::units::kilogram_t{
          gearing * gearbox.Kt.value() /
          (gearbox.R.value() * m_drumRadius.value() * plant.B(1, 0))}),
      m_minHeight(minHeight),
      m_maxHeight(maxHeight) {
  if (gearing <= 0.0) {
    throw std::domain_error("gearing must be greater than zero.");
  }
  if (plant.A(1, 1) == 0.0) {
    throw std::domain_error(
        "plant must have nonzero velocity damping A(1, 1); a plant built with "
        "kV = 0 doesn't determine the drum radius.");
  }
  if (plant.B(1, 0) == 0.0) {
    throw std::domain_error("plant must have nonzero input gain B(1, 0).");
  }
  if (minHeight > maxHeight) {
    throw std::domain_error("minHeight must not be greater than maxHeight.");
  }
  SetState(startingHeight, 0_mps);
}

void ElevatorSim::SetState(wpi::units::meter_t position,
                           wpi::units::meters_per_second_t velocity) {
  SetState(wpi::math::Vectord<2>{std::clamp(position, m_minHeight, m_maxHeight),
                                 velocity});
}

void ElevatorSim::SetPosition(wpi::units::meter_t position) {
  SetState(position, wpi::units::meters_per_second_t{m_x(1)});
}

void ElevatorSim::SetVelocity(wpi::units::meters_per_second_t velocity) {
  SetState(wpi::units::meter_t{m_x(0)}, velocity);
}

bool ElevatorSim::WouldHitLowerLimit(wpi::units::meter_t elevatorHeight) const {
  return elevatorHeight <= m_minHeight;
}

bool ElevatorSim::WouldHitUpperLimit(wpi::units::meter_t elevatorHeight) const {
  return elevatorHeight >= m_maxHeight;
}

bool ElevatorSim::HasHitLowerLimit() const {
  return WouldHitLowerLimit(GetPosition());
}

bool ElevatorSim::HasHitUpperLimit() const {
  return WouldHitUpperLimit(GetPosition());
}

const wpi::math::DCMotor& ElevatorSim::GetGearbox() const {
  return m_gearbox;
}

double ElevatorSim::GetGearing() const {
  return m_gearing;
}

wpi::units::meter_t ElevatorSim::GetDrumRadius() const {
  return m_drumRadius;
}

wpi::units::kilogram_t ElevatorSim::GetCarriageMass() const {
  return m_carriageMass;
}

wpi::units::volt_t ElevatorSim::GetKg() const {
  return m_kG;
}

wpi::units::meter_t ElevatorSim::GetPosition() const {
  return wpi::units::meter_t{GetOutput(0)};
}

wpi::units::meters_per_second_t ElevatorSim::GetVelocity() const {
  return wpi::units::meters_per_second_t{GetOutput(1)};
}

wpi::units::meters_per_second_squared_t ElevatorSim::GetAcceleration() const {
  wpi::units::meters_per_second_squared_t acceleration{
      (m_plant.A() * m_x +
       m_plant.B() * (m_u - wpi::math::Vectord<1>{m_kG.value()}))(1, 0)};

  // UpdateX() pins the state at a hard stop with zero velocity, so acceleration
  // pointing farther into a stop the carriage is already resting against is
  // motion the sim prevents. The stop absorbs it.
  wpi::units::meter_t position{m_x(0)};
  if (WouldHitLowerLimit(position) && acceleration < 0_mps_sq) {
    return 0_mps_sq;
  }
  if (WouldHitUpperLimit(position) && acceleration > 0_mps_sq) {
    return 0_mps_sq;
  }

  return acceleration;
}

wpi::units::newton_t ElevatorSim::GetForce() const {
  return wpi::units::newton_t{GetAcceleration().value() *
                              m_carriageMass.value()};
}

wpi::units::ampere_t ElevatorSim::GetCurrentDraw() const {
  // Reductions are greater than 1, so a reduction of 10:1 would mean the motor
  // is spinning 10x faster than the output. v = rω, so ω = v/r.
  //
  // The current through the motor is I = V/R - ω/(KᵥR), where V is the voltage
  // across the motor terminals.
  //
  // The motor controller produces V by PWMing the battery voltage at duty cycle
  // D = V/V_batt. An ideal H-bridge conserves power, so V_batt·I_supply = V·I,
  // giving
  //
  //   I_supply = D·I
  //
  // Scaling by D also makes the result continuous through V = 0, where the
  // motor is braking and its circulating current isn't drawn from the battery.
  wpi::units::radians_per_second_t motorVelocity{m_x(1) * m_gearing /
                                                 m_drumRadius.value()};
  wpi::units::volt_t appliedVoltage{m_u(0)};
  wpi::units::volt_t batteryVoltage = wpi::RobotController::GetBatteryVoltage();

  // With no battery voltage the controller can't apply any duty cycle, so
  // nothing is drawn.
  if (batteryVoltage == 0_V) {
    return 0_A;
  }

  double ratio = appliedVoltage.value() / batteryVoltage.value();
  double dutyCycle = std::clamp(ratio, -1.0, 1.0);
  return m_gearbox.Current(motorVelocity, appliedVoltage) * dutyCycle;
}

wpi::units::volt_t ElevatorSim::GetInputVoltage() const {
  return wpi::units::volt_t{GetInput(0)};
}

void ElevatorSim::SetInputVoltage(wpi::units::volt_t voltage) {
  SetInput(wpi::math::Vectord<1>{voltage.value()});
  ClampInput(wpi::RobotController::GetBatteryVoltage().value());
}

wpi::math::Vectord<2> ElevatorSim::UpdateX(
    const wpi::math::Vectord<2>& currentXhat, const wpi::math::Vectord<1>& u,
    wpi::units::second_t dt) {
  // Gravity enters as a constant voltage offset. Since B = [0  B₁,₀]ᵀ,
  //
  //   ẋ = Ax + Bu + [0  -kG·B₁,₀]ᵀ = Ax + B(u - kG)
  //
  // so the plant's exact discretization applies with the input offset by -kG.
  auto updatedXhat = m_plant.CalculateX(
      currentXhat, u - wpi::math::Vectord<1>{m_kG.value()}, dt);

  // Check for collision after updating x-hat.
  if (WouldHitLowerLimit(wpi::units::meter_t{updatedXhat(0)})) {
    return wpi::math::Vectord<2>{m_minHeight.value(), 0.0};
  }
  if (WouldHitUpperLimit(wpi::units::meter_t{updatedXhat(0)})) {
    return wpi::math::Vectord<2>{m_maxHeight.value(), 0.0};
  }
  return updatedXhat;
}
