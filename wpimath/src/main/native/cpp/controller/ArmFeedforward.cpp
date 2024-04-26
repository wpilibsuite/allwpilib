// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/ArmFeedforward.h"

using namespace frc;

units::volt_t ArmFeedforward::Calculate(units::unit_t<Angle> currentAngle,
                                        units::unit_t<Velocity> currentVelocity,
                                        units::unit_t<Velocity> nextVelocity,
                                        units::second_t dt) const {
  using VarMat = sleipnir::VariableMatrix;

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

  sleipnir::OptimizationProblem problem;
  auto u_k = problem.DecisionVariable();

  // Initial guess
  auto acceleration = (nextVelocity - currentVelocity) / dt;
  u_k.SetValue((kS * wpi::sgn(currentVelocity.value()) + kV * currentVelocity +
                kA * acceleration + kG * units::math::cos(currentAngle))
                   .value());

  auto r_k1 = RK4<decltype(f), VarMat, VarMat>(f, r_k, u_k, dt);
  problem.Minimize((nextVelocity.value() - r_k1(1)) *
                   (nextVelocity.value() - r_k1(1)));
  problem.Solve({.diagnostics = true});

  return units::volt_t{u_k.Value()};
}
