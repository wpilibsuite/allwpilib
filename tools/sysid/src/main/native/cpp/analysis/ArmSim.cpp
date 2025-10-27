// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "sysid/analysis/ArmSim.h"

#include <cmath>

#include <frc/StateSpaceUtil.h>
#include <frc/system/NumericalIntegration.h>
#include <wpi/MathExtras.h>

using namespace sysid;

ArmSim::ArmSim(double Ks, double Kv, double Ka, double Kg, double offset,
               double initialPosition, double initialVelocity)
    // u = Ks sgn(x) + Kv x + Ka a + Kg cos(theta)
    // Ka a = u - Ks sgn(x) - Kv x - Kg cos(theta)
    // a = 1/Ka u - Ks/Ka sgn(x) - Kv/Ka x - Kg/Ka cos(theta)
    // a = -Kv/Ka x + 1/Ka u - Ks/Ka sgn(x) - Kg/Ka cos(theta)
    // a = Ax + Bu + c sgn(x) + d cos(theta)
    : m_A{-Kv / Ka},
      m_B{1.0 / Ka},
      m_c{-Ks / Ka},
      m_d{-Kg / Ka},
      m_offset{offset} {
  Reset(initialPosition, initialVelocity);
}

void ArmSim::Update(units::volt_t voltage, units::second_t dt) {
  // Returns arm acceleration under gravity
  auto f = [=, this](
               const Eigen::Vector<double, 2>& x,
               const Eigen::Vector<double, 1>& u) -> Eigen::Vector<double, 2> {
    return Eigen::Vector<double, 2>{
        x(1), (m_A * x.block<1, 1>(1, 0) + m_B * u + m_c * wpi::sgn(x(1)) +
               m_d * std::cos(x(0) + m_offset))(0)};
  };

  // Max error is large because an accurate sim isn't as important as the sim
  // finishing in a timely manner. Otherwise, the timestep can become absurdly
  // small for ill-conditioned data (e.g., high velocities with sharp spikes in
  // acceleration).
  Eigen::Vector<double, 1> u{voltage.value()};
  m_x = frc::RKDP(f, m_x, u, dt, 0.25);
}

double ArmSim::GetPosition() const {
  return m_x(0);
}

double ArmSim::GetVelocity() const {
  return m_x(1);
}

double ArmSim::GetAcceleration(units::volt_t voltage) const {
  Eigen::Vector<double, 1> u{voltage.value()};
  return (m_A * m_x.block<1, 1>(1, 0) + m_B * u +
          m_c * wpi::sgn(GetVelocity()) + m_d * std::cos(m_x(0) + m_offset))(0);
}

void ArmSim::Reset(double position, double velocity) {
  m_x = Eigen::Vector<double, 2>{position, velocity};
}
