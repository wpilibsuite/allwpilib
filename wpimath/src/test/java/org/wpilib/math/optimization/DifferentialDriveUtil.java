// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization;

import static org.wpilib.math.autodiff.Variable.cos;
import static org.wpilib.math.autodiff.Variable.sin;

import org.ejml.simple.SimpleMatrix;
import org.wpilib.math.autodiff.VariableMatrix;

// x = [x, y, heading, left velocity, right velocity]ᵀ
// u = [left voltage, right voltage]ᵀ

public final class DifferentialDriveUtil {
  private DifferentialDriveUtil() {
    // Utility class.
  }

  private static final double trackwidth = 0.699; // m
  private static final double Kv_linear = 3.02; // V/(m/s)
  private static final double Ka_linear = 0.642; // V/(m/s²)
  private static final double Kv_angular = 1.382; // V/(m/s)
  private static final double Ka_angular = 0.08495; // V/(m/s²)

  private static final double A1 = -(Kv_linear / Ka_linear + Kv_angular / Ka_angular) / 2.0;
  private static final double A2 = -(Kv_linear / Ka_linear - Kv_angular / Ka_angular) / 2.0;
  private static final double B1 = 0.5 / Ka_linear + 0.5 / Ka_angular;
  private static final double B2 = 0.5 / Ka_linear - 0.5 / Ka_angular;
  private static final SimpleMatrix A = new SimpleMatrix(new double[][] {{A1, A2}, {A2, A1}});
  private static final SimpleMatrix B = new SimpleMatrix(new double[][] {{B1, B2}, {B2, B1}});

  public static SimpleMatrix differentialDriveDynamics(SimpleMatrix x, SimpleMatrix u) {
    var xdot = new SimpleMatrix(5, 1);

    var v = (x.get(3, 0) + x.get(4, 0)) / 2.0;
    xdot.set(0, 0, v * Math.cos(x.get(2, 0)));
    xdot.set(1, 0, v * Math.sin(x.get(2, 0)));
    xdot.set(2, 0, (x.get(4, 0) - x.get(3, 0)) / trackwidth);
    xdot.insertIntoThis(3, 0, A.mult(x.extractMatrix(3, 5, 0, 1)).plus(B.mult(u)));

    return xdot;
  }

  public static VariableMatrix differentialDriveDynamics(VariableMatrix x, VariableMatrix u) {
    var xdot = new VariableMatrix(5);

    var v = x.get(3).plus(x.get(4)).div(2.0);
    xdot.set(0, 0, v.times(cos(x.get(2))));
    xdot.set(1, 0, v.times(sin(x.get(2))));
    xdot.set(2, 0, x.get(4).minus(x.get(3)).div(trackwidth));
    xdot.segment(3, 2)
        .set(new VariableMatrix(A).times(x.segment(3, 2)).plus(new VariableMatrix(B).times(u)));

    return xdot;
  }
}
