// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N3;
import edu.wpi.first.math.proto.VectorProto;
import edu.wpi.first.math.struct.VectorStruct;
import edu.wpi.first.util.protobuf.Protobuf;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.Struct;
import edu.wpi.first.util.struct.StructSerializable;
import java.util.Objects;
import org.ejml.simple.SimpleMatrix;

/**
 * A shape-safe wrapper over Efficient Java Matrix Library (EJML) matrices.
 *
 * <p>This class is intended to be used alongside the state space library.
 *
 * @param <R> The number of rows in this matrix.
 */
public class Vector<R extends Num> extends Matrix<R, N1>
    implements ProtobufSerializable, StructSerializable {
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

  /**
   * Returns an element of the vector at a specified row.
   *
   * @param row The row that the element is located at.
   * @return An element of the vector.
   */
  public double get(int row) {
    return get(row, 0);
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
   * Adds the given vector to this vector.
   *
   * @param value The vector to add.
   * @return The resultant vector.
   */
  public final Vector<R> plus(Vector<R> value) {
    return new Vector<>(this.m_storage.plus(Objects.requireNonNull(value).m_storage));
  }

  /**
   * Subtracts the given vector to this vector.
   *
   * @param value The vector to add.
   * @return The resultant vector.
   */
  public final Vector<R> minus(Vector<R> value) {
    return new Vector<>(this.m_storage.minus(Objects.requireNonNull(value).m_storage));
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
    return Math.sqrt(dot(this));
  }

  /**
   * Returns the unit vector parallel with this vector.
   *
   * @return The unit vector.
   */
  public Vector<R> unit() {
    return div(norm());
  }

  /**
   * Returns the projection of this vector along another.
   *
   * @param other The vector to project along.
   * @return The projection.
   */
  public Vector<R> projection(Vector<R> other) {
    return other.times(dot(other)).div(other.dot(other));
  }

  /**
   * Returns the cross product of 3 dimensional vectors a and b.
   *
   * @param a The vector to cross with b.
   * @param b The vector to cross with a.
   * @return The cross product of a and b.
   */
  public static Vector<N3> cross(Vector<N3> a, Vector<N3> b) {
    return VecBuilder.fill(
        a.get(1) * b.get(2) - a.get(2) * b.get(1),
        a.get(2) * b.get(0) - a.get(0) * b.get(2),
        a.get(0) * b.get(1) - a.get(1) * b.get(0));
  }

  /**
   * Creates an implementation of the {@link Protobuf} interface for vectors.
   *
   * @param <R> The number of rows of the vectors this serializer processes.
   * @param rows The number of rows of the vectors this serializer processes.
   * @return The protobuf implementation.
   */
  public static final <R extends Num> VectorProto<R> getProto(Nat<R> rows) {
    return new VectorProto<>(rows);
  }

  /**
   * Creates an implementation of the {@link Struct} interface for vectors.
   *
   * @param <R> The number of rows of the vectors this serializer processes.
   * @param rows The number of rows of the vectors this serializer processes.
   * @return The struct implementation.
   */
  public static final <R extends Num> VectorStruct<R> getStruct(Nat<R> rows) {
    return new VectorStruct<>(rows);
  }
}
