/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.system;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.VecBuilder;
import edu.wpi.first.wpiutil.math.numbers.N1;

import static org.junit.jupiter.api.Assertions.assertEquals;

public class RungeKuttaTest {
  @Test
  @SuppressWarnings({"ParameterName", "LocalVariableName"})
  public void testExponential() {

    Matrix<N1, N1> y0 = VecBuilder.fill(0.0);

    //noinspection SuspiciousNameCombination
    var y1 = RungeKutta.rungeKutta((Matrix<N1, N1> x) -> {
      var y = new Matrix<>(Nat.N1(), Nat.N1());
      y.set(0, 0, Math.exp(x.get(0, 0)));
      return y; },
            y0, 0.1
    );

    assertEquals(Math.exp(0.1) - Math.exp(0.0), y1.get(0, 0), 1e-3);
  }
}
