// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.Num;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N10;
import edu.wpi.first.math.numbers.N11;
import edu.wpi.first.math.numbers.N12;
import edu.wpi.first.math.numbers.N13;
import edu.wpi.first.math.numbers.N14;
import edu.wpi.first.math.numbers.N15;
import edu.wpi.first.math.numbers.N16;
import edu.wpi.first.math.numbers.N17;
import edu.wpi.first.math.numbers.N18;
import edu.wpi.first.math.numbers.N19;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.numbers.N20;
import edu.wpi.first.math.numbers.N3;
import edu.wpi.first.math.numbers.N4;
import edu.wpi.first.math.numbers.N5;
import edu.wpi.first.math.numbers.N6;
import edu.wpi.first.math.numbers.N7;
import edu.wpi.first.math.numbers.N8;
import edu.wpi.first.math.numbers.N9;
import edu.wpi.first.math.system.proto.LinearSystemProto;
import edu.wpi.first.math.system.struct.LinearSystemStruct;
import edu.wpi.first.util.protobuf.Protobuf;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.Struct;
import edu.wpi.first.util.struct.StructSerializable;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.stream.Collectors;
import org.ejml.simple.SimpleMatrix;

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
   * @param dtSeconds Timestep for model update.
   * @return the updated x.
   */
  public Matrix<States, N1> calculateX(
      Matrix<States, N1> x, Matrix<Inputs, N1> clampedU, double dtSeconds) {
    var discABpair = Discretization.discretizeAB(m_A, m_B, dtSeconds);

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

    List<Integer> outputIndicesList =
        Arrays.stream(outputIndices).distinct().boxed().collect(Collectors.toList());
    Collections.sort(outputIndicesList);

    if (outputIndices.length != outputIndicesList.size()) {
      throw new IllegalArgumentException(
          "Duplicate indices exist.  This is usually due to model implementation " + "errors.");
    }

    SimpleMatrix new_C_Storage = new SimpleMatrix(outputIndices.length, m_C.getNumCols());
    int row = 0;
    for (var index : outputIndicesList) {
      var current_row_data = m_C.extractRowVector(index).getData();
      new_C_Storage.setRow(row, 0, current_row_data);
      row++;
    }

    SimpleMatrix new_D_Storage = new SimpleMatrix(outputIndices.length, m_D.getNumCols());
    row = 0;
    for (var index : outputIndicesList) {
      var current_row_data = m_D.extractRowVector(index).getData();
      new_D_Storage.setRow(row, 0, current_row_data);
      row++;
    }

    switch (outputIndices.length) {
      case 20:
        Matrix<N20, States> new_C20 = new Matrix<>(new_C_Storage);
        Matrix<N20, Inputs> new_D20 = new Matrix<>(new_D_Storage);
        return new LinearSystem<>(m_A, m_B, new_C20, new_D20);
      case 19:
        Matrix<N19, States> new_C19 = new Matrix<>(new_C_Storage);
        Matrix<N19, Inputs> new_D19 = new Matrix<>(new_D_Storage);
        return new LinearSystem<>(m_A, m_B, new_C19, new_D19);
      case 18:
        Matrix<N18, States> new_C18 = new Matrix<>(new_C_Storage);
        Matrix<N18, Inputs> new_D18 = new Matrix<>(new_D_Storage);
        return new LinearSystem<>(m_A, m_B, new_C18, new_D18);
      case 17:
        Matrix<N17, States> new_C17 = new Matrix<>(new_C_Storage);
        Matrix<N17, Inputs> new_D17 = new Matrix<>(new_D_Storage);
        return new LinearSystem<>(m_A, m_B, new_C17, new_D17);
      case 16:
        Matrix<N16, States> new_C16 = new Matrix<>(new_C_Storage);
        Matrix<N16, Inputs> new_D16 = new Matrix<>(new_D_Storage);
        return new LinearSystem<>(m_A, m_B, new_C16, new_D16);
      case 15:
        Matrix<N15, States> new_C15 = new Matrix<>(new_C_Storage);
        Matrix<N15, Inputs> new_D15 = new Matrix<>(new_D_Storage);
        return new LinearSystem<>(m_A, m_B, new_C15, new_D15);
      case 14:
        Matrix<N14, States> new_C14 = new Matrix<>(new_C_Storage);
        Matrix<N14, Inputs> new_D14 = new Matrix<>(new_D_Storage);
        return new LinearSystem<>(m_A, m_B, new_C14, new_D14);
      case 13:
        Matrix<N13, States> new_C13 = new Matrix<>(new_C_Storage);
        Matrix<N13, Inputs> new_D13 = new Matrix<>(new_D_Storage);
        return new LinearSystem<>(m_A, m_B, new_C13, new_D13);
      case 12:
        Matrix<N12, States> new_C12 = new Matrix<>(new_C_Storage);
        Matrix<N12, Inputs> new_D12 = new Matrix<>(new_D_Storage);
        return new LinearSystem<>(m_A, m_B, new_C12, new_D12);
      case 11:
        Matrix<N11, States> new_C11 = new Matrix<>(new_C_Storage);
        Matrix<N11, Inputs> new_D11 = new Matrix<>(new_D_Storage);
        return new LinearSystem<>(m_A, m_B, new_C11, new_D11);
      case 10:
        Matrix<N10, States> new_C10 = new Matrix<>(new_C_Storage);
        Matrix<N10, Inputs> new_D10 = new Matrix<>(new_D_Storage);
        return new LinearSystem<>(m_A, m_B, new_C10, new_D10);
      case 9:
        Matrix<N9, States> new_C9 = new Matrix<>(new_C_Storage);
        Matrix<N9, Inputs> new_D9 = new Matrix<>(new_D_Storage);
        return new LinearSystem<>(m_A, m_B, new_C9, new_D9);
      case 8:
        Matrix<N8, States> new_C8 = new Matrix<>(new_C_Storage);
        Matrix<N8, Inputs> new_D8 = new Matrix<>(new_D_Storage);
        return new LinearSystem<>(m_A, m_B, new_C8, new_D8);
      case 7:
        Matrix<N7, States> new_C7 = new Matrix<>(new_C_Storage);
        Matrix<N7, Inputs> new_D7 = new Matrix<>(new_D_Storage);
        return new LinearSystem<>(m_A, m_B, new_C7, new_D7);
      case 6:
        Matrix<N6, States> new_C6 = new Matrix<>(new_C_Storage);
        Matrix<N6, Inputs> new_D6 = new Matrix<>(new_D_Storage);
        return new LinearSystem<>(m_A, m_B, new_C6, new_D6);
      case 5:
        Matrix<N5, States> new_C5 = new Matrix<>(new_C_Storage);
        Matrix<N5, Inputs> new_D5 = new Matrix<>(new_D_Storage);
        return new LinearSystem<>(m_A, m_B, new_C5, new_D5);
      case 4:
        Matrix<N4, States> new_C4 = new Matrix<>(new_C_Storage);
        Matrix<N4, Inputs> new_D4 = new Matrix<>(new_D_Storage);
        return new LinearSystem<>(m_A, m_B, new_C4, new_D4);
      case 3:
        Matrix<N3, States> new_C3 = new Matrix<>(new_C_Storage);
        Matrix<N3, Inputs> new_D3 = new Matrix<>(new_D_Storage);
        return new LinearSystem<>(m_A, m_B, new_C3, new_D3);
      case 2:
        Matrix<N2, States> new_C2 = new Matrix<>(new_C_Storage);
        Matrix<N2, Inputs> new_D2 = new Matrix<>(new_D_Storage);
        return new LinearSystem<>(m_A, m_B, new_C2, new_D2);
      default:
        Matrix<N1, States> new_C1 = new Matrix<>(new_C_Storage);
        Matrix<N1, Inputs> new_D1 = new Matrix<>(new_D_Storage);
        return new LinearSystem<>(m_A, m_B, new_C1, new_D1);
    }
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
