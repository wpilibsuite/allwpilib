// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/SingleJointedArmSim.hpp"

#include <cmath>

#include "wpi/math/system/Models.hpp"
#include "wpi/math/system/NumericalIntegration.hpp"
#include "wpi/system/RobotController.hpp"
#include "wpi/units/voltage.hpp"
#include "wpi/util/MathExtras.hpp"

using namespace wpi;
using namespace wpi::sim;

SingleJointedArmSim::SingleJointedArmSim(
    const wpi::math::LinearSystem<2, 1, 2>& system,
    const wpi::math::DCMotor& gearbox, double gearing,
    wpi::units::meters<> armLength, wpi::units::radians<> minAngle,
    wpi::units::radians<> maxAngle, bool simulateGravity,
    wpi::units::radians<> startingAngle,
    const std::array<double, 2>& measurementStdDevs)
    : LinearSystemSim<2, 1, 2>(system, measurementStdDevs),
      m_armLen(armLength),
      m_minAngle(minAngle),
      m_maxAngle(maxAngle),
      m_gearbox(gearbox),
      m_gearing(gearing),
      m_simulateGravity(simulateGravity) {
  SetState(startingAngle, 0_rad_per_s);
}

SingleJointedArmSim::SingleJointedArmSim(
    const wpi::math::DCMotor& gearbox, double gearing,
    wpi::units::kilogram_square_meters<> moi, wpi::units::meters<> armLength,
    wpi::units::radians<> minAngle, wpi::units::radians<> maxAngle,
    bool simulateGravity, wpi::units::radians<> startingAngle,
    const std::array<double, 2>& measurementStdDevs)
    : SingleJointedArmSim(
          wpi::math::Models::SingleJointedArmFromPhysicalConstants(gearbox, moi,
                                                                   gearing),
          gearbox, gearing, armLength, minAngle, maxAngle, simulateGravity,
          startingAngle, measurementStdDevs) {}

void SingleJointedArmSim::SetState(wpi::units::radians<> angle,
                                   wpi::units::radians_per_second<> velocity) {
  SetState(wpi::math::Vectord<2>{std::clamp(angle, m_minAngle, m_maxAngle),
                                 velocity});
}

bool SingleJointedArmSim::WouldHitLowerLimit(
    wpi::units::radians<> armAngle) const {
  return armAngle <= m_minAngle;
}

bool SingleJointedArmSim::WouldHitUpperLimit(
    wpi::units::radians<> armAngle) const {
  return armAngle >= m_maxAngle;
}

bool SingleJointedArmSim::HasHitLowerLimit() const {
  return WouldHitLowerLimit(wpi::units::radians<>{m_y(0)});
}

bool SingleJointedArmSim::HasHitUpperLimit() const {
  return WouldHitUpperLimit(wpi::units::radians<>{m_y(0)});
}

wpi::units::radians<> SingleJointedArmSim::GetAngle() const {
  return wpi::units::radians<>{m_y(0)};
}

wpi::units::radians_per_second<> SingleJointedArmSim::GetVelocity() const {
  return wpi::units::radians_per_second<>{m_x(1)};
}

wpi::units::amperes<> SingleJointedArmSim::GetCurrentDraw() const {
  // Reductions are greater than 1, so a reduction of 10:1 would mean the motor
  // is spinning 10x faster than the output
  wpi::units::radians_per_second<> motorVelocity{m_x(1) * m_gearing};
  return m_gearbox.Current(motorVelocity, wpi::units::volts<>{m_u(0)}) *
         wpi::util::sgn(m_u(0));
}

void SingleJointedArmSim::SetInputVoltage(wpi::units::volts<> voltage) {
  SetInput(wpi::math::Vectord<1>{voltage.value()});
  ClampInput(wpi::RobotController::GetBatteryVoltage().value());
}

wpi::math::Vectord<2> SingleJointedArmSim::UpdateX(
    const wpi::math::Vectord<2>& currentXhat, const wpi::math::Vectord<1>& u,
    wpi::units::seconds<> dt) {
  // The torque on the arm is given by τ = F⋅r, where F is the force applied by
  // gravity and r the distance from pivot to center of mass. Recall from
  // dynamics that the sum of torques for a rigid body is τ = J⋅α, were τ is
  // torque on the arm, J is the mass-moment of inertia about the pivot axis,
  // and α is the angular acceleration in rad/s². Rearranging yields: α = F⋅r/J
  //
  // We substitute in F = m⋅g⋅cos(θ), where θ is the angle from horizontal:
  //
  //   α = (m⋅g⋅cos(θ))⋅r/J
  //
  // Multiply RHS by cos(θ) to account for the arm angle. Further, we know the
  // arm mass-moment of inertia J of our arm is given by J=1/3 mL², modeled as a
  // rod rotating about it's end, where L is the overall rod length. The mass
  // distribution is assumed to be uniform. Substitute r=L/2 to find:
  //
  //   α = (m⋅g⋅cos(θ))⋅r/(1/3 mL²)
  //   α = (m⋅g⋅cos(θ))⋅(L/2)/(1/3 mL²)
  //   α = 3/2⋅g⋅cos(θ)/L
  //
  // This acceleration is next added to the linear system dynamics ẋ=Ax+Bu
  //
  //   f(x, u) = Ax + Bu + [0  α]ᵀ
  //   f(x, u) = Ax + Bu + [0  3/2⋅g⋅cos(θ)/L]ᵀ

  wpi::math::Vectord<2> updatedXhat = wpi::math::RKDP(
      [&](const auto& x, const auto& u) -> wpi::math::Vectord<2> {
        wpi::math::Vectord<2> xdot = m_plant.A() * x + m_plant.B() * u;

        if (m_simulateGravity) {
          xdot += wpi::math::Vectord<2>{
              0.0, (3.0 / 2.0 * -9.8 / m_armLen * std::cos(x(0))).value()};
        }
        return xdot;
      },
      currentXhat, u, dt);

  // Check for collisions.
  if (WouldHitLowerLimit(wpi::units::radians<>{updatedXhat(0)})) {
    return wpi::math::Vectord<2>{m_minAngle.value(), 0.0};
  } else if (WouldHitUpperLimit(wpi::units::radians<>{updatedXhat(0)})) {
    return wpi::math::Vectord<2>{m_maxAngle.value(), 0.0};
  }
  return updatedXhat;
}
