// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.proto;

import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.Num;
import edu.wpi.first.math.proto.Wpimath.ProtobufMatrix;
import edu.wpi.first.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class MatrixProto<R extends Num, C extends Num>
    implements Protobuf<Matrix<R, C>, ProtobufMatrix> {
  private final Nat<R> m_rows;
  private final Nat<C> m_cols;

  /**
   * Constructs the {@link Protobuf} implementation.
   *
   * @param rows The number of rows of the matrices this serializer processes.
   * @param cols The number of cols of the matrices this serializer processes.
   */
  public MatrixProto(Nat<R> rows, Nat<C> cols) {
    m_rows = rows;
    m_cols = cols;
  }

  @Override
  public Class<Matrix<R, C>> getTypeClass() {
    @SuppressWarnings("unchecked")
    var clazz = (Class<Matrix<R, C>>) (Class<?>) Matrix.class;
    return clazz;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufMatrix.getDescriptor();
  }

  @Override
  public ProtobufMatrix createMessage() {
    return ProtobufMatrix.newInstance();
  }

  @Override
  public Matrix<R, C> unpack(ProtobufMatrix msg) {
    if (msg.getNumRows() != m_rows.getNum() || msg.getNumCols() != m_cols.getNum()) {
      throw new IllegalArgumentException(
          "Tried to unpack msg "
              + msg
              + " with "
              + msg.getNumRows()
              + " rows and "
              + msg.getNumCols()
              + " columns into Matrix with "
              + m_rows.getNum()
              + " rows and "
              + m_cols.getNum()
              + " columns");
    }
    return MatBuilder.fill(m_rows, m_cols, Protobuf.unpackArray(msg.getData()));
  }

  @Override
  public void pack(ProtobufMatrix msg, Matrix<R, C> value) {
    msg.setNumRows(value.getNumRows());
    msg.setNumCols(value.getNumCols());
    Protobuf.packArray(msg.getMutableData(), value.getData());
  }
}
