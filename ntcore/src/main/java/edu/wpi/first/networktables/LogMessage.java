// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/** NetworkTables log message. */
@SuppressWarnings("MemberName")
public final class LogMessage {
  /** Critical logging level. */
  public static final int kCritical = 50;

  /** Error logging level. */
  public static final int kError = 40;

  /** Warning log level. */
  public static final int kWarning = 30;

  /** Info log level. */
  public static final int kInfo = 20;

  /** Debug log level. */
  public static final int kDebug = 10;

  /** Debug log level 1. */
  public static final int kDebug1 = 9;

  /** Debug log level 2. */
  public static final int kDebug2 = 8;

  /** Debug log level 3. */
  public static final int kDebug3 = 7;

  /** Debug log level 4. */
  public static final int kDebug4 = 6;

  /** Log level of the message. */
  public final int level;

  /** The filename of the source file that generated the message. */
  public final String filename;

  /** The line number in the source file that generated the message. */
  public final int line;

  /** The message. */
  public final String message;

  /**
   * Constructor. This should generally only be used internally to NetworkTables.
   *
   * @param level Log level
   * @param filename Filename
   * @param line Line number
   * @param message Message
   */
  public LogMessage(int level, String filename, int line, String message) {
    this.level = level;
    this.filename = filename;
    this.line = line;
    this.message = message;
  }
}
