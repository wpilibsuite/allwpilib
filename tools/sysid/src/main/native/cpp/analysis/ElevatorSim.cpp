// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/sysid/analysis/ElevatorSim.hpp"

#include "wpi/math/system/Discretization.hpp"
#include "wpi/math/util/StateSpaceUtil.hpp"
#include "wpi/util/MathExtras.hpp"

using namespace sysid;

ElevatorSim::ElevatorSim(double Ks, double Kv, double Ka, double Kg,
                         double initialPosition, double initialVelocity)
    // dx/dt = Ax + Bu + c sgn(x) + d
    : m_A{{0.0, 1.0}, {0.0, -Kv / Ka}},
      m_B{0.0, 1.0 / Ka},
      m_c{0.0, -Ks / Ka},
      m_d{0.0, -Kg / Ka} {
  Reset(initialPosition, initialVelocity);
}

void ElevatorSim::Update(wpi::units::volt_t voltage, wpi::units::second_t dt) {
  Eigen::Vector<double, 1> u{voltage.value()};

  // Given dx/dt = Ax + Bu + c sgn(x) + d,
  // x_k+1 = e^(AT) x_k + A^-1 (e^(AT) - 1) (Bu + c sgn(x) + d)
  Eigen::Matrix<double, 2, 2> Ad;
  Eigen::Matrix<double, 2, 1> Bd;
  wpi::math::DiscretizeAB<2, 1>(m_A, m_B, dt, &Ad, &Bd);
  m_x = Ad * m_x + Bd * u +
        Bd * m_B.householderQr().solve(m_c * wpi::util::sgn(GetVelocity()) + m_d);
}

double ElevatorSim::GetPosition() const {
  return m_x(0);
}

double ElevatorSim::GetVelocity() const {
  return m_x(1);
}

double ElevatorSim::GetAcceleration(wpi::units::volt_t voltage) const {
  Eigen::Vector<double, 1> u{voltage.value()};
  return (m_A * m_x + m_B * u + m_c * wpi::util::sgn(GetVelocity()) + m_d)(1);
}

void ElevatorSim::Reset(double position, double velocity) {
  m_x = Eigen::Vector<double, 2>{position, velocity};
}
