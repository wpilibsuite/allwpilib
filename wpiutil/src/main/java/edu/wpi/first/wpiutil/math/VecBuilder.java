/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpiutil.math;

import edu.wpi.first.wpiutil.math.numbers.N1;

/**
 * A specialization of {@link MatBuilder} for constructing vectors (Nx1 matrices).
 *
 * @param <N> The dimension of the vector to be constructed.
 */
public class VecBuilder<N extends Num> extends MatBuilder<N, N1> {
  public VecBuilder(Nat<N> rows) {
    super(rows, Nat.N1());
  }
}
