// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.linalg.struct;

import org.wpilib.math.linalg.MatBuilder;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.util.Nat;
import org.wpilib.math.util.Num;
import org.wpilib.util.struct.Struct;
import java.nio.ByteBuffer;

public final class MatrixStruct<R extends Num, C extends Num> implements Struct<Matrix<R, C>> {
  private final Nat<R> m_rows;
  private final Nat<C> m_cols;

  /**
   * Constructs the {@link Struct} implementation.
   *
   * @param rows The number of rows of the matrices this serializer processes.
   * @param cols The number of cols of the matrices this serializer processes.
   */
  public MatrixStruct(Nat<R> rows, Nat<C> cols) {
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
  public String getTypeName() {
    return "Matrix__" + m_rows.getNum() + "_" + m_cols.getNum();
  }

  @Override
  public int getSize() {
    return kSizeDouble * m_rows.getNum() * m_cols.getNum();
  }

  @Override
  public String getSchema() {
    return "double data[" + (m_rows.getNum() * m_cols.getNum()) + "]";
  }

  @Override
  public Matrix<R, C> unpack(ByteBuffer bb) {
    return MatBuilder.fill(
        m_rows, m_cols, Struct.unpackDoubleArray(bb, m_rows.getNum() * m_cols.getNum()));
  }

  @Override
  public void pack(ByteBuffer bb, Matrix<R, C> value) {
    Struct.packArray(bb, value.getData());
  }
}
