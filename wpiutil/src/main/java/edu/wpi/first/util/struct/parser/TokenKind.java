// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.struct.parser;

/** A lexed raw struct schema token. */
public enum TokenKind {
  kUnknown("unknown"),
  kInteger("integer"),
  kIdentifier("identifier"),
  kLeftBracket("'['"),
  kRightBracket("']'"),
  kLeftBrace("'{'"),
  kRightBrace("'}'"),
  kColon("':'"),
  kSemicolon("';'"),
  kComma("','"),
  kEquals("'='"),
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
