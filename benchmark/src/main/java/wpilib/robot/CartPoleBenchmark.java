// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package wpilib.robot;

import static org.wpilib.math.autodiff.NumericalIntegration.rk4;
import static org.wpilib.math.autodiff.Variable.cos;
import static org.wpilib.math.autodiff.Variable.sin;
import static org.wpilib.math.autodiff.VariableMatrix.solve;
import static org.wpilib.math.optimization.Constraints.eq;
import static org.wpilib.math.optimization.Constraints.ge;
import static org.wpilib.math.optimization.Constraints.le;

import org.ejml.simple.SimpleMatrix;
import org.wpilib.math.autodiff.Variable;
import org.wpilib.math.autodiff.VariableMatrix;
import org.wpilib.math.optimization.Problem;
import org.wpilib.math.optimization.solver.Options;
import org.wpilib.math.util.MathUtil;

public final class CartPoleBenchmark {
  private CartPoleBenchmark() {
    // Utility class.
  }

  @SuppressWarnings("LocalVariableName")
  private static VariableMatrix cartPoleDynamics(VariableMatrix x, VariableMatrix u) {
    final double m_c = 5.0; // Cart mass (kg)
    final double m_p = 0.5; // Pole mass (kg)
    final double l = 0.5; // Pole length (m)
    final double g = 9.806; // Acceleration due to gravity (m/s²)

    var q = x.segment(0, 2);
    var qdot = x.segment(2, 2);
    var theta = q.get(1);
    var thetadot = qdot.get(1);

    //        [ m_c + m_p  m_p l cosθ]
    // M(q) = [m_p l cosθ    m_p l²  ]
    var M =
        new VariableMatrix(
            new Variable[][] {
              {new Variable(m_c + m_p), cos(theta).times(m_p * l)},
              {cos(theta).times(m_p * l), new Variable(m_p * Math.pow(l, 2))}
            });

    //           [0  −m_p lθ̇ sinθ]
    // C(q, q̇) = [0       0      ]
    var C =
        new VariableMatrix(
            new Variable[][] {
              {new Variable(0), thetadot.times(-m_p * l).times(sin(theta))},
              {new Variable(0), new Variable(0)}
            });

    //          [     0      ]
    // τ_g(q) = [-m_p gl sinθ]
    var tau_g =
        new VariableMatrix(new Variable[][] {{new Variable(0)}, {sin(theta).times(-m_p * g * l)}});

    //     [1]
    // B = [0]
    var B = new VariableMatrix(new double[][] {{1}, {0}});

    // q̈ = M⁻¹(q)(τ_g(q) − C(q, q̇)q̇ + Bu)
    var qddot = new VariableMatrix(4);
    qddot.segment(0, 2).set(qdot);
    qddot.segment(2, 2).set(solve(M, tau_g.minus(C.times(qdot)).plus(B.times(u))));
    return qddot;
  }

  /** Cart-pole benchmark. */
  public static void cartPole() {
    final double T = 5.0; // s
    final double dt = 0.05; // s
    final int N = (int) (T / dt);

    final double u_max = 20.0; // N
    final double d_max = 2.0; // m

    final var x_initial = new SimpleMatrix(new double[][] {{0.0}, {0.0}, {0.0}, {0.0}});
    final var x_final = new SimpleMatrix(new double[][] {{1.0}, {Math.PI}, {0.0}, {0.0}});

    var problem = new Problem();

    // x = [q, q̇]ᵀ = [x, θ, ẋ, θ̇]ᵀ
    var X = problem.decisionVariable(4, N + 1);

    // Initial guess
    for (int k = 0; k < N + 1; ++k) {
      X.get(0, k).setValue(MathUtil.lerp(x_initial.get(0), x_final.get(0), (double) k / N));
      X.get(1, k).setValue(MathUtil.lerp(x_initial.get(1), x_final.get(1), (double) k / N));
    }

    // u = f_x
    var U = problem.decisionVariable(1, N);

    // Initial conditions
    problem.subjectTo(eq(X.col(0), x_initial));

    // Final conditions
    problem.subjectTo(eq(X.col(N), x_final));

    // Cart position constraints
    problem.subjectTo(ge(X.row(0), 0.0));
    problem.subjectTo(le(X.row(0), d_max));

    // Input constraints
    problem.subjectTo(ge(U, -u_max));
    problem.subjectTo(le(U, u_max));

    // Dynamics constraints - RK4 integration
    for (int k = 0; k < N; ++k) {
      problem.subjectTo(
          eq(X.col(k + 1), rk4(CartPoleBenchmark::cartPoleDynamics, X.col(k), U.col(k), dt)));
    }

    // Minimize sum squared inputs
    var J = new Variable(0.0);
    for (int k = 0; k < N; ++k) {
      J = J.plus(U.col(k).T().times(U.col(k)).get(0));
    }
    problem.minimize(J);

    problem.solve(new Options().withDiagnostics(true));
  }
}
