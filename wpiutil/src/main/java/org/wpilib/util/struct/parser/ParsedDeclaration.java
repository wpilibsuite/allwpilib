// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util.struct.parser;

import java.util.Map;

/** Raw struct schema declaration. */
public class ParsedDeclaration {
  /** Type string. */
  @SuppressWarnings("MemberName")
  public String typeString;

  /** Name. */
  @SuppressWarnings("MemberName")
  public String name;

  /** Enum values. */
  @SuppressWarnings("MemberName")
  public Map<String, Long> enumValues;

  /** Array size. */
  @SuppressWarnings("MemberName")
  public int arraySize = 1;

  /** Bit width. */
  @SuppressWarnings("MemberName")
  public int bitWidth;

  /** Default constructor. */
  public ParsedDeclaration() {}
}
