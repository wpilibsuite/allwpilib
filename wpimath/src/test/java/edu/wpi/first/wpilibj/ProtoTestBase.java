// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.util.protobuf.Protobuf;
import org.junit.jupiter.api.Test;
import us.hebi.quickbuf.ProtoMessage;

public abstract class ProtoTestBase<T, MessageType extends ProtoMessage<MessageType>> {
  private final T m_testData;
  private final Protobuf<T, MessageType> m_proto;

  protected ProtoTestBase(T testData, Protobuf<T, MessageType> proto) {
    m_testData = testData;
    m_proto = proto;
  }

  public abstract void checkEquals(T testData, T data);

  @Test
  void testRoundTrip() {
    final MessageType msg = m_proto.createMessage();
    m_proto.pack(msg, m_testData);

    final T data = m_proto.unpack(msg);
    checkEquals(m_testData, data);
  }

  @Test
  void testDoublePack() {
    final MessageType msg = m_proto.createMessage();
    m_proto.pack(msg, m_testData);
    m_proto.pack(msg, m_testData);

    final T data = m_proto.unpack(msg);
    checkEquals(m_testData, data);
  }

  @Test
  void testDoubleUnpack() {
    final MessageType msg = m_proto.createMessage();
    m_proto.pack(msg, m_testData);

    T data = m_proto.unpack(msg);
    checkEquals(m_testData, data);

    data = m_proto.unpack(msg);
    checkEquals(m_testData, data);
  }
}
