// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.struct.parser;

/** Exception for parsing errors. */
public class ParseException extends Exception {
  /** The parser position. */
  private final int m_pos;

  /**
   * Constructs a ParseException.
   *
   * @param pos The parser position.
   * @param s Reason for parse failure.
   */
  public ParseException(int pos, String s) {
    super(s);
    m_pos = pos;
  }

  /**
   * Constructs a ParseException.
   *
   * @param pos The parser position.
   * @param message Reason for parse failure.
   * @param cause Exception that caused the parser failure.
   */
  public ParseException(int pos, String message, Throwable cause) {
    super(message, cause);
    m_pos = pos;
  }

  /**
   * Constructs a ParseException.
   *
   * @param pos The parser position.
   * @param cause Exception that caused the parser failure.
   */
  public ParseException(int pos, Throwable cause) {
    super(cause);
    m_pos = pos;
  }

  /**
   * Returns position in parsed string.
   *
   * @return Position in parsed string.
   */
  public int getPosition() {
    return m_pos;
  }

  @Override
  public String getMessage() {
    return m_pos + ": " + super.getMessage();
  }
}
