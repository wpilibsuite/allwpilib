// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.struct.parser;

/** A lexed raw struct schema token. */
public enum TokenKind {
  /** Unknown. */
  UNKNOWN("unknown"),

  /** Integer. */
  INTEGER("integer"),

  /** Identifier. */
  IDENTIFIER("identifier"),

  /** Left square bracket. */
  LEFT_BRACKET("'['"),

  /** Right square bracket. */
  RIGHT_BRACKET("']'"),

  /** Left curly brace. */
  LEFT_BRACE("'{'"),

  /** Right curly brace. */
  RIGHT_BRACE("'}'"),

  /** Colon. */
  COLON("':'"),

  /** Semicolon. */
  SEMICOLON("';'"),

  /** Comma. */
  COMMA("','"),

  /** Equals. */
  EQUALS("'='"),

  /** End of input. */
  END_OF_INPUT("<EOF>");

  private final String m_name;

  TokenKind(String name) {
    this.m_name = name;
  }

  @Override
  public String toString() {
    return m_name;
  }
}
