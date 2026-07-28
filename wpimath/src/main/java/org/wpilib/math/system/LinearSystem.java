// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.system;

import java.util.Arrays;
import org.ejml.simple.SimpleMatrix;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.system.proto.LinearSystemProto;
import org.wpilib.math.system.struct.LinearSystemStruct;
import org.wpilib.math.util.Nat;
import org.wpilib.math.util.Num;
import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.Struct;
import org.wpilib.util.struct.StructSerializable;

/**
 * A plant defined using state-space notation.
 *
 * <p>A plant is a mathematical model of a system's dynamics.
 *
 * <p>For more on the underlying math, read
 * https://file.tavsys.net/control/controls-engineering-in-frc.pdf.
 *
 * @param <States> Number of states.
 * @param <Inputs> Number of inputs.
 * @param <Outputs> Number of outputs.
 */
public class LinearSystem<States extends Num, Inputs extends Num, Outputs extends Num>
    implements ProtobufSerializable, StructSerializable {
  /** Continuous system matrix. */
  private final Matrix<States, States> m_A;

  /** Continuous input matrix. */
  private final Matrix<States, Inputs> m_B;

  /** Output matrix. */
  private final Matrix<Outputs, States> m_C;

  /** Feedthrough matrix. */
  private final Matrix<Outputs, Inputs> m_D;

  /**
   * Construct a new LinearSystem from the four system matrices.
   *
   * @param A The system matrix A.
   * @param B The input matrix B.
   * @param C The output matrix C.
   * @param D The feedthrough matrix D.
   * @throws IllegalArgumentException if any matrix element isn't finite.
   */
  public LinearSystem(
      Matrix<States, States> A,
      Matrix<States, Inputs> B,
      Matrix<Outputs, States> C,
      Matrix<Outputs, Inputs> D) {
    for (int row = 0; row < A.getNumRows(); ++row) {
      for (int col = 0; col < A.getNumCols(); ++col) {
        if (!Double.isFinite(A.get(row, col))) {
          throw new IllegalArgumentException(
              "Elements of A aren't finite. This is usually due to model implementation errors.");
        }
      }
    }
    for (int row = 0; row < B.getNumRows(); ++row) {
      for (int col = 0; col < B.getNumCols(); ++col) {
        if (!Double.isFinite(B.get(row, col))) {
          throw new IllegalArgumentException(
              "Elements of B aren't finite. This is usually due to model implementation errors.");
        }
      }
    }
    for (int row = 0; row < C.getNumRows(); ++row) {
      for (int col = 0; col < C.getNumCols(); ++col) {
        if (!Double.isFinite(C.get(row, col))) {
          throw new IllegalArgumentException(
              "Elements of C aren't finite. This is usually due to model implementation errors.");
        }
      }
    }
    for (int row = 0; row < D.getNumRows(); ++row) {
      for (int col = 0; col < D.getNumCols(); ++col) {
        if (!Double.isFinite(D.get(row, col))) {
          throw new IllegalArgumentException(
              "Elements of D aren't finite. This is usually due to model implementation errors.");
        }
      }
    }

    this.m_A = A;
    this.m_B = B;
    this.m_C = C;
    this.m_D = D;
  }

  /**
   * Returns the system matrix A.
   *
   * @return the system matrix A.
   */
  public Matrix<States, States> getA() {
    return m_A;
  }

  /**
   * Returns an element of the system matrix A.
   *
   * @param row Row of A.
   * @param col Column of A.
   * @return the system matrix A at (i, j).
   */
  public double getA(int row, int col) {
    return m_A.get(row, col);
  }

  /**
   * Returns the input matrix B.
   *
   * @return the input matrix B.
   */
  public Matrix<States, Inputs> getB() {
    return m_B;
  }

  /**
   * Returns an element of the input matrix B.
   *
   * @param row Row of B.
   * @param col Column of B.
   * @return The value of the input matrix B at (i, j).
   */
  public double getB(int row, int col) {
    return m_B.get(row, col);
  }

  /**
   * Returns the output matrix C.
   *
   * @return Output matrix C.
   */
  public Matrix<Outputs, States> getC() {
    return m_C;
  }

  /**
   * Returns an element of the output matrix C.
   *
   * @param row Row of C.
   * @param col Column of C.
   * @return the double value of C at the given position.
   */
  public double getC(int row, int col) {
    return m_C.get(row, col);
  }

  /**
   * Returns the feedthrough matrix D.
   *
   * @return the feedthrough matrix D.
   */
  public Matrix<Outputs, Inputs> getD() {
    return m_D;
  }

  /**
   * Returns an element of the feedthrough matrix D.
   *
   * @param row Row of D.
   * @param col Column of D.
   * @return The feedthrough matrix D at (i, j).
   */
  public double getD(int row, int col) {
    return m_D.get(row, col);
  }

  /**
   * Computes the new x given the old x and the control input.
   *
   * <p>This is used by state observers directly to run updates based on state estimate.
   *
   * @param x The current state.
   * @param clampedU The control input.
   * @param dt Timestep for model update in seconds.
   * @return the updated x.
   */
  public Matrix<States, N1> calculateX(
      Matrix<States, N1> x, Matrix<Inputs, N1> clampedU, double dt) {
    var discABpair = Discretization.discretizeAB(m_A, m_B, dt);

    return discABpair.getFirst().times(x).plus(discABpair.getSecond().times(clampedU));
  }

  /**
   * Computes the new y given the control input.
   *
   * <p>This is used by state observers directly to run updates based on state estimate.
   *
   * @param x The current state.
   * @param clampedU The control input.
   * @return the updated output matrix Y.
   */
  public Matrix<Outputs, N1> calculateY(Matrix<States, N1> x, Matrix<Inputs, N1> clampedU) {
    return m_C.times(x).plus(m_D.times(clampedU));
  }

  /**
   * Returns the LinearSystem with the outputs listed in outputIndices.
   *
   * <p>This is used by state observers such as the Kalman Filter.
   *
   * @param outputIndices the list of output indices to include in the sliced system.
   * @return the sliced LinearSystem with outputs set to row vectors of LinearSystem.
   * @throws IllegalArgumentException if any outputIndices are outside the range of system outputs.
   * @throws IllegalArgumentException if number of outputIndices exceeds the number of system
   *     outputs.
   * @throws IllegalArgumentException if duplication exists in outputIndices.
   */
  public LinearSystem<States, Inputs, ? extends Num> slice(int... outputIndices) {
    for (int index : outputIndices) {
      if (index < 0 || index >= m_C.getNumRows()) {
        throw new IllegalArgumentException(
            "Output indices out of range. This is usually due to model implementation errors.");
      }
    }

    if (outputIndices.length >= m_C.getNumRows()) {
      throw new IllegalArgumentException(
          "More outputs requested than available. This is usually due to model implementation "
              + "errors.");
    }

    int[] outputIndicesList = Arrays.stream(outputIndices).distinct().sorted().toArray();

    if (outputIndices.length != outputIndicesList.length) {
      throw new IllegalArgumentException(
          "Duplicate indices exist.  This is usually due to model implementation " + "errors.");
    }

    SimpleMatrix new_C_Storage = new SimpleMatrix(outputIndices.length, m_C.getNumCols());
    SimpleMatrix new_D_Storage = new SimpleMatrix(outputIndices.length, m_D.getNumCols());
    int row = 0;
    for (var index : outputIndicesList) {
      new_C_Storage.setRow(row, 0, m_C.extractRowVector(index).getData());
      new_D_Storage.setRow(row, 0, m_D.extractRowVector(index).getData());
      row++;
    }

    return new LinearSystem<>(m_A, m_B, new Matrix<>(new_C_Storage), new Matrix<>(new_D_Storage));
  }

  @Override
  public String toString() {
    return String.format(
        "Linear System: A\n%s\n\nB:\n%s\n\nC:\n%s\n\nD:\n%s\n",
        m_A.toString(), m_B.toString(), m_C.toString(), m_D.toString());
  }

  /**
   * Creates an implementation of the {@link Protobuf} interface for linear systems.
   *
   * @param <States> The number of states of the linear systems this serializer processes.
   * @param <Inputs> The number of inputs of the linear systems this serializer processes.
   * @param <Outputs> The number of outputs of the linear systems this serializer processes.
   * @param states The number of states of the linear systems this serializer processes.
   * @param inputs The number of inputs of the linear systems this serializer processes.
   * @param outputs The number of outputs of the linear systems this serializer processes.
   * @return The protobuf implementation.
   */
  public static <States extends Num, Inputs extends Num, Outputs extends Num>
      LinearSystemProto<States, Inputs, Outputs> getProto(
          Nat<States> states, Nat<Inputs> inputs, Nat<Outputs> outputs) {
    return new LinearSystemProto<>(states, inputs, outputs);
  }

  /**
   * Creates an implementation of the {@link Struct} interface for linear systems.
   *
   * @param <States> The number of states of the linear systems this serializer processes.
   * @param <Inputs> The number of inputs of the linear systems this serializer processes.
   * @param <Outputs> The number of outputs of the linear systems this serializer processes.
   * @param states The number of states of the linear systems this serializer processes.
   * @param inputs The number of inputs of the linear systems this serializer processes.
   * @param outputs The number of outputs of the linear systems this serializer processes.
   * @return The struct implementation.
   */
  public static <States extends Num, Inputs extends Num, Outputs extends Num>
      LinearSystemStruct<States, Inputs, Outputs> getStruct(
          Nat<States> states, Nat<Inputs> inputs, Nat<Outputs> outputs) {
    return new LinearSystemStruct<>(states, inputs, outputs);
  }
}
