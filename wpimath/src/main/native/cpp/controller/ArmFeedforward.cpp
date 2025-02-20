// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/ArmFeedforward.h"

#include <algorithm>
#include <limits>

#include <sleipnir/autodiff/Gradient.hpp>
#include <sleipnir/autodiff/Hessian.hpp>

#include "frc/EigenCore.h"
#include "frc/system/NumericalIntegration.h"

using namespace frc;

units::volt_t ArmFeedforward::Calculate(
    units::unit_t<Angle> currentAngle, units::unit_t<Velocity> currentVelocity,
    units::unit_t<Velocity> nextVelocity) const {
  using VarMat = sleipnir::VariableMatrix;

  // Small kₐ values make the solver ill-conditioned
  if (kA < units::unit_t<ka_unit>{1e-1}) {
    auto acceleration = (nextVelocity - currentVelocity) / m_dt;
    return kS * wpi::sgn(currentVelocity.value()) + kV * currentVelocity +
           kA * acceleration + kG * units::math::cos(currentAngle);
  }

  // Arm dynamics
  Matrixd<2, 2> A{{0.0, 1.0}, {0.0, -kV.value() / kA.value()}};
  Matrixd<2, 1> B{{0.0}, {1.0 / kA.value()}};
  const auto& f = [&](const VarMat& x, const VarMat& u) -> VarMat {
    VarMat c{{0.0},
             {-(kS / kA).value() * sleipnir::sign(x(1)) -
              (kG / kA).value() * sleipnir::cos(x(0))}};
    return A * x + B * u + c;
  };

  Vectord<2> r_k{currentAngle.value(), currentVelocity.value()};

  sleipnir::Variable u_k;

  // Initial guess
  auto acceleration = (nextVelocity - currentVelocity) / m_dt;
  u_k.SetValue((kS * wpi::sgn(currentVelocity.value()) + kV * currentVelocity +
                kA * acceleration + kG * units::math::cos(currentAngle))
                   .value());

  auto r_k1 = RK4<decltype(f), VarMat, VarMat>(f, r_k, u_k, m_dt);

  // Minimize difference between desired and actual next velocity
  auto cost =
      (nextVelocity.value() - r_k1(1)) * (nextVelocity.value() - r_k1(1));

  // Refine solution via Newton's method
  {
    auto xAD = u_k;
    double x = xAD.Value();

    sleipnir::Gradient gradientF{cost, xAD};
    Eigen::SparseVector<double> g = gradientF.Value();

    sleipnir::Hessian hessianF{cost, xAD};
    Eigen::SparseMatrix<double> H = hessianF.Value();

    double error = std::numeric_limits<double>::infinity();
    while (error > 1e-8) {
      // Iterate via Newton's method.
      //
      //   xₖ₊₁ = xₖ − H⁻¹g
      //
      // The Hessian is regularized to at least 1e-4.
      double p_x = -g.coeff(0) / std::max(H.coeff(0, 0), 1e-4);

      // Shrink step until cost goes down
      {
        double oldCost = cost.Value();

        double α = 1.0;
        double trial_x = x + α * p_x;

        xAD.SetValue(trial_x);

        while (cost.Value() > oldCost) {
          α *= 0.5;
          trial_x = x + α * p_x;

          xAD.SetValue(trial_x);
        }

        x = trial_x;
      }

      xAD.SetValue(x);

      g = gradientF.Value();
      H = hessianF.Value();

      error = std::abs(g.coeff(0));
    }
  }

  return units::volt_t{u_k.Value()};
}
