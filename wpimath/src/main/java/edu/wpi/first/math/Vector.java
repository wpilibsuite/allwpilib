// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import edu.wpi.first.math.numbers.N1;
import org.ejml.simple.SimpleMatrix;

/**
 * A shape-safe wrapper over Efficient Java Matrix Library (EJML) matrices.
 *
 * <p>This class is intended to be used alongside the state space library.
 *
 * @param <R> The number of rows in this matrix.
 */
public class Vector<R extends Num> extends Matrix<R, N1> {
  /**
   * Constructs an empty zero vector of the given dimensions.
   *
   * @param rows The number of rows of the vector.
   */
  public Vector(Nat<R> rows) {
    super(rows, Nat.N1());
  }

  /**
   * Constructs a new {@link Vector} with the given storage. Caller should make sure that the
   * provided generic bounds match the shape of the provided {@link Vector}.
   *
   * <p>NOTE:It is not recommended to use this constructor unless the {@link SimpleMatrix} API is
   * absolutely necessary due to the desired function not being accessible through the {@link
   * Vector} wrapper.
   *
   * @param storage The {@link SimpleMatrix} to back this vector.
   */
  public Vector(SimpleMatrix storage) {
    super(storage);
  }

  /**
   * Constructs a new vector with the storage of the supplied matrix.
   *
   * @param other The {@link Vector} to copy the storage of.
   */
  public Vector(Matrix<R, N1> other) {
    super(other);
  }

  @Override
  public Vector<R> times(double value) {
    return new Vector<>(this.m_storage.scale(value));
  }

  @Override
  public Vector<R> div(int value) {
    return new Vector<>(this.m_storage.divide(value));
  }

  @Override
  public Vector<R> div(double value) {
    return new Vector<>(this.m_storage.divide(value));
  }

  /**
   * Returns the dot product of this vector with another.
   *
   * @param other The other vector.
   * @return The dot product.
   */
  public double dot(Vector<R> other) {
    double dot = 0.0;

    for (int i = 0; i < getNumRows(); ++i) {
      dot += get(i, 0) * other.get(i, 0);
    }

    return dot;
  }

  /**
   * Returns the norm of this vector.
   *
   * @return The norm.
   */
  public double norm() {
    double squaredNorm = 0.0;

    for (int i = 0; i < getNumRows(); ++i) {
      squaredNorm += get(i, 0) * get(i, 0);
    }

    return Math.sqrt(squaredNorm);
  }
}
