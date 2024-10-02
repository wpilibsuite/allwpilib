package edu.wpi.first.util.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.util.struct.ProceduralStructifier.ProcEnumStruct;
import edu.wpi.first.util.struct.ProceduralStructifier.ProcRecordStruct;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class ProceduralStructifierRecordTest {
  public record CustomRecord(int int32, boolean bool, double float64, char character, short int16)
      implements StructSerializable {

    public static CustomRecord create() {
      return new CustomRecord(42, true, Math.PI, 'a', (short) 16);
    }

    public static final Struct<CustomRecord> struct = ProcRecordStruct.generate(CustomRecord.class);

    @Override
    public final boolean equals(Object arg) {
      if (arg == this) {
        return true;
      }
      if (!(arg instanceof CustomRecord)) {
        return false;
      }
      CustomRecord other = (CustomRecord) arg;
      return int32 == other.int32
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

    public static final Struct<CustomEnum> struct = ProcEnumStruct.generate(CustomEnum.class);
  }

  public record HigherOrderRecord(
      CustomRecord procRecord, CustomEnum procEnum, long i64, byte uint8)
      implements StructSerializable {

    public static HigherOrderRecord create() {
      return new HigherOrderRecord(
          CustomRecord.create(), CustomEnum.A, 1234567890123456789L, (byte) 255);
    }

    public static final Struct<HigherOrderRecord> struct =
        ProcRecordStruct.generate(HigherOrderRecord.class);

    @Override
    public final boolean equals(Object arg) {
      if (arg == this) {
        return true;
      }
      if (!(arg instanceof HigherOrderRecord)) {
        return false;
      }
      HigherOrderRecord other = (HigherOrderRecord) arg;
      return procRecord.equals(other.procRecord)
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
    Struct<S> struct = ProceduralStructifier.extractClassStruct((Class<S>) value.getClass()).get();
    ByteBuffer buffer = ByteBuffer.allocate(struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    struct.pack(buffer, value);
    buffer.rewind();

    final S deser = struct.unpack(buffer);
    assertEquals(value, deser);
  }

  @SuppressWarnings("unchecked")
  private <S extends StructSerializable> void testStructDoublePack(S value) {
    Struct<S> struct = ProceduralStructifier.extractClassStruct((Class<S>) value.getClass()).get();
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
    Struct<S> struct = ProceduralStructifier.extractClassStruct((Class<S>) value.getClass()).get();
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
