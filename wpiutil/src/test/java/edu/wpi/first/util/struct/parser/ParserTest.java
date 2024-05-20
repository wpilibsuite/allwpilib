// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.struct.parser;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class ParserTest {
  @Test
  void testEmpty() {
    Parser p = new Parser("");
    ParsedSchema schema = assertDoesNotThrow(p::parse);
    assertTrue(schema.declarations.isEmpty());
  }

  @Test
  void testEmptySemicolon() {
    Parser p = new Parser(";");
    ParsedSchema schema = assertDoesNotThrow(p::parse);
    assertTrue(schema.declarations.isEmpty());
  }

  @Test
  void testSimple() {
    Parser p = new Parser("int32 a");
    ParsedSchema schema = assertDoesNotThrow(p::parse);
    assertEquals(schema.declarations.size(), 1);
    var decl = schema.declarations.get(0);
    assertEquals(decl.typeString, "int32");
    assertEquals(decl.name, "a");
    assertEquals(decl.arraySize, 1);
  }

  @Test
  void testSimpleTrailingSemi() {
    Parser p = new Parser("int32 a;");
    ParsedSchema schema = assertDoesNotThrow(p::parse);
    assertEquals(schema.declarations.size(), 1);
  }

  @Test
  void testArray() {
    Parser p = new Parser("int32 a[2]");
    ParsedSchema schema = assertDoesNotThrow(p::parse);
    assertEquals(schema.declarations.size(), 1);
    var decl = schema.declarations.get(0);
    assertEquals(decl.typeString, "int32");
    assertEquals(decl.name, "a");
    assertEquals(decl.arraySize, 2);
  }

  @Test
  void testArrayTrailingSemi() {
    Parser p = new Parser("int32 a[2];");
    ParsedSchema schema = assertDoesNotThrow(p::parse);
    assertEquals(schema.declarations.size(), 1);
  }

  @Test
  void testBitfield() {
    Parser p = new Parser("int32 a:2");
    ParsedSchema schema = assertDoesNotThrow(p::parse);
    assertEquals(schema.declarations.size(), 1);
    var decl = schema.declarations.get(0);
    assertEquals(decl.typeString, "int32");
    assertEquals(decl.name, "a");
    assertEquals(decl.bitWidth, 2);
  }

  @Test
  void testBitfieldTrailingSemi() {
    Parser p = new Parser("int32 a:2;");
    ParsedSchema schema = assertDoesNotThrow(p::parse);
    assertEquals(schema.declarations.size(), 1);
  }

  @Test
  void testEnumKeyword() {
    Parser p = new Parser("enum {x=1} int32 a;");
    ParsedSchema schema = assertDoesNotThrow(p::parse);
    assertEquals(schema.declarations.size(), 1);
    var decl = schema.declarations.get(0);
    assertEquals(decl.typeString, "int32");
    assertEquals(decl.name, "a");
    assertEquals(decl.enumValues.size(), 1);
    assertEquals(decl.enumValues.get("x"), 1);
  }

  @Test
  void testEnumNoKeyword() {
    Parser p = new Parser("{x=1} int32 a;");
    ParsedSchema schema = assertDoesNotThrow(p::parse);
    assertEquals(schema.declarations.size(), 1);
    var decl = schema.declarations.get(0);
    assertEquals(decl.typeString, "int32");
    assertEquals(decl.name, "a");
    assertEquals(decl.enumValues.size(), 1);
    assertEquals(decl.enumValues.get("x"), 1);
  }

  @Test
  void testEnumNoValues() {
    Parser p = new Parser("{} int32 a;");
    ParsedSchema schema = assertDoesNotThrow(p::parse);
    assertEquals(schema.declarations.size(), 1);
    var decl = schema.declarations.get(0);
    assertEquals(decl.typeString, "int32");
    assertEquals(decl.name, "a");
    assertTrue(decl.enumValues.isEmpty());
  }

  @Test
  void testEnumMultipleValues() {
    Parser p = new Parser("{x=1,y=-2} int32 a;");
    ParsedSchema schema = assertDoesNotThrow(p::parse);
    assertEquals(schema.declarations.size(), 1);
    var decl = schema.declarations.get(0);
    assertEquals(decl.typeString, "int32");
    assertEquals(decl.name, "a");
    assertEquals(decl.enumValues.size(), 2);
    assertEquals(decl.enumValues.get("x"), 1);
    assertEquals(decl.enumValues.get("y"), -2);
  }

  @Test
  void testEnumTrailingComma() {
    Parser p = new Parser("{x=1,y=2,} int32 a;");
    ParsedSchema schema = assertDoesNotThrow(p::parse);
    assertEquals(schema.declarations.size(), 1);
    var decl = schema.declarations.get(0);
    assertEquals(decl.typeString, "int32");
    assertEquals(decl.name, "a");
    assertEquals(decl.enumValues.size(), 2);
    assertEquals(decl.enumValues.get("x"), 1);
    assertEquals(decl.enumValues.get("y"), 2);
  }

  @Test
  void testMultipleNoTrailingSemi() {
    Parser p = new Parser("int32 a; int16 b");
    ParsedSchema schema = assertDoesNotThrow(p::parse);
    assertEquals(schema.declarations.size(), 2);
    assertEquals(schema.declarations.get(0).typeString, "int32");
    assertEquals(schema.declarations.get(0).name, "a");
    assertEquals(schema.declarations.get(1).typeString, "int16");
    assertEquals(schema.declarations.get(1).name, "b");
  }

  @Test
  void testErrBitfieldArray() {
    Parser p = new Parser("int32 a[1]:2");
    assertThrows(ParseException.class, p::parse, "10: expected ';', got ':'");
  }

  @Test
  void testErrNoArrayValue() {
    Parser p = new Parser("int32 a[]");
    assertThrows(ParseException.class, p::parse, "8: expected integer, got ']'");
  }

  @Test
  void testErrNoBitfieldValue() {
    Parser p = new Parser("int32 a:");
    assertThrows(ParseException.class, p::parse, "8: expected integer, got ''");
  }

  @Test
  void testErrNoNameArray() {
    Parser p = new Parser("int32 [2]");
    assertThrows(ParseException.class, p::parse, "6: expected identifier, got '['");
  }

  @Test
  void testErrNoNameBitField() {
    Parser p = new Parser("int32 :2");
    assertThrows(ParseException.class, p::parse, "6: expected identifier, got ':'");
  }

  @Test
  void testNegativeBitField() {
    Parser p = new Parser("int32 a:-1");
    assertThrows(
        ParseException.class, p::parse, "8: bitfield width '-1' is not a positive integer");
  }

  @Test
  void testNegativeArraySize() {
    Parser p = new Parser("int32 a[-1]");
    assertThrows(ParseException.class, p::parse, "8: array size '-1' is not a positive integer");
  }

  @Test
  void testZeroBitField() {
    Parser p = new Parser("int32 a:0");
    assertThrows(ParseException.class, p::parse, "8: bitfield width '0' is not a positive integer");
  }

  @Test
  void testZeroArraySize() {
    Parser p = new Parser("int32 a[0]");
    assertThrows(ParseException.class, p::parse, "8: array size '0' is not a positive integer");
  }
}
