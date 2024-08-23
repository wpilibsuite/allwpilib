// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.datalog;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.util.struct.Struct;
import edu.wpi.first.util.struct.StructSerializable;
import java.io.ByteArrayOutputStream;
import java.nio.ByteBuffer;
import java.util.Objects;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

@SuppressWarnings("AvoidEscapedUnicodeCharacters")
class DataLogTest {
  static class ImmutableThingStruct implements Struct<ImmutableThing> {
    @Override
    public Class<ImmutableThing> getTypeClass() {
      return ImmutableThing.class;
    }

    @Override
    public String getTypeName() {
      return "Thing";
    }

    @Override
    public int getSize() {
      return 1;
    }

    @Override
    public String getSchema() {
      return "uint8 value";
    }

    @Override
    public ImmutableThing unpack(ByteBuffer bb) {
      return new ImmutableThing(bb.get());
    }

    @Override
    public void pack(ByteBuffer bb, ImmutableThing value) {
      bb.put(value.m_x);
    }

    @Override
    public boolean isImmutable() {
      return true;
    }
  }

  static class ImmutableThing implements StructSerializable {
    byte m_x;

    ImmutableThing(int x) {
      m_x = (byte) x;
    }

    @Override
    public boolean equals(Object obj) {
      return obj instanceof ImmutableThing other && other.m_x == m_x;
    }

    @Override
    public int hashCode() {
      return Objects.hash(m_x);
    }

    public static final ImmutableThingStruct struct = new ImmutableThingStruct();
  }

  static class CloneableThingStruct implements Struct<CloneableThing> {
    @Override
    public Class<CloneableThing> getTypeClass() {
      return CloneableThing.class;
    }

    @Override
    public String getTypeName() {
      return "Thing";
    }

    @Override
    public int getSize() {
      return 1;
    }

    @Override
    public String getSchema() {
      return "uint8 value";
    }

    @Override
    public CloneableThing unpack(ByteBuffer bb) {
      return new CloneableThing(bb.get());
    }

    @Override
    public void pack(ByteBuffer bb, CloneableThing value) {
      bb.put(value.m_x);
    }

    @Override
    public boolean isCloneable() {
      return true;
    }

    @Override
    public CloneableThing clone(CloneableThing obj) throws CloneNotSupportedException {
      return obj.clone();
    }
  }

  @SuppressWarnings("MemberName")
  private static int cloneCalls;

  static class CloneableThing implements StructSerializable, Cloneable {
    byte m_x;

    CloneableThing(int x) {
      m_x = (byte) x;
    }

    @Override
    public boolean equals(Object obj) {
      return obj instanceof CloneableThing other && other.m_x == m_x;
    }

    @Override
    public int hashCode() {
      return Objects.hash(m_x);
    }

    @Override
    public CloneableThing clone() throws CloneNotSupportedException {
      CloneableThing thing = (CloneableThing) super.clone();
      cloneCalls++;
      return thing;
    }

    public static final CloneableThingStruct struct = new CloneableThingStruct();
  }

  static class ThingStruct implements Struct<Thing> {
    @Override
    public Class<Thing> getTypeClass() {
      return Thing.class;
    }

    @Override
    public String getTypeName() {
      return "Thing";
    }

    @Override
    public int getSize() {
      return 1;
    }

    @Override
    public String getSchema() {
      return "uint8 value";
    }

    @Override
    public Thing unpack(ByteBuffer bb) {
      return new Thing(bb.get());
    }

    @Override
    public void pack(ByteBuffer bb, Thing value) {
      bb.put(value.m_x);
    }
  }

  static class Thing implements StructSerializable {
    byte m_x;

    Thing(int x) {
      m_x = (byte) x;
    }

    @Override
    public boolean equals(Object obj) {
      return obj instanceof Thing other && other.m_x == m_x;
    }

    @Override
    public int hashCode() {
      return Objects.hash(m_x);
    }

    public static final ThingStruct struct = new ThingStruct();
  }

  @SuppressWarnings("MemberName")
  private ByteArrayOutputStream data;

  @SuppressWarnings("MemberName")
  private DataLog log;

  @BeforeEach
  public void init() {
    data = new ByteArrayOutputStream();
    log = new DataLogWriter(data);
    cloneCalls = 0;
  }

  @AfterEach
  public void shutdown() {
    log.close();
  }

  @Test
  void testSimpleInt() {
    int entry = log.start("test", "int64", "", 1);
    log.appendInteger(entry, 1, 2);
    log.flush();
    assertEquals(54, data.size());
  }

  @Test
  void testBooleanAppend() {
    BooleanLogEntry entry = new BooleanLogEntry(log, "a", 5);
    entry.append(false, 7);
    log.flush();
    assertEquals(46, data.size());
  }

  @Test
  void testBooleanUpdate() {
    BooleanLogEntry entry = new BooleanLogEntry(log, "a", 5);
    assertFalse(entry.hasLastValue());
    entry.update(false, 7);
    log.flush();
    assertEquals(46, data.size());
    assertTrue(entry.hasLastValue());
    assertFalse(entry.getLastValue());
    entry.update(false, 8);
    log.flush();
    assertEquals(46, data.size());
    entry.update(true, 9);
    log.flush();
    assertEquals(51, data.size());
    assertTrue(entry.hasLastValue());
    assertTrue(entry.getLastValue());
  }

  @Test
  void testIntegerAppend() {
    IntegerLogEntry entry = new IntegerLogEntry(log, "a", 5);
    entry.append(5, 7);
    log.flush();
    assertEquals(51, data.size());
  }

  @Test
  void testIntegerUpdate() {
    IntegerLogEntry entry = new IntegerLogEntry(log, "a", 5);
    assertFalse(entry.hasLastValue());
    entry.update(0, 7);
    log.flush();
    assertEquals(51, data.size());
    assertTrue(entry.hasLastValue());
    assertEquals(0, entry.getLastValue());
    entry.update(0, 8);
    log.flush();
    assertEquals(51, data.size());
    entry.update(2, 9);
    log.flush();
    assertEquals(63, data.size());
    assertTrue(entry.hasLastValue());
    assertEquals(2, entry.getLastValue());
  }

  @Test
  void testFloatAppend() {
    FloatLogEntry entry = new FloatLogEntry(log, "a", 5);
    entry.append(5.0f, 7);
    log.flush();
    assertEquals(47, data.size());
  }

  @Test
  void testFloatUpdate() {
    FloatLogEntry entry = new FloatLogEntry(log, "a", 5);
    assertFalse(entry.hasLastValue());
    entry.update(0.0f, 7);
    log.flush();
    assertEquals(47, data.size());
    assertTrue(entry.hasLastValue());
    assertEquals(0.0f, entry.getLastValue());
    entry.update(0.0f, 8);
    log.flush();
    assertEquals(47, data.size());
    entry.update(0.1f, 9);
    log.flush();
    assertEquals(55, data.size());
    assertTrue(entry.hasLastValue());
    assertEquals(0.1f, entry.getLastValue());
  }

  @Test
  void testDoubleAppend() {
    DoubleLogEntry entry = new DoubleLogEntry(log, "a", 5);
    entry.append(5.0, 7);
    log.flush();
    assertEquals(52, data.size());
  }

  @Test
  void testDoubleUpdate() {
    DoubleLogEntry entry = new DoubleLogEntry(log, "a", 5);
    assertFalse(entry.hasLastValue());
    entry.update(0.0, 7);
    log.flush();
    assertEquals(52, data.size());
    assertTrue(entry.hasLastValue());
    assertEquals(0.0, entry.getLastValue());
    entry.update(0.0, 8);
    log.flush();
    assertEquals(52, data.size());
    entry.update(0.1, 9);
    log.flush();
    assertEquals(64, data.size());
    assertTrue(entry.hasLastValue());
    assertEquals(0.1, entry.getLastValue());
  }

  @Test
  void testStringAppend() {
    StringLogEntry entry = new StringLogEntry(log, "a", 5);
    entry.append("x", 7);
    log.flush();
    assertEquals(45, data.size());
  }

  @Test
  void testStringUpdate() {
    StringLogEntry entry = new StringLogEntry(log, "a", 5);
    assertFalse(entry.hasLastValue());

    entry.update("x", 7);
    log.flush();
    assertEquals(45, data.size());
    assertTrue(entry.hasLastValue());
    assertEquals("x", entry.getLastValue());

    entry.update("x", 8);
    log.flush();
    assertEquals(45, data.size());

    entry.update("y", 9);
    log.flush();
    assertEquals(50, data.size());
    assertTrue(entry.hasLastValue());
    assertEquals("y", entry.getLastValue());

    entry.update("yy", 10);
    log.flush();
    assertEquals(56, data.size());
    assertTrue(entry.hasLastValue());
    assertEquals("yy", entry.getLastValue());

    entry.update("", 11);
    log.flush();
    assertEquals(60, data.size());
    assertTrue(entry.hasLastValue());
    assertEquals("", entry.getLastValue());
  }

  @Test
  void testRawAppend() {
    RawLogEntry entry = new RawLogEntry(log, "a", 5);
    entry.append(new byte[] {5}, 7);
    log.flush();
    assertEquals(42, data.size());
  }

  @Test
  void testRawUpdate() {
    RawLogEntry entry = new RawLogEntry(log, "a", 5);
    assertFalse(entry.hasLastValue());

    entry.update(new byte[] {5}, 7);
    log.flush();
    assertEquals(42, data.size());
    assertTrue(entry.hasLastValue());
    assertArrayEquals(new byte[] {5}, entry.getLastValue());

    entry.update(new byte[] {5}, 8);
    log.flush();
    assertEquals(42, data.size());

    entry.update(new byte[] {6}, 9);
    log.flush();
    assertEquals(47, data.size());
    assertTrue(entry.hasLastValue());
    assertArrayEquals(new byte[] {6}, entry.getLastValue());

    entry.update(new byte[] {6, 6}, 10);
    log.flush();
    assertEquals(53, data.size());
    assertTrue(entry.hasLastValue());
    assertArrayEquals(new byte[] {6, 6}, entry.getLastValue());

    entry.update(new byte[] {}, 11);
    log.flush();
    assertEquals(57, data.size());
    assertTrue(entry.hasLastValue());
    assertArrayEquals(new byte[] {}, entry.getLastValue());
  }

  @Test
  void testBooleanArrayAppendEmpty() {
    BooleanArrayLogEntry entry = new BooleanArrayLogEntry(log, "a", 5);
    entry.append(new boolean[] {}, 7);
    log.flush();
    assertEquals(47, data.size());
  }

  @Test
  void testBooleanArrayAppend() {
    BooleanArrayLogEntry entry = new BooleanArrayLogEntry(log, "a", 5);
    entry.append(new boolean[] {false}, 7);
    log.flush();
    assertEquals(48, data.size());
  }

  @Test
  void testBooleanArrayUpdate() {
    BooleanArrayLogEntry entry = new BooleanArrayLogEntry(log, "a", 5);
    assertFalse(entry.hasLastValue());
    entry.update(new boolean[] {false}, 7);
    log.flush();
    assertEquals(48, data.size());
    assertTrue(entry.hasLastValue());
    assertArrayEquals(new boolean[] {false}, entry.getLastValue());
    entry.update(new boolean[] {false}, 8);
    log.flush();
    assertEquals(48, data.size());
    entry.update(new boolean[] {true}, 9);
    log.flush();
    assertEquals(53, data.size());
    assertTrue(entry.hasLastValue());
    assertArrayEquals(new boolean[] {true}, entry.getLastValue());
    entry.update(new boolean[] {}, 10);
    log.flush();
    assertEquals(57, data.size());
    assertTrue(entry.hasLastValue());
    assertArrayEquals(new boolean[] {}, entry.getLastValue());
  }

  @Test
  void testIntegerArrayAppendEmpty() {
    IntegerArrayLogEntry entry = new IntegerArrayLogEntry(log, "a", 5);
    entry.append(new long[] {}, 7);
    log.flush();
    assertEquals(45, data.size());
  }

  @Test
  void testIntegerArrayAppend() {
    IntegerArrayLogEntry entry = new IntegerArrayLogEntry(log, "a", 5);
    entry.append(new long[] {1}, 7);
    log.flush();
    assertEquals(53, data.size());
  }

  @Test
  void testIntegerArrayUpdate() {
    IntegerArrayLogEntry entry = new IntegerArrayLogEntry(log, "a", 5);
    assertFalse(entry.hasLastValue());
    entry.update(new long[] {1}, 7);
    log.flush();
    assertEquals(53, data.size());
    assertTrue(entry.hasLastValue());
    assertArrayEquals(new long[] {1}, entry.getLastValue());
    entry.update(new long[] {1}, 8);
    log.flush();
    assertEquals(53, data.size());
    entry.update(new long[] {2}, 9);
    log.flush();
    assertEquals(65, data.size());
    assertTrue(entry.hasLastValue());
    assertArrayEquals(new long[] {2}, entry.getLastValue());
    entry.update(new long[] {}, 10);
    log.flush();
    assertEquals(69, data.size());
    assertTrue(entry.hasLastValue());
    assertArrayEquals(new long[] {}, entry.getLastValue());
  }

  @Test
  void testDoubleArrayAppendEmpty() {
    DoubleArrayLogEntry entry = new DoubleArrayLogEntry(log, "a", 5);
    entry.append(new double[] {}, 7);
    log.flush();
    assertEquals(46, data.size());
  }

  @Test
  void testDoubleArrayAppend() {
    DoubleArrayLogEntry entry = new DoubleArrayLogEntry(log, "a", 5);
    entry.append(new double[] {1.0}, 7);
    log.flush();
    assertEquals(54, data.size());
  }

  @Test
  void testDoubleArrayUpdate() {
    DoubleArrayLogEntry entry = new DoubleArrayLogEntry(log, "a", 5);
    assertFalse(entry.hasLastValue());
    entry.update(new double[] {1.0}, 7);
    log.flush();
    assertEquals(54, data.size());
    assertTrue(entry.hasLastValue());
    assertArrayEquals(new double[] {1.0}, entry.getLastValue());
    entry.update(new double[] {1.0}, 8);
    log.flush();
    assertEquals(54, data.size());
    entry.update(new double[] {2.0}, 9);
    log.flush();
    assertEquals(66, data.size());
    assertTrue(entry.hasLastValue());
    assertArrayEquals(new double[] {2}, entry.getLastValue());
    entry.update(new double[] {}, 10);
    log.flush();
    assertEquals(70, data.size());
    assertTrue(entry.hasLastValue());
    assertArrayEquals(new double[] {}, entry.getLastValue());
  }

  @Test
  void testFloatArrayAppendEmpty() {
    FloatArrayLogEntry entry = new FloatArrayLogEntry(log, "a", 5);
    entry.append(new float[] {}, 7);
    log.flush();
    assertEquals(45, data.size());
  }

  @Test
  void testFloatArrayAppend() {
    FloatArrayLogEntry entry = new FloatArrayLogEntry(log, "a", 5);
    entry.append(new float[] {1.0f}, 7);
    log.flush();
    assertEquals(49, data.size());
  }

  @Test
  void testFloatArrayUpdate() {
    FloatArrayLogEntry entry = new FloatArrayLogEntry(log, "a", 5);
    assertFalse(entry.hasLastValue());
    entry.update(new float[] {1.0f}, 7);
    log.flush();
    assertEquals(49, data.size());
    assertTrue(entry.hasLastValue());
    assertArrayEquals(new float[] {1.0f}, entry.getLastValue());
    entry.update(new float[] {1.0f}, 8);
    log.flush();
    assertEquals(49, data.size());
    entry.update(new float[] {2.0f}, 9);
    log.flush();
    assertEquals(57, data.size());
    assertTrue(entry.hasLastValue());
    assertArrayEquals(new float[] {2.0f}, entry.getLastValue());
    entry.update(new float[] {}, 10);
    log.flush();
    assertEquals(61, data.size());
    assertTrue(entry.hasLastValue());
    assertArrayEquals(new float[] {}, entry.getLastValue());
  }

  @Test
  void testStringArrayAppendEmpty() {
    StringArrayLogEntry entry = new StringArrayLogEntry(log, "a", 5);
    entry.append(new String[] {}, 7);
    entry.append(new String[] {}, 7);
    log.flush();
    assertEquals(58, data.size());
  }

  @Test
  void testStringArrayAppend() {
    StringArrayLogEntry entry = new StringArrayLogEntry(log, "a", 5);
    entry.append(new String[] {"x"}, 7);
    log.flush();
    assertEquals(55, data.size());
  }

  @Test
  void testStringArrayUpdate() {
    StringArrayLogEntry entry = new StringArrayLogEntry(log, "a", 5);
    assertFalse(entry.hasLastValue());
    entry.update(new String[] {"x"}, 7);
    log.flush();
    assertEquals(55, data.size());
    assertTrue(entry.hasLastValue());
    assertArrayEquals(new String[] {"x"}, entry.getLastValue());
    entry.update(new String[] {"x"}, 8);
    log.flush();
    assertEquals(55, data.size());
    entry.update(new String[] {"y"}, 9);
    log.flush();
    assertEquals(68, data.size());
    assertTrue(entry.hasLastValue());
    assertArrayEquals(new String[] {"y"}, entry.getLastValue());
    entry.update(new String[] {}, 10);
    log.flush();
    assertEquals(76, data.size());
    assertTrue(entry.hasLastValue());
    assertArrayEquals(new String[] {}, entry.getLastValue());
  }

  @Test
  void testStruct() {
    StructLogEntry<Thing> entry = StructLogEntry.create(log, "a", Thing.struct, 5);
    entry.append(new Thing((byte) 1), 6);
    entry.append(new Thing((byte) 0), 7);
  }

  @Test
  void testStructUpdate() {
    StructLogEntry<Thing> entry = StructLogEntry.create(log, "a", Thing.struct, 5);
    assertFalse(entry.hasLastValue());

    entry.update(new Thing(1), 7);
    log.flush();
    assertEquals(120, data.size());
    assertTrue(entry.hasLastValue());
    assertEquals(new Thing(1), entry.getLastValue());

    entry.update(new Thing(1), 8);
    log.flush();
    assertEquals(120, data.size());

    entry.update(new Thing(2), 9);
    log.flush();
    assertEquals(125, data.size());
    assertTrue(entry.hasLastValue());
    assertEquals(new Thing(2), entry.getLastValue());
  }

  @Test
  void testCloneableStructUpdate() {
    StructLogEntry<CloneableThing> entry =
        StructLogEntry.create(log, "a", CloneableThing.struct, 5);
    assertFalse(entry.hasLastValue());

    entry.update(new CloneableThing(1), 7);
    assertEquals(1, cloneCalls);
    log.flush();
    assertEquals(120, data.size());
    assertTrue(entry.hasLastValue());
    assertEquals(new CloneableThing(1), entry.getLastValue());
    assertEquals(2, cloneCalls);

    entry.update(new CloneableThing(1), 8);
    assertEquals(2, cloneCalls);
    log.flush();
    assertEquals(120, data.size());

    entry.update(new CloneableThing(2), 9);
    assertEquals(3, cloneCalls);
    log.flush();
    assertEquals(125, data.size());
    assertTrue(entry.hasLastValue());
    assertEquals(new CloneableThing(2), entry.getLastValue());
    assertEquals(4, cloneCalls);
  }

  @Test
  void testImmutableStructUpdate() {
    StructLogEntry<ImmutableThing> entry =
        StructLogEntry.create(log, "a", ImmutableThing.struct, 5);
    assertFalse(entry.hasLastValue());

    entry.update(new ImmutableThing(1), 7);
    log.flush();
    assertEquals(120, data.size());
    assertTrue(entry.hasLastValue());
    assertEquals(new ImmutableThing(1), entry.getLastValue());

    entry.update(new ImmutableThing(1), 8);
    log.flush();
    assertEquals(120, data.size());

    entry.update(new ImmutableThing(2), 9);
    log.flush();
    assertEquals(125, data.size());
    assertTrue(entry.hasLastValue());
    assertEquals(new ImmutableThing(2), entry.getLastValue());
  }

  @Test
  void testStructArrayUpdate() {
    StructArrayLogEntry<Thing> entry = StructArrayLogEntry.create(log, "a", Thing.struct, 5);
    assertFalse(entry.hasLastValue());

    entry.update(new Thing[] {new Thing(1), new Thing(2)}, 7);
    log.flush();
    assertEquals(123, data.size());
    assertTrue(entry.hasLastValue());
    assertArrayEquals(new Thing[] {new Thing(1), new Thing(2)}, entry.getLastValue());

    entry.update(new Thing[] {new Thing(1), new Thing(2)}, 8);
    log.flush();
    assertEquals(123, data.size());

    entry.update(new Thing[] {new Thing(1), new Thing(3)}, 9);
    log.flush();
    assertEquals(129, data.size());
    assertTrue(entry.hasLastValue());
    assertArrayEquals(new Thing[] {new Thing(1), new Thing(3)}, entry.getLastValue());

    entry.update(new Thing[] {}, 10);
    log.flush();
    assertEquals(133, data.size());
    assertTrue(entry.hasLastValue());
    assertArrayEquals(new Thing[] {}, entry.getLastValue());
  }

  @Test
  void testCloneableStructArrayUpdate() {
    StructArrayLogEntry<CloneableThing> entry =
        StructArrayLogEntry.create(log, "a", CloneableThing.struct, 5);
    assertFalse(entry.hasLastValue());

    entry.update(new CloneableThing[] {new CloneableThing(1), new CloneableThing(2)}, 7);
    assertEquals(2, cloneCalls);
    log.flush();
    assertEquals(123, data.size());
    assertTrue(entry.hasLastValue());
    assertArrayEquals(
        new CloneableThing[] {new CloneableThing(1), new CloneableThing(2)}, entry.getLastValue());
    assertEquals(4, cloneCalls);

    entry.update(new CloneableThing[] {new CloneableThing(1), new CloneableThing(2)}, 8);
    assertEquals(4, cloneCalls);
    log.flush();
    assertEquals(123, data.size());

    entry.update(new CloneableThing[] {new CloneableThing(1), new CloneableThing(3)}, 9);
    assertEquals(6, cloneCalls);
    log.flush();
    assertEquals(129, data.size());
    assertTrue(entry.hasLastValue());
    assertArrayEquals(
        new CloneableThing[] {new CloneableThing(1), new CloneableThing(3)}, entry.getLastValue());
    assertEquals(8, cloneCalls);

    entry.update(new CloneableThing[] {}, 10);
    assertEquals(8, cloneCalls);
    log.flush();
    assertEquals(133, data.size());
    assertTrue(entry.hasLastValue());
    assertArrayEquals(new CloneableThing[] {}, entry.getLastValue());
    assertEquals(8, cloneCalls);
  }

  @Test
  void testImmutableStructArrayUpdate() {
    StructArrayLogEntry<ImmutableThing> entry =
        StructArrayLogEntry.create(log, "a", ImmutableThing.struct, 5);
    assertFalse(entry.hasLastValue());

    entry.update(new ImmutableThing[] {new ImmutableThing(1), new ImmutableThing(2)}, 7);
    log.flush();
    assertEquals(123, data.size());
    assertTrue(entry.hasLastValue());
    assertArrayEquals(
        new ImmutableThing[] {new ImmutableThing(1), new ImmutableThing(2)}, entry.getLastValue());

    entry.update(new ImmutableThing[] {new ImmutableThing(1), new ImmutableThing(2)}, 8);
    log.flush();
    assertEquals(123, data.size());

    entry.update(new ImmutableThing[] {new ImmutableThing(1), new ImmutableThing(3)}, 9);
    log.flush();
    assertEquals(129, data.size());
    assertTrue(entry.hasLastValue());
    assertArrayEquals(
        new ImmutableThing[] {new ImmutableThing(1), new ImmutableThing(3)}, entry.getLastValue());

    entry.update(new ImmutableThing[] {}, 10);
    log.flush();
    assertEquals(133, data.size());
    assertTrue(entry.hasLastValue());
    assertArrayEquals(new ImmutableThing[] {}, entry.getLastValue());
  }
}
