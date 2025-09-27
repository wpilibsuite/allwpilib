// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util.struct.parser;

/** A lexed raw struct schema token. */
public enum TokenKind {
  /** Unknown. */
  kUnknown("unknown"),

  /** Integer. */
  kInteger("integer"),

  /** Identifier. */
  kIdentifier("identifier"),

  /** Left square bracket. */
  kLeftBracket("'['"),

  /** Right square bracket. */
  kRightBracket("']'"),

  /** Left curly brace. */
  kLeftBrace("'{'"),

  /** Right curly brace. */
  kRightBrace("'}'"),

  /** Colon. */
  kColon("':'"),

  /** Semicolon. */
  kSemicolon("';'"),

  /** Comma. */
  kComma("','"),

  /** Equals. */
  kEquals("'='"),

  /** End of input. */
  kEndOfInput("<EOF>");

  private final String m_name;

  TokenKind(String name) {
    this.m_name = name;
  }

  @Override
  public String toString() {
    return m_name;
  }
}
