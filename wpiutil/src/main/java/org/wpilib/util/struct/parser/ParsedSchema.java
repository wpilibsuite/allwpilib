// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util.struct.parser;

import java.util.ArrayList;
import java.util.List;

/** Raw struct schema. */
public class ParsedSchema {
  /** Declarations. */
  @SuppressWarnings("MemberName")
  public List<ParsedDeclaration> declarations = new ArrayList<>();

  /** Default constructor. */
  public ParsedSchema() {}
}
