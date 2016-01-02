/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.communication;

public class UsageReporting {

  public static void report(int resource, int instanceNumber, int i) {
    report(resource, instanceNumber, i, "");
  }

  public static void report(int resource, int instanceNumber) {
    report(resource, instanceNumber, 0, "");
  }

  public static void report(int resource, int instanceNumber, int i, String string) {
    FRCNetworkCommunicationsLibrary.FRCNetworkCommunicationUsageReportingReport((byte) resource,
        (byte) instanceNumber, (byte) i, string);
  }

}
