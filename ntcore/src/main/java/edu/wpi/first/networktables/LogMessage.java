/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017-2018. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.networktables;

/**
 * NetworkTables log message.
 */
public final class LogMessage {
  /**
   * Logging levels.
   */
  public static final int kCritical = 50;
  public static final int kError = 40;
  public static final int kWarning = 30;
  public static final int kInfo = 20;
  public static final int kDebug = 10;
  public static final int kDebug1 = 9;
  public static final int kDebug2 = 8;
  public static final int kDebug3 = 7;
  public static final int kDebug4 = 6;

  /**
   * The logger that generated the message.
   */
  public final int logger;

  /**
   * Log level of the message.
   */
  public final int level;

  /**
   * The filename of the source file that generated the message.
   */
  public final String filename;

  /**
   * The line number in the source file that generated the message.
   */
  public final int line;

  /**
   * The message.
   */
  public final String message;

  /** Constructor.
   * This should generally only be used internally to NetworkTables.
   * @param inst Instance
   * @param logger Logger
   * @param level Log level
   * @param filename Filename
   * @param line Line number
   * @param message Message
   */
  public LogMessage(NetworkTableInstance inst, int logger, int level, String filename, int line, String message) {
    this.inst = inst;
    this.logger = logger;
    this.level = level;
    this.filename = filename;
    this.line = line;
    this.message = message;
  }

  private final NetworkTableInstance inst;

  NetworkTableInstance getInstance() {
    return inst;
  }
}
