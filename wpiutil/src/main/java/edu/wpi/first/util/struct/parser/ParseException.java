// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.struct.parser;

public class ParseException extends Exception {
  /** The parser position. */
  private final int m_pos;

  public ParseException(int pos, String s) {
    super(s);
    m_pos = pos;
  }

  public ParseException(int pos, String message, Throwable cause) {
    super(message, cause);
    m_pos = pos;
  }

  public ParseException(int pos, Throwable cause) {
    super(cause);
    m_pos = pos;
  }

  public int getPosition() {
    return m_pos;
  }

  @Override
  public String toString() {
    return m_pos + ": " + getMessage();
  }
}
