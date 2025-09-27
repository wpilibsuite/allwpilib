// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.struct;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.stream.Stream;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

@SuppressWarnings("AvoidEscapedUnicodeCharacters")
class DynamicStructTest {
  @SuppressWarnings("MemberName")
  private StructDescriptorDatabase db;

  @BeforeEach
  public void init() {
    db = new StructDescriptorDatabase();
  }

  @Test
  void testEmpty() {
    var desc = assertDoesNotThrow(() -> db.add("test", ""));
    assertEquals(desc.getName(), "test");
    assertEquals(desc.getSchema(), "");
    assertTrue(desc.getFields().isEmpty());
    assertTrue(desc.isValid());
    assertEquals(desc.getSize(), 0);
  }

  @Test
  void testNestedStruct() {
    var desc = assertDoesNotThrow(() -> db.add("test", "int32 a"));
    assertTrue(desc.isValid());
    var desc2 = assertDoesNotThrow(() -> db.add("test2", "test a"));
    assertTrue(desc2.isValid());
    assertEquals(desc2.getSize(), 4);
  }

  @Test
  void testDelayedValid() {
    final var desc = assertDoesNotThrow(() -> db.add("test", "foo a"));
    assertFalse(desc.isValid());
    var desc2 = assertDoesNotThrow(() -> db.add("test2", "foo a; foo b;"));
    assertFalse(desc2.isValid());
    var desc3 = assertDoesNotThrow(() -> db.add("test3", "foo a[2]"));
    assertFalse(desc3.isValid());
    var desc4 = assertDoesNotThrow(() -> db.add("foo", "int32 a"));
    assertTrue(desc4.isValid());
    assertTrue(desc.isValid());
    assertEquals(desc.getSize(), 4);
    assertTrue(desc2.isValid());
    assertEquals(desc2.getSize(), 8);
    assertTrue(desc3.isValid());
    assertEquals(desc3.getSize(), 8);
  }

  @Test
  void testReuseNestedStructDelayed() {
    var desc2 = assertDoesNotThrow(() -> db.add("test2", "test a;test b;"));
    var desc = assertDoesNotThrow(() -> db.add("test", "int32 a; uint16 b; int16 c;"));
    assertTrue(desc.isValid());
    assertTrue(desc2.isValid());
    assertEquals(desc2.getSize(), 16);
    var fields = desc2.getFields();
    var field = fields.get(0);
    assertEquals(field.getOffset(), 0);
    assertEquals(field.getName(), "a");
    field = fields.get(1);
    assertEquals(field.getOffset(), 8);
    assertEquals(field.getName(), "b");
  }

  @Test
  void testInvalidBitfield() {
    assertThrows(
        BadSchemaException.class,
        () -> db.add("test", "float a:1"),
        "field a: type float cannot be bitfield");
    assertThrows(
        BadSchemaException.class,
        () -> db.add("test", "double a:1"),
        "field a: type double cannot be bitfield");
    assertThrows(
        BadSchemaException.class,
        () -> db.add("test", "foo a:1"),
        "field a: type foo cannot be bitfield");
  }

  @Test
  void testCircularStructReference() {
    assertThrows(
        BadSchemaException.class,
        () -> db.add("test", "test a"),
        "field a: recursive struct reference");
  }

  @Test
  void testNestedCircularStructRef() {
    assertDoesNotThrow(() -> db.add("test", "foo a"));
    assertDoesNotThrow(() -> db.add("foo", "bar a"));
    assertThrows(
        BadSchemaException.class,
        () -> db.add("bar", "test a"),
        "circular struct reference: bar <- foo <- test");

    // ok
    var desc = assertDoesNotThrow(() -> db.add("baz", "bar a"));
    assertFalse(desc.isValid());
  }

  @Test
  void testNestedCircularStructRef2() {
    assertDoesNotThrow(() -> db.add("test", "foo a"));
    assertDoesNotThrow(() -> db.add("bar", "test a"));
    assertThrows(
        BadSchemaException.class,
        () -> db.add("foo", "bar a"),
        "circular struct reference: foo <- test <- bar");
  }

  @Test
  void testBitfieldBasic() {
    var desc = assertDoesNotThrow(() -> db.add("test", "int32 a:2; uint32 b:30"));
    assertEquals(desc.getSize(), 4);
    var fields = desc.getFields();
    assertEquals(fields.size(), 2);
    var field = fields.get(0);
    assertEquals(field.getBitWidth(), 2);
    assertEquals(field.getBitShift(), 0);
    assertEquals(field.getBitMask(), 0x3);
    assertEquals(field.getOffset(), 0);
    assertEquals(field.getSize(), 4);
    field = fields.get(1);
    assertEquals(field.getBitWidth(), 30);
    assertEquals(field.getBitShift(), 2);
    assertEquals(field.getBitMask(), 0x3fffffff);
    assertEquals(field.getOffset(), 0);
    assertEquals(field.getSize(), 4);
  }

  @Test
  void testBitfieldDiffType() {
    var desc = assertDoesNotThrow(() -> db.add("test", "int32 a:2; int16 b:2"));
    assertEquals(desc.getSize(), 6);
    var fields = desc.getFields();
    assertEquals(fields.size(), 2);
    var field = fields.get(0);
    assertEquals(field.getBitWidth(), 2);
    assertEquals(field.getBitShift(), 0);
    assertEquals(field.getBitMask(), 0x3);
    assertEquals(field.getOffset(), 0);
    assertEquals(field.getSize(), 4);
    field = fields.get(1);
    assertEquals(field.getBitWidth(), 2);
    assertEquals(field.getBitShift(), 0);
    assertEquals(field.getBitMask(), 0x3);
    assertEquals(field.getOffset(), 4);
    assertEquals(field.getSize(), 2);
  }

  @Test
  void testBitfieldOverflow() {
    var desc = assertDoesNotThrow(() -> db.add("test", "int8 a:4; int8 b:5"));
    assertEquals(desc.getSize(), 2);
    var fields = desc.getFields();
    assertEquals(fields.size(), 2);
    var field = fields.get(0);
    assertEquals(field.getBitWidth(), 4);
    assertEquals(field.getBitShift(), 0);
    assertEquals(field.getBitMask(), 0xf);
    assertEquals(field.getOffset(), 0);
    assertEquals(field.getSize(), 1);
    field = fields.get(1);
    assertEquals(field.getBitWidth(), 5);
    assertEquals(field.getBitMask(), 0x1f);
    assertEquals(field.getBitShift(), 0);
    assertEquals(field.getOffset(), 1);
    assertEquals(field.getSize(), 1);
  }

  @Test
  void testBitfieldBoolBegin8() {
    var desc = assertDoesNotThrow(() -> db.add("test", "bool a:1; int8 b:5"));
    assertEquals(desc.getSize(), 1);
    var fields = desc.getFields();
    assertEquals(fields.size(), 2);
    var field = fields.get(0);
    assertEquals(field.getBitWidth(), 1);
    assertEquals(field.getBitShift(), 0);
    assertEquals(field.getBitMask(), 0x1);
    assertEquals(field.getOffset(), 0);
    assertEquals(field.getSize(), 1);
    field = fields.get(1);
    assertEquals(field.getBitWidth(), 5);
    assertEquals(field.getBitMask(), 0x1f);
    assertEquals(field.getBitShift(), 1);
    assertEquals(field.getOffset(), 0);
    assertEquals(field.getSize(), 1);
  }

  @Test
  void testBitfieldBoolBegin16() {
    var desc = assertDoesNotThrow(() -> db.add("test", "bool a:1; int16 b:5"));
    assertEquals(desc.getSize(), 3);
    var fields = desc.getFields();
    assertEquals(fields.size(), 2);
    var field = fields.get(0);
    assertEquals(field.getBitWidth(), 1);
    assertEquals(field.getBitShift(), 0);
    assertEquals(field.getBitMask(), 0x1);
    assertEquals(field.getOffset(), 0);
    assertEquals(field.getSize(), 1);
    field = fields.get(1);
    assertEquals(field.getBitWidth(), 5);
    assertEquals(field.getBitMask(), 0x1f);
    assertEquals(field.getBitShift(), 0);
    assertEquals(field.getOffset(), 1);
    assertEquals(field.getSize(), 2);
  }

  @Test
  void testBitfieldBoolMid() {
    var desc =
        assertDoesNotThrow(() -> db.add("test", "int16 a:2; bool b:1; bool c:1; uint16 d:5"));
    assertEquals(desc.getSize(), 2);
    var fields = desc.getFields();
    assertEquals(fields.size(), 4);
    var field = fields.get(0);
    assertEquals(field.getBitWidth(), 2);
    assertEquals(field.getBitShift(), 0);
    assertEquals(field.getBitMask(), 0x3);
    assertEquals(field.getOffset(), 0);
    assertEquals(field.getSize(), 2);
    field = fields.get(1);
    assertEquals(field.getBitWidth(), 1);
    assertEquals(field.getBitMask(), 0x1);
    assertEquals(field.getBitShift(), 2);
    assertEquals(field.getOffset(), 0);
    assertEquals(field.getSize(), 2);
    field = fields.get(2);
    assertEquals(field.getBitWidth(), 1);
    assertEquals(field.getBitMask(), 0x1);
    assertEquals(field.getBitShift(), 3);
    assertEquals(field.getOffset(), 0);
    assertEquals(field.getSize(), 2);
    field = fields.get(3);
    assertEquals(field.getBitWidth(), 5);
    assertEquals(field.getBitMask(), 0x1f);
    assertEquals(field.getBitShift(), 4);
    assertEquals(field.getOffset(), 0);
    assertEquals(field.getSize(), 2);
  }

  @Test
  void testBitfieldBoolEnd() {
    var desc = assertDoesNotThrow(() -> db.add("test", "int16 a:15; bool b:1"));
    assertEquals(desc.getSize(), 2);
    var fields = desc.getFields();
    assertEquals(fields.size(), 2);
    var field = fields.get(0);
    assertEquals(field.getBitWidth(), 15);
    assertEquals(field.getBitShift(), 0);
    assertEquals(field.getBitMask(), 0x7fff);
    assertEquals(field.getOffset(), 0);
    assertEquals(field.getSize(), 2);
    field = fields.get(1);
    assertEquals(field.getBitWidth(), 1);
    assertEquals(field.getBitMask(), 0x1);
    assertEquals(field.getBitShift(), 15);
    assertEquals(field.getOffset(), 0);
    assertEquals(field.getSize(), 2);
  }

  @Test
  void testBitfieldBoolEnd2() {
    var desc = assertDoesNotThrow(() -> db.add("test", "int16 a:16; bool b:1"));
    assertEquals(desc.getSize(), 3);
    var fields = desc.getFields();
    assertEquals(fields.size(), 2);
    var field = fields.get(0);
    assertEquals(field.getBitWidth(), 16);
    assertEquals(field.getBitShift(), 0);
    assertEquals(field.getBitMask(), 0xffff);
    assertEquals(field.getOffset(), 0);
    assertEquals(field.getSize(), 2);
    field = fields.get(1);
    assertEquals(field.getBitWidth(), 1);
    assertEquals(field.getBitMask(), 0x1);
    assertEquals(field.getBitShift(), 0);
    assertEquals(field.getOffset(), 2);
    assertEquals(field.getSize(), 1);
  }

  @Test
  void testBitfieldBoolWrongSize() {
    assertThrows(
        BadSchemaException.class,
        () -> db.add("test", "bool a:2"),
        "field a: bit width must be 1 for bool type");
  }

  @Test
  void testBitfieldTooBig() {
    assertThrows(
        BadSchemaException.class,
        () -> db.add("test", "int16 a:17"),
        "field a: bit width 17 exceeds type size");
  }

  @Test
  void testDuplicateFieldName() {
    assertThrows(
        BadSchemaException.class,
        () -> db.add("test", "int16 a; int8 a"),
        "field a: duplicate field name");
  }

  private static Stream<Arguments> provideSimpleTestParams() {
    return Stream.of(
        Arguments.of("bool a", 1, StructFieldType.kBool, false, false, 8, 0xff, 0, 0),
        Arguments.of("char a", 1, StructFieldType.kChar, false, false, 8, 0xff, 0, 0),
        Arguments.of("int8 a", 1, StructFieldType.kInt8, true, false, 8, 0xff, -128, 127),
        Arguments.of("int16 a", 2, StructFieldType.kInt16, true, false, 16, 0xffff, -32768, 32767),
        Arguments.of(
            "int32 a",
            4,
            StructFieldType.kInt32,
            true,
            false,
            32,
            0xffffffffL,
            -2147483648,
            2147483647),
        Arguments.of(
            "int64 a",
            8,
            StructFieldType.kInt64,
            true,
            false,
            64,
            -1,
            -9223372036854775808L,
            9223372036854775807L),
        Arguments.of("uint8 a", 1, StructFieldType.kUint8, false, true, 8, 0xff, 0, 255),
        Arguments.of("uint16 a", 2, StructFieldType.kUint16, false, true, 16, 0xffff, 0, 65535),
        Arguments.of(
            "uint32 a", 4, StructFieldType.kUint32, false, true, 32, 0xffffffffL, 0, 4294967295L),
        Arguments.of("uint64 a", 8, StructFieldType.kUint64, false, true, 64, -1, 0, 0),
        Arguments.of("float a", 4, StructFieldType.kFloat, false, false, 32, 0xffffffffL, 0, 0),
        Arguments.of("float32 a", 4, StructFieldType.kFloat, false, false, 32, 0xffffffffL, 0, 0),
        Arguments.of("double a", 8, StructFieldType.kDouble, false, false, 64, -1, 0, 0),
        Arguments.of("float64 a", 8, StructFieldType.kDouble, false, false, 64, -1, 0, 0),
        Arguments.of("foo a", 0, StructFieldType.kStruct, false, false, 0, 0, 0, 0));
  }

  @ParameterizedTest
  @MethodSource("provideSimpleTestParams")
  void testStandardCheck(
      String schema,
      int size,
      StructFieldType type,
      boolean isInt,
      boolean isUint,
      int bitWidth,
      long bitMask) {
    var desc = assertDoesNotThrow(() -> db.add("test", schema));
    assertEquals(desc.getName(), "test");
    assertEquals(desc.getSchema(), schema);
    var fields = desc.getFields();
    assertEquals(fields.size(), 1);
    var field = fields.get(0);
    assertEquals(field.getParent(), desc);
    assertEquals(field.getName(), "a");
    assertEquals(field.isInt(), isInt);
    assertEquals(field.isUint(), isUint);
    assertFalse(field.isArray());
    if (type != StructFieldType.kStruct) {
      assertTrue(desc.isValid());
      assertEquals(desc.getSize(), size);
      assertEquals(field.getSize(), size);
      assertEquals(field.getBitWidth(), bitWidth);
      assertEquals(field.getBitMask(), bitMask);
    } else {
      assertFalse(desc.isValid());
      assertNotNull(field.getStruct());
    }
  }

  @ParameterizedTest
  @MethodSource("provideSimpleTestParams")
  void testStandardArray(
      String schema,
      int size,
      StructFieldType type,
      boolean isInt,
      boolean isUint,
      int bitWidth,
      long bitMask) {
    var desc = assertDoesNotThrow(() -> db.add("test", schema + "[2]"));
    assertEquals(desc.getName(), "test");
    assertEquals(desc.getSchema(), schema + "[2]");
    var fields = desc.getFields();
    assertEquals(fields.size(), 1);
    var field = fields.get(0);
    assertEquals(field.getParent(), desc);
    assertEquals(field.getName(), "a");
    assertEquals(field.isInt(), isInt);
    assertEquals(field.isUint(), isUint);
    assertTrue(field.isArray());
    assertEquals(field.getArraySize(), 2);
    if (type != StructFieldType.kStruct) {
      assertTrue(desc.isValid());
      assertEquals(desc.getSize(), size * 2);
    } else {
      assertFalse(desc.isValid());
      assertNotNull(field.getStruct());
    }
  }

  @ParameterizedTest
  @MethodSource("provideSimpleTestParams")
  void testIntRoundTrip(
      String schema,
      int size,
      StructFieldType type,
      boolean isInt,
      boolean isUint,
      int bitWidth,
      long bitMask,
      long minVal,
      long maxVal) {
    if (type == StructFieldType.kStruct) {
      return;
    }
    var desc = assertDoesNotThrow(() -> db.add("test", schema));
    assertTrue(desc.isValid());
    var dynamic = DynamicStruct.allocate(desc);
    var field = desc.findFieldByName("a");
    assertNotNull(field);
    if ((isInt || isUint) && type != StructFieldType.kUint64) {
      // Java can't represent uint64 max
      dynamic.setIntField(field, minVal);
      assertEquals(minVal, dynamic.getIntField(field));
      dynamic.setIntField(field, maxVal);
      assertEquals(maxVal, dynamic.getIntField(field));
    } else if (type == StructFieldType.kBool) {
      dynamic.setBoolField(field, false);
      assertFalse(dynamic.getBoolField(field));
      dynamic.setBoolField(field, true);
      assertTrue(dynamic.getBoolField(field));
    }
  }

  @Test
  void testStringAllZeros() {
    var desc = assertDoesNotThrow(() -> db.add("test", "char a[32]"));
    var dynamic = DynamicStruct.allocate(desc);
    var field = desc.findFieldByName("a");
    assertEquals("", dynamic.getStringField(field));
  }

  @Test
  void testStringRoundTrip() {
    var desc = assertDoesNotThrow(() -> db.add("test", "char a[32]"));
    var dynamic = DynamicStruct.allocate(desc);
    var field = desc.findFieldByName("a");
    assertTrue(dynamic.setStringField(field, "abc"));
    assertEquals("abc", dynamic.getStringField(field));
  }

  @Test
  void testStringRoundTripEmbeddedNull() {
    var desc = assertDoesNotThrow(() -> db.add("test", "char a[32]"));
    var dynamic = DynamicStruct.allocate(desc);
    var field = desc.findFieldByName("a");
    assertTrue(dynamic.setStringField(field, "ab\0c"));
    assertEquals("ab\0c", dynamic.getStringField(field));
  }

  @Test
  void testStringRoundTripStringTooLong() {
    var desc = assertDoesNotThrow(() -> db.add("test", "char a[2]"));
    var dynamic = DynamicStruct.allocate(desc);
    var field = desc.findFieldByName("a");
    assertFalse(dynamic.setStringField(field, "abc"));
    assertEquals("ab", dynamic.getStringField(field));
  }

  @Test
  void testStringRoundTripPartial2ByteUtf8() {
    var desc = assertDoesNotThrow(() -> db.add("test", "char a[2]"));
    var dynamic = DynamicStruct.allocate(desc);
    var field = desc.findFieldByName("a");
    assertFalse(dynamic.setStringField(field, "a\u0234"));
    assertEquals("a", dynamic.getStringField(field));
  }

  @Test
  void testStringRoundTrip2ByteUtf8() {
    var desc = assertDoesNotThrow(() -> db.add("test", "char a[3]"));
    var dynamic = DynamicStruct.allocate(desc);
    var field = desc.findFieldByName("a");
    assertTrue(dynamic.setStringField(field, "a\u0234"));
    assertEquals("a\u0234", dynamic.getStringField(field));
  }

  @Test
  void testStringRoundTripPartial3ByteUtf8FirstByte() {
    var desc = assertDoesNotThrow(() -> db.add("test", "char a[2]"));
    var dynamic = DynamicStruct.allocate(desc);
    var field = desc.findFieldByName("a");
    assertFalse(dynamic.setStringField(field, "a\u1234"));
    assertEquals("a", dynamic.getStringField(field));
  }

  @Test
  void testStringRoundTripPartial3ByteUtf8SecondByte() {
    var desc = assertDoesNotThrow(() -> db.add("test", "char a[3]"));
    var dynamic = DynamicStruct.allocate(desc);
    var field = desc.findFieldByName("a");
    assertFalse(dynamic.setStringField(field, "a\u1234"));
    assertEquals("a", dynamic.getStringField(field));
  }

  @Test
  void testStringRoundTrip3ByteUtf8() {
    var desc = assertDoesNotThrow(() -> db.add("test", "char a[4]"));
    var dynamic = DynamicStruct.allocate(desc);
    var field = desc.findFieldByName("a");
    assertTrue(dynamic.setStringField(field, "a\u1234"));
    assertEquals("a\u1234", dynamic.getStringField(field));
  }

  @Test
  void testStringRoundTripPartial4ByteUtf8FirstByte() {
    var desc = assertDoesNotThrow(() -> db.add("test", "char a[2]"));
    var dynamic = DynamicStruct.allocate(desc);
    var field = desc.findFieldByName("a");
    assertFalse(dynamic.setStringField(field, "a\uD83D\uDC00"));
    assertEquals("a", dynamic.getStringField(field));
  }

  @Test
  void testStringRoundTripPartial4ByteUtf8SecondByte() {
    var desc = assertDoesNotThrow(() -> db.add("test", "char a[3]"));
    var dynamic = DynamicStruct.allocate(desc);
    var field = desc.findFieldByName("a");
    assertFalse(dynamic.setStringField(field, "a\uD83D\uDC00"));
    assertEquals("a", dynamic.getStringField(field));
  }

  @Test
  void testStringRoundTripPartial4ByteUtf8ThirdByte() {
    var desc = assertDoesNotThrow(() -> db.add("test", "char a[4]"));
    var dynamic = DynamicStruct.allocate(desc);
    var field = desc.findFieldByName("a");
    assertFalse(dynamic.setStringField(field, "a\uD83D\uDC00"));
    assertEquals("a", dynamic.getStringField(field));
  }

  @Test
  void testStringRoundTrip4ByteUtf8() {
    var desc = assertDoesNotThrow(() -> db.add("test", "char a[5]"));
    var dynamic = DynamicStruct.allocate(desc);
    var field = desc.findFieldByName("a");
    assertTrue(dynamic.setStringField(field, "a\uD83D\uDC00"));
    assertEquals("a\uD83D\uDC00", dynamic.getStringField(field));
  }
}
