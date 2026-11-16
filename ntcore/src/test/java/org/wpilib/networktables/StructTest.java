// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.networktables;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.CsvSource;
import org.junit.jupiter.params.provider.ValueSource;
import org.wpilib.util.struct.Struct;
import org.wpilib.util.struct.StructGenerator;
import org.wpilib.util.struct.StructSerializable;

class StructTest {
  public record TestValue(int value) implements StructSerializable {
    public static final Struct<TestValue> struct = StructGenerator.genRecord(TestValue.class);
  }

  private NetworkTableInstance m_inst;

  @BeforeEach
  void setUp() {
    m_inst = NetworkTableInstance.create();
    m_inst.startLocal();
  }

  @AfterEach
  void tearDown() {
    m_inst.close();
  }

  private void publishSamples(RawPublisher publisher, int malformedIndex) {
    for (int i = 0; i < 3; i++) {
      byte[] payload =
          i == malformedIndex
              ? new byte[] {(byte) 0xff}
              : ByteBuffer.allocate(Integer.BYTES)
                  .order(ByteOrder.LITTLE_ENDIAN)
                  .putInt(i < malformedIndex ? i + 1 : i)
                  .array();
      publisher.set(payload, 10 * (i + 1));
    }
  }

  @ParameterizedTest
  @CsvSource({"0, 20, 30", "1, 10, 30", "2, 10, 20"})
  void readQueuePreservesValidSamples(
      int malformedIndex, long firstTimestamp, long secondTimestamp) {
    try (StructSubscriber<TestValue> subscriber =
            m_inst
                .getStructTopic("test", TestValue.struct)
                .subscribe(new TestValue(0), PubSubOption.SEND_ALL, PubSubOption.pollStorage(3));
        RawPublisher publisher =
            m_inst.getRawTopic("test").publish(TestValue.struct.getTypeString())) {
      publishSamples(publisher, malformedIndex);

      TimestampedObject<TestValue>[] samples = subscriber.readQueue();
      assertEquals(2, samples.length);
      assertNotNull(samples[0]);
      assertNotNull(samples[1]);
      assertEquals(new TestValue(1), samples[0].value);
      assertEquals(firstTimestamp, samples[0].timestamp);
      assertEquals(new TestValue(2), samples[1].value);
      assertEquals(secondTimestamp, samples[1].timestamp);
    }
  }

  @ParameterizedTest
  @ValueSource(ints = {0, 1, 2})
  void readQueueValuesPreservesValidSamples(int malformedIndex) {
    try (StructSubscriber<TestValue> subscriber =
            m_inst
                .getStructTopic("test", TestValue.struct)
                .subscribe(new TestValue(0), PubSubOption.SEND_ALL, PubSubOption.pollStorage(3));
        RawPublisher publisher =
            m_inst.getRawTopic("test").publish(TestValue.struct.getTypeString())) {
      publishSamples(publisher, malformedIndex);

      assertArrayEquals(
          new TestValue[] {new TestValue(1), new TestValue(2)}, subscriber.readQueueValues());
    }
  }
}
