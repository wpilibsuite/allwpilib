// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

public abstract class StructTestBase<T> {
  private final T m_testData;
  private final Struct<T> m_struct;

  protected StructTestBase(T testData, Struct<T> struct) {
    m_testData = testData;
    m_struct = struct;
  }

  public abstract void checkEquals(T testData, T data);

  @Test
  void testRoundTrip() {
    ByteBuffer buffer = ByteBuffer.allocate(m_struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    m_struct.pack(buffer, m_testData);
    buffer.rewind();

    final T data = m_struct.unpack(buffer);
    checkEquals(m_testData, data);
  }

  @Test
  void testDoublePack() {
    ByteBuffer buffer = ByteBuffer.allocate(m_struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    m_struct.pack(buffer, m_testData);
    buffer.rewind();
    m_struct.pack(buffer, m_testData);
    buffer.rewind();

    final T data = m_struct.unpack(buffer);
    checkEquals(m_testData, data);
  }

  @Test
  void testDoubleUnpack() {
    ByteBuffer buffer = ByteBuffer.allocate(m_struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    m_struct.pack(buffer, m_testData);
    buffer.rewind();

    T data = m_struct.unpack(buffer);
    checkEquals(m_testData, data);
    buffer.rewind();

    data = m_struct.unpack(buffer);
    checkEquals(m_testData, data);
  }
}
