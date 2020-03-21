/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpiutil.math;

import org.ejml.simple.SimpleMatrix;

import edu.wpi.first.wpiutil.math.numbers.N1;

public class Vector<R extends Num> extends Matrix<R, N1> {

  public Vector(SimpleMatrix storage) {
    super(storage);
  }

  public Vector(Nat<R> size, double... data) {
    super(new SimpleMatrix(size.getNum(), 1, true, data));
  }

  public static Vector<N1> d1(double data) {
    return new Vector<>(Nat.N1(), data);
  }

}
