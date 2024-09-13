// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.proto;

import edu.wpi.first.math.Nat;
import edu.wpi.first.math.Num;
import edu.wpi.first.math.Vector;
import edu.wpi.first.math.proto.Wpimath.ProtobufVector;
import edu.wpi.first.util.protobuf.Protobuf;
import org.ejml.simple.SimpleMatrix;
import us.hebi.quickbuf.Descriptors.Descriptor;

public final class VectorProto<R extends Num> implements Protobuf<Vector<R>, ProtobufVector> {
  private final Nat<R> m_rows;

  /**
   * Constructs the {@link Protobuf} implementation.
   *
   * @param rows The number of rows of the vectors this serializer processes.
   */
  public VectorProto(Nat<R> rows) {
    m_rows = rows;
  }

  @Override
  public Class<Vector<R>> getTypeClass() {
    @SuppressWarnings("unchecked")
    var clazz = (Class<Vector<R>>) (Class<?>) Vector.class;
    return clazz;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufVector.getDescriptor();
  }

  @Override
  public ProtobufVector createMessage() {
    return ProtobufVector.newInstance();
  }

  @Override
  public Vector<R> unpack(ProtobufVector msg) {
    if (msg.getRows().length() != m_rows.getNum()) {
      throw new IllegalArgumentException(
          "Tried to unpack msg "
              + msg
              + " with "
              + msg.getRows().length()
              + " rows into Vector with "
              + m_rows.getNum()
              + " rows");
    }
    var storage = new SimpleMatrix(Protobuf.unpackArray(msg.getRows()));
    return new Vector<R>(storage);
  }

  @Override
  public void pack(ProtobufVector msg, Vector<R> value) {
    Protobuf.packArray(msg.getMutableRows(), value.getData());
  }
}
