// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization;

import static org.wpilib.math.autodiff.Variable.cos;
import static org.wpilib.math.autodiff.Variable.sin;
import static org.wpilib.math.autodiff.VariableMatrix.solve;

import org.ejml.simple.SimpleMatrix;
import org.wpilib.math.autodiff.Variable;
import org.wpilib.math.autodiff.VariableMatrix;

// https://underactuated.mit.edu/acrobot.html#cart_pole
//
// θ is CCW+ measured from negative y-axis.
//
// q = [x, θ]ᵀ
// q̇ = [ẋ, θ̇]ᵀ
// u = f_x
//
// M(q)q̈ + C(q, q̇)q̇ = τ_g(q) + Bu
// M(q)q̈ = τ_g(q) − C(q, q̇)q̇ + Bu
// q̈ = M⁻¹(q)(τ_g(q) − C(q, q̇)q̇ + Bu)
//
//        [ m_c + m_p  m_p l cosθ]
// M(q) = [m_p l cosθ    m_p l²  ]
//
//           [0  −m_p lθ̇ sinθ]
// C(q, q̇) = [0       0      ]
//
//          [     0      ]
// τ_g(q) = [-m_p gl sinθ]
//
//     [1]
// B = [0]

public final class CartPoleUtil {
  private CartPoleUtil() {
    // Utility class.
  }

  private static final double m_c = 5.0; // Cart mass (kg)
  private static final double m_p = 0.5; // Pole mass (kg)
  private static final double l = 0.5; // Pole length (m)
  private static final double g = 9.806; // Acceleration due to gravity (m/s²)

  public static SimpleMatrix cartPoleDynamics(SimpleMatrix x, SimpleMatrix u) {
    var q = x.extractMatrix(0, 2, 0, 1);
    var qdot = x.extractMatrix(2, 4, 0, 1);
    var theta = q.get(1, 0);
    var thetadot = qdot.get(1, 0);

    //        [ m_c + m_p  m_p l cosθ]
    // M(q) = [m_p l cosθ    m_p l²  ]
    var M =
        new SimpleMatrix(
            new double[][] {
              {m_c + m_p, m_p * l * Math.cos(theta)},
              {m_p * l * Math.cos(theta), m_p * Math.pow(l, 2)}
            });

    //           [0  −m_p lθ̇ sinθ]
    // C(q, q̇) = [0       0      ]
    var C = new SimpleMatrix(new double[][] {{0, -m_p * l * thetadot * Math.sin(theta)}, {0, 0}});

    //          [     0      ]
    // τ_g(q) = [-m_p gl sinθ]
    var tau_g = new SimpleMatrix(new double[][] {{0}, {-m_p * g * l * Math.sin(theta)}});

    //     [1]
    // B = [0]
    final var B = new SimpleMatrix(new double[][] {{1}, {0}});

    // q̈ = M⁻¹(q)(τ_g(q) − C(q, q̇)q̇ + Bu)
    var qddot = new SimpleMatrix(4, 1);
    qddot.insertIntoThis(0, 0, qdot);
    qddot.insertIntoThis(2, 0, M.solve(tau_g.minus(C.mult(qdot)).plus(B.mult(u))));
    return qddot;
  }

  public static VariableMatrix cartPoleDynamics(VariableMatrix x, VariableMatrix u) {
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
}
