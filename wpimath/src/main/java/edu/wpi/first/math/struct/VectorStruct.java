// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.struct;

import edu.wpi.first.math.Nat;
import edu.wpi.first.math.Num;
import edu.wpi.first.math.Vector;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;
import org.ejml.simple.SimpleMatrix;

public final class VectorStruct<R extends Num> implements Struct<Vector<R>> {
  private final Nat<R> m_rows;

  /**
   * Constructs the {@link Struct} implementation.
   *
   * @param rows The number of rows of the vectors this serializer processes.
   */
  public VectorStruct(Nat<R> rows) {
    m_rows = rows;
  }

  @Override
  public Class<Vector<R>> getTypeClass() {
    @SuppressWarnings("unchecked")
    var clazz = (Class<Vector<R>>) (Class<?>) Vector.class;
    return clazz;
  }

  @Override
  public String getTypeString() {
    return "struct:Vector__" + m_rows.getNum();
  }

  @Override
  public int getSize() {
    return kSizeDouble * m_rows.getNum();
  }

  @Override
  public String getSchema() {
    return "double data[" + m_rows.getNum() + "]";
  }

  @Override
  public Vector<R> unpack(ByteBuffer bb) {
    var storage = new SimpleMatrix(Struct.unpackDoubleArray(bb, m_rows.getNum()));
    return new Vector<R>(storage);
  }

  @Override
  public void pack(ByteBuffer bb, Vector<R> value) {
    Struct.packArray(bb, value.getData());
  }
}
