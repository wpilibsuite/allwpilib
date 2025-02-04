// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/**
 * Usage Reporting JNI Functions.
 *
 * @see "hal/UsageReporting.h"
 */
public class UsageReportingJNI extends JNIWrapper {
  /**
   * Reports usage of a resource of interest.  Repeated calls for the same
   * resource name replace the previous report.
   *
   * @param resource       the used resource name
   * @param data           arbitrary associated data string
   * @return               a handle
   */
  public static native int report(String resource, String data);

  /** Utility class. */
  private UsageReportingJNI() {}
}
