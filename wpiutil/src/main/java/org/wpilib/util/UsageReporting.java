// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util;

import java.util.function.BiConsumer;

/** Utility functions for reporting resource usage. */
public final class UsageReporting {
  private static final BiConsumer<String, String> DEFAULT_REPORT_USAGE = (resource, data) -> {};
  private static volatile BiConsumer<String, String> reportUsageImpl = DEFAULT_REPORT_USAGE;

  private UsageReporting() {}

  /**
   * Reports usage of a resource of interest. Repeated calls for the same resource name replace the
   * previous report.
   *
   * @param resource the used resource name
   * @param data arbitrary associated data string
   */
  public static void reportUsage(String resource, String data) {
    reportUsageImpl.accept(resource, data);
  }

  /**
   * Reports usage of a resource of interest. Repeated calls for the same resource name replace the
   * previous report.
   *
   * @param resource the used resource name
   * @param instanceNumber an index that identifies the resource instance
   * @param data arbitrary associated data string
   */
  public static void reportUsage(String resource, int instanceNumber, String data) {
    reportUsage(resource + "[" + instanceNumber + "]", data);
  }

  /**
   * Sets the implementation used by {@link #reportUsage(String, String)}.
   *
   * @param func function called by reportUsage(); pass null to restore the default no-op
   *     implementation
   */
  public static synchronized void setReportUsageImpl(BiConsumer<String, String> func) {
    reportUsageImpl = func == null ? DEFAULT_REPORT_USAGE : func;
  }
}
