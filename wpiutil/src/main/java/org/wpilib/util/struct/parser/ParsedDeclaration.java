// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util.struct.parser;

import java.util.Map;

/** Raw struct schema declaration. */
public class ParsedDeclaration {
  /** Type string. */
  public String typeString;

  /** Name. */
  public String name;

  /** Enum values. */
  public Map<String, Long> enumValues;

  /** Array size. */
  public int arraySize = 1;

  /** Bit width. */
  public int bitWidth;

  /** Default constructor. */
  public ParsedDeclaration() {}
}
