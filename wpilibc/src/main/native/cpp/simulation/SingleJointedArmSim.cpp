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
    const LinearSystem<2, 1, 2>& system, const DCMotor& gearbox, units::meter_t armLength,
    units::meter_t pivotPoint, units::radian_t minAngle,
    units::radian_t maxAngle, bool simulateGravity,
    units::radian_t startingAngle,
    const std::array<double, 2>& measurementStdDevs)
    : AngularMechanismSim(system, gearbox, measurementStdDevs),
      m_armLen(armLength),
      m_pivotPoint(pivotPoint),
      m_minAngle(minAngle),
      m_maxAngle(maxAngle),
      m_gearbox(gearbox),
      m_simulateGravity(simulateGravity) {
  SetState(startingAngle, 0_rad_per_s);
}

void SingleJointedArmSim::SetState(units::radian_t angle,
                                   units::radians_per_second_t velocity) {
  AngularMechanismSim::SetState(Vectord<2>{std::clamp(angle, m_minAngle, m_maxAngle), velocity});
}

void SingleJointedArmSim::SetPosition(units::radian_t angle) {
  AngularMechanismSim::SetState(Vectord<2>{std::clamp(angle, m_minAngle, m_maxAngle), m_x(1, 0)});

}

bool SingleJointedArmSim::WouldHitLowerLimit(units::radian_t armAngle) const {
  return armAngle <= m_minAngle;
}

bool SingleJointedArmSim::WouldHitUpperLimit(units::radian_t armAngle) const {
  return armAngle >= m_maxAngle;
}

bool SingleJointedArmSim::HasHitLowerLimit() const {
  return WouldHitLowerLimit(units::radian_t{m_y(0)});
}

bool SingleJointedArmSim::HasHitUpperLimit() const {
  return WouldHitUpperLimit(units::radian_t{m_y(0)});
}

Vectord<2> SingleJointedArmSim::UpdateX(const Vectord<2>& currentXhat,
                                        const Vectord<1>& u,
                                        units::second_t dt) {
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

  Vectord<2> updatedXhat = RKDP(
      [&](const auto& x, const auto& u) -> Vectord<2> {
        Vectord<2> xdot = m_plant.A() * x + m_plant.B() * u;

        if (m_simulateGravity) {
          double alphaGravConstant = 
          -9.8 
          * (3.0 / 2.0)
          * std::abs(1 - 2 * m_pivotPoint.value() / m_armLen.value())
          / m_armLen.value();
          xdot += Vectord<2>{
              0.0, (alphaGravConstant * std::cos(x(0)))};
        }
        return xdot;
      },
      currentXhat, u, dt);

  // Check for collisions.
  if (WouldHitLowerLimit(units::radian_t{updatedXhat(0)})) {
    return Vectord<2>{m_minAngle.value(), 0.0};
  } else if (WouldHitUpperLimit(units::radian_t{updatedXhat(0)})) {
    return Vectord<2>{m_maxAngle.value(), 0.0};
  }
  return updatedXhat;
}
