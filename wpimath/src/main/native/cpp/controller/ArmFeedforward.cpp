// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/ArmFeedforward.hpp"

#include <algorithm>
#include <limits>

#include <sleipnir/autodiff/gradient.hpp>
#include <sleipnir/autodiff/hessian.hpp>

#include "wpi/math/linalg/EigenCore.hpp"
#include "wpi/math/system/NumericalIntegration.hpp"

using namespace wpi::math;

wpi::units::volt_t ArmFeedforward::Calculate(
    wpi::units::unit_t<Angle> currentAngle,
    wpi::units::unit_t<Velocity> currentVelocity,
    wpi::units::unit_t<Velocity> nextVelocity) const {
  using VarMat = slp::VariableMatrix<double>;

  // Small kₐ values make the solver ill-conditioned
  if (kA < wpi::units::unit_t<ka_unit>{1e-1}) {
    auto acceleration = (nextVelocity - currentVelocity) / m_dt;
    return kS * wpi::util::sgn(currentVelocity.value()) + kV * currentVelocity +
           kA * acceleration + kG * wpi::units::math::cos(currentAngle);
  }

  // Arm dynamics
  Matrixd<2, 2> A{{0.0, 1.0}, {0.0, -kV.value() / kA.value()}};
  Matrixd<2, 1> B{{0.0}, {1.0 / kA.value()}};
  const auto& f = [&](const VarMat& x, const VarMat& u) -> VarMat {
    VarMat c{{0.0},
             {-(kS / kA).value() * slp::sign(x[1]) -
              (kG / kA).value() * slp::cos(x[0])}};
    return A * x + B * u + c;
  };

  Vectord<2> r_k{currentAngle.value(), currentVelocity.value()};

  slp::Variable<double> u_k;

  // Initial guess
  auto acceleration = (nextVelocity - currentVelocity) / m_dt;
  u_k.set_value((kS * wpi::util::sgn(currentVelocity.value()) +
                 kV * currentVelocity + kA * acceleration +
                 kG * wpi::units::math::cos(currentAngle))
                    .value());

  auto r_k1 = RK4<decltype(f), VarMat, VarMat>(f, r_k, u_k, m_dt);

  // Minimize difference between desired and actual next velocity
  auto cost =
      (nextVelocity.value() - r_k1[1]) * (nextVelocity.value() - r_k1[1]);

  // Refine solution via Newton's method
  {
    auto xAD = u_k;
    double x = xAD.value();

    slp::Gradient gradientF{cost, xAD};
    Eigen::SparseVector<double> g = gradientF.value();

    slp::Hessian hessianF{cost, xAD};
    Eigen::SparseMatrix<double> H = hessianF.value();

    double error_k = std::numeric_limits<double>::infinity();
    double error_k1 = std::abs(g.coeff(0));

    // Loop until error stops decreasing or max iterations is reached
    for (size_t iteration = 0;
         iteration < 50 && error_k1 < (1.0 - 1e-10) * error_k; ++iteration) {
      error_k = error_k1;

      // Iterate via Newton's method.
      //
      //   xₖ₊₁ = xₖ − H⁻¹g
      //
      // The Hessian is regularized to at least 1e-4.
      double p_x = -g.coeff(0) / std::max(H.coeff(0, 0), 1e-4);

      // Shrink step until cost goes down
      {
        double oldCost = cost.value();

        double α = 1.0;
        double trial_x = x + α * p_x;

        xAD.set_value(trial_x);

        while (cost.value() > oldCost) {
          α *= 0.5;
          trial_x = x + α * p_x;

          xAD.set_value(trial_x);
        }

        x = trial_x;
      }

      xAD.set_value(x);

      g = gradientF.value();
      H = hessianF.value();

      error_k1 = std::abs(g.coeff(0));
    }
  }

  return wpi::units::volt_t{u_k.value()};
}
