// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue;

import edu.wpi.first.epilogue.logging.EpilogueBackend;
import edu.wpi.first.epilogue.logging.NTEpilogueBackend;
import edu.wpi.first.epilogue.logging.errors.ErrorHandler;
import edu.wpi.first.epilogue.logging.errors.ErrorPrinter;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.units.measure.Time;

/**
 * A configuration object to be used by the generated {@code Epilogue} class to customize its
 * behavior.
 */
@SuppressWarnings("checkstyle:MemberName")
public class EpilogueConfiguration {
  /**
   * The backend implementation for Epilogue to use. By default, this will log data directly to
   * NetworkTables. NetworkTable data can be mirrored to a log file on disk by calling {@code
   * DataLogManager.start()} in your {@code robotInit} method.
   */
  public EpilogueBackend backend = new NTEpilogueBackend(NetworkTableInstance.getDefault());

  /**
   * The period Epilogue will log at. By default this is the period that the robot runs at. This is
   * the field used by bind to configure speed when adding the periodic logging function
   */
  public Time loggingPeriod;

  /**
   * The offset from the periodic run that Epilogue will log at. By default this will be half of the
   * robots period. This is the field used by bind when adding the periodic logging function
   */
  public Time loggingPeriodOffset;

  /**
   * The minimum importance level of data to be logged. Defaults to debug, which logs data of all
   * importance levels. Any data tagged with an importance level lower than this will not be logged.
   */
  public Logged.Importance minimumImportance = Logged.Importance.DEBUG;

  /**
   * The error handler for loggers to use if they encounter an error while logging. Defaults to
   * printing an error to the standard output.
   */
  public ErrorHandler errorHandler = new ErrorPrinter();

  /**
   * The root identifier to use for all logged data. Defaults to "Robot", but can be changed to any
   * string.
   */
  public String root = "Robot";

  /** Default constructor. */
  public EpilogueConfiguration() {}
}
