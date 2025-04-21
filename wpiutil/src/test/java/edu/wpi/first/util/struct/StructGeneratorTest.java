// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.struct;

import static edu.wpi.first.util.struct.StructGenerator.genEnum;
import static edu.wpi.first.util.struct.StructGenerator.genRecord;
import static org.junit.jupiter.api.Assertions.assertEquals;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class StructGeneratorTest {
  public record CustomRecord(int int32, boolean bool, double float64, char character, short int16)
      implements StructSerializable {
    public static CustomRecord create() {
      return new CustomRecord(42, true, Math.PI, 'a', (short) 16);
    }

    public static final Struct<CustomRecord> struct = genRecord(CustomRecord.class);

    @Override
    public final boolean equals(Object arg) {
      return arg == this
          || arg instanceof CustomRecord other
              && int32 == other.int32
              && bool == other.bool
              && float64 == other.float64
              && character == other.character
              && int16 == other.int16;
    }

    @Override
    public final int hashCode() {
      return Integer.hashCode(int32)
          + Boolean.hashCode(bool)
          + Double.hashCode(float64)
          + Character.hashCode(character)
          + Short.hashCode(int16);
    }
  }

  public enum CustomEnum implements StructSerializable {
    A(8),
    B(16),
    C(32),
    D(64);

    public final int value;

    CustomEnum(int value) {
      this.value = value;
    }

    public static final Struct<CustomEnum> struct = genEnum(CustomEnum.class);
  }

  public enum AnimalEnum implements StructSerializable {
    Dog,
    Cat;

    public static final Struct<AnimalEnum> struct = genEnum(AnimalEnum.class);
  }

  public record HigherOrderRecord(
      CustomRecord procRecord, CustomEnum procEnum, AnimalEnum animal, long i64, byte uint8)
      implements StructSerializable {
    public static HigherOrderRecord create() {
      return new HigherOrderRecord(
          CustomRecord.create(), CustomEnum.A, AnimalEnum.Dog, 1234567890123456789L, (byte) 100);
    }

    public static final Struct<HigherOrderRecord> struct = genRecord(HigherOrderRecord.class);

    @Override
    public final boolean equals(Object arg) {
      return arg == this
          || arg instanceof HigherOrderRecord other
              && procRecord.equals(other.procRecord)
              && procEnum == other.procEnum
              && i64 == other.i64
              && uint8 == other.uint8;
    }

    @Override
    public final int hashCode() {
      return procRecord.hashCode()
          + procEnum.hashCode()
          + Long.hashCode(i64)
          + Byte.hashCode(uint8);
    }
  }

  @SuppressWarnings("unchecked")
  private <S extends StructSerializable> void testStructRoundTrip(S value) {
    Struct<S> struct = StructFetcher.fetchStruct((Class<S>) value.getClass()).get();
    ByteBuffer buffer = ByteBuffer.allocate(struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    struct.pack(buffer, value);
    buffer.rewind();

    final S deser = struct.unpack(buffer);
    assertEquals(value, deser);
  }

  @SuppressWarnings("unchecked")
  private <S extends StructSerializable> void testStructDoublePack(S value) {
    Struct<S> struct = StructFetcher.fetchStruct((Class<S>) value.getClass()).get();
    ByteBuffer buffer = ByteBuffer.allocate(struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    struct.pack(buffer, value);
    buffer.rewind();
    struct.pack(buffer, value);
    buffer.rewind();

    final S deser = struct.unpack(buffer);
    assertEquals(value, deser);
  }

  @SuppressWarnings("unchecked")
  private <S extends StructSerializable> void testStructDoubleUnpack(S value) {
    Struct<S> struct = StructFetcher.fetchStruct((Class<S>) value.getClass()).get();
    ByteBuffer buffer = ByteBuffer.allocate(struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    struct.pack(buffer, value);
    buffer.rewind();

    final S deser = struct.unpack(buffer);
    assertEquals(value, deser);
    buffer.rewind();

    final S deser2 = struct.unpack(buffer);
    assertEquals(value, deser2);
  }

  @Test
  void testCustomRecordRoundTrip() {
    testStructRoundTrip(CustomRecord.create());
  }

  @Test
  void testCustomRecordDoublePack() {
    testStructDoublePack(CustomRecord.create());
  }

  @Test
  void testCustomRecordDoubleUnpack() {
    testStructDoubleUnpack(CustomRecord.create());
  }

  @Test
  void testCustomEnumRoundTrip() {
    testStructRoundTrip(CustomEnum.A);
  }

  @Test
  void testCustomEnumDoublePack() {
    testStructDoublePack(CustomEnum.A);
  }

  @Test
  void testCustomEnumDoubleUnpack() {
    testStructDoubleUnpack(CustomEnum.A);
  }

  @Test
  void testHigherOrderRecordRoundTrip() {
    testStructRoundTrip(HigherOrderRecord.create());
  }

  @Test
  void testHigherOrderRecordDoublePack() {
    testStructDoublePack(HigherOrderRecord.create());
  }

  @Test
  void testHigherOrderRecordDoubleUnpack() {
    testStructDoubleUnpack(HigherOrderRecord.create());
  }
}
