/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal;

import java.util.ArrayList;
import java.util.List;

public class HALReporter {
  @SuppressWarnings("MemberName")
  static class ReportStore {
    public int resource;
    public int instanceNumber;
    public int context;
    public String feature;

    ReportStore(int resource, int instanceNumber, int context, String feature) {
      this.resource = resource;
      this.instanceNumber = instanceNumber;
      this.context = context;
      this.feature = feature;
    }
  }
  
  private HALReporter() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  private static final List<ReportStore> storeList = new ArrayList<>();

  private static final Object storeLock = new Object();
  private static boolean allowDirectWrite;

  /**
   * Write all caches reports to the low level API.
   *
   * <p>Not for public use, called from WPILibJ.
   */
  public static void writeReports() {
    synchronized (storeLock) {
      if (allowDirectWrite) {
        return;
      }
      allowDirectWrite = true;
    }
    for (ReportStore r : storeList) {
      HAL.report(r.resource, r.instanceNumber, r.context, r.feature);
    }
  }

  /**
   * Report a usage.
   *
   * @param resource resource
   * @param instanceNumber instanceNumber
   */
  public static void report(int resource, int instanceNumber) {
    report(resource, instanceNumber, 0, "");
  }

  /**
   * Report a usage.
   *
   * @param resource resource
   * @param instanceNumber instanceNumber
   * @param context context
   */
  public static void report(int resource, int instanceNumber, int context) {
    report(resource, instanceNumber, context, "");
  }

  /**
   * Report a usage.
   *
   * @param resource resource
   * @param instanceNumber instanceNumber
   * @param context context
   * @param feature feature
   * @return
   */
  public static int report(int resource, int instanceNumber, int context, String feature) {
    synchronized (storeLock) {
      if (!allowDirectWrite) {
        storeList.add(new ReportStore(resource, instanceNumber, context, feature));
        return 0;
      }
    }
    return HAL.report(resource, instanceNumber, context, feature);
  }
}
