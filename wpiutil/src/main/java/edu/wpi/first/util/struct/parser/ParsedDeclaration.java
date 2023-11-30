// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.struct.parser;

import java.util.Map;

/** Raw struct schema declaration. */
public class ParsedDeclaration {
  @SuppressWarnings("MemberName")
  public String typeString;

  @SuppressWarnings("MemberName")
  public String name;

  @SuppressWarnings("MemberName")
  public Map<String, Long> enumValues;

  @SuppressWarnings("MemberName")
  public int arraySize = 1;

  @SuppressWarnings("MemberName")
  public int bitWidth;
}
