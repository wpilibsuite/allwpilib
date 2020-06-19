/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.DriverStationDataJNI;
import edu.wpi.first.hal.simulation.NotifyCallback;

/**
 * Class to control a simulated driver station.
 */
@SuppressWarnings("PMD.UseUtilityClass")
public class DriverStationSim {
  public static CallbackStore registerEnabledCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DriverStationDataJNI.registerEnabledCallback(callback, initialNotify);
    return new CallbackStore(uid, DriverStationDataJNI::cancelEnabledCallback);
  }
  public static boolean getEnabled() {
    return DriverStationDataJNI.getEnabled();
  }
  public static void setEnabled(boolean enabled) {
    DriverStationDataJNI.setEnabled(enabled);
  }

  public static CallbackStore registerAutonomousCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DriverStationDataJNI.registerAutonomousCallback(callback, initialNotify);
    return new CallbackStore(uid, DriverStationDataJNI::cancelAutonomousCallback);
  }
  public static boolean getAutonomous() {
    return DriverStationDataJNI.getAutonomous();
  }
  public static void setAutonomous(boolean autonomous) {
    DriverStationDataJNI.setAutonomous(autonomous);
  }

  public static CallbackStore registerTestCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DriverStationDataJNI.registerTestCallback(callback, initialNotify);
    return new CallbackStore(uid, DriverStationDataJNI::cancelTestCallback);
  }
  public static boolean getTest() {
    return DriverStationDataJNI.getTest();
  }
  public static void setTest(boolean test) {
    DriverStationDataJNI.setTest(test);
  }

  public static CallbackStore registerEStopCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DriverStationDataJNI.registerEStopCallback(callback, initialNotify);
    return new CallbackStore(uid, DriverStationDataJNI::cancelEStopCallback);
  }
  public static boolean getEStop() {
    return DriverStationDataJNI.getEStop();
  }
  public static void setEStop(boolean eStop) {
    DriverStationDataJNI.setEStop(eStop);
  }

  public static CallbackStore registerFmsAttachedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DriverStationDataJNI.registerFmsAttachedCallback(callback, initialNotify);
    return new CallbackStore(uid, DriverStationDataJNI::cancelFmsAttachedCallback);
  }
  public static boolean getFmsAttached() {
    return DriverStationDataJNI.getFmsAttached();
  }
  public static void setFmsAttached(boolean fmsAttached) {
    DriverStationDataJNI.setFmsAttached(fmsAttached);
  }

  public static CallbackStore registerDsAttachedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DriverStationDataJNI.registerDsAttachedCallback(callback, initialNotify);
    return new CallbackStore(uid, DriverStationDataJNI::cancelDsAttachedCallback);
  }
  public static boolean getDsAttached() {
    return DriverStationDataJNI.getDsAttached();
  }
  public static void setDsAttached(boolean dsAttached) {
    DriverStationDataJNI.setDsAttached(dsAttached);
  }
  public static void notifyNewData() {
    DriverStationDataJNI.notifyNewData();
  }

  /**
   * Toggles suppression of DriverStation.reportError and reportWarning messages.
   *
   * @param shouldSend If false then messages will will be suppressed.
   */
  public static void setSendError(boolean shouldSend) {
    DriverStationDataJNI.setSendError(shouldSend);
  }

  public static void resetData() {
    DriverStationDataJNI.resetData();
  }
}
