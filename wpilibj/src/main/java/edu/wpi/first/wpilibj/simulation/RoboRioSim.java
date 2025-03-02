// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.hal.simulation.RoboRioDataJNI;
import edu.wpi.first.wpilibj.RobotController;

/** A utility class to control a simulated RoboRIO. */
public final class RoboRioSim {
  private RoboRioSim() {
    // Utility class
  }

  /**
   * Register a callback to be run whenever the Vin voltage changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public static CallbackStore registerVInVoltageCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerVInVoltageCallback(callback, initialNotify);
    return new CallbackStore(uid, RoboRioDataJNI::cancelVInVoltageCallback);
  }

  /**
   * Measure the Vin voltage.
   *
   * @return the Vin voltage
   */
  public static double getVInVoltage() {
    return RoboRioDataJNI.getVInVoltage();
  }

  /**
   * Define the Vin voltage.
   *
   * @param vInVoltage the new voltage
   */
  public static void setVInVoltage(double vInVoltage) {
    RoboRioDataJNI.setVInVoltage(vInVoltage);
  }

  /**
   * Register a callback to be run whenever the Vin current changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public static CallbackStore registerVInCurrentCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerVInCurrentCallback(callback, initialNotify);
    return new CallbackStore(uid, RoboRioDataJNI::cancelVInCurrentCallback);
  }

  /**
   * Measure the Vin current.
   *
   * @return the Vin current
   */
  public static double getVInCurrent() {
    return RoboRioDataJNI.getVInCurrent();
  }

  /**
   * Define the Vin current.
   *
   * @param vInCurrent the new current
   */
  public static void setVInCurrent(double vInCurrent) {
    RoboRioDataJNI.setVInCurrent(vInCurrent);
  }

  /**
   * Register a callback to be run whenever the 3.3V rail voltage changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public static CallbackStore registerUserVoltage3V3Callback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerUserVoltage3V3Callback(callback, initialNotify);
    return new CallbackStore(uid, RoboRioDataJNI::cancelUserVoltage3V3Callback);
  }

  /**
   * Measure the 3.3V rail voltage.
   *
   * @return the 3.3V rail voltage
   */
  public static double getUserVoltage3V3() {
    return RoboRioDataJNI.getUserVoltage3V3();
  }

  /**
   * Define the 3.3V rail voltage.
   *
   * @param userVoltage3V3 the new voltage
   */
  public static void setUserVoltage3V3(double userVoltage3V3) {
    RoboRioDataJNI.setUserVoltage3V3(userVoltage3V3);
  }

  /**
   * Register a callback to be run whenever the 3.3V rail current changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public static CallbackStore registerUserCurrent3V3Callback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerUserCurrent3V3Callback(callback, initialNotify);
    return new CallbackStore(uid, RoboRioDataJNI::cancelUserCurrent3V3Callback);
  }

  /**
   * Measure the 3.3V rail current.
   *
   * @return the 3.3V rail current
   */
  public static double getUserCurrent3V3() {
    return RoboRioDataJNI.getUserCurrent3V3();
  }

  /**
   * Define the 3.3V rail current.
   *
   * @param userCurrent3V3 the new current
   */
  public static void setUserCurrent3V3(double userCurrent3V3) {
    RoboRioDataJNI.setUserCurrent3V3(userCurrent3V3);
  }

  /**
   * Register a callback to be run whenever the 3.3V rail active state changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the initial state
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public static CallbackStore registerUserActive3V3Callback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerUserActive3V3Callback(callback, initialNotify);
    return new CallbackStore(uid, RoboRioDataJNI::cancelUserActive3V3Callback);
  }

  /**
   * Get the 3.3V rail active state.
   *
   * @return true if the 3.3V rail is active
   */
  public static boolean getUserActive3V3() {
    return RoboRioDataJNI.getUserActive3V3();
  }

  /**
   * Set the 3.3V rail active state.
   *
   * @param userActive3V3 true to make rail active
   */
  public static void setUserActive3V3(boolean userActive3V3) {
    RoboRioDataJNI.setUserActive3V3(userActive3V3);
  }

  /**
   * Register a callback to be run whenever the 3.3V rail number of faults changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public static CallbackStore registerUserFaults3V3Callback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerUserFaults3V3Callback(callback, initialNotify);
    return new CallbackStore(uid, RoboRioDataJNI::cancelUserFaults3V3Callback);
  }

  /**
   * Get the 3.3V rail number of faults.
   *
   * @return number of faults
   */
  public static int getUserFaults3V3() {
    return RoboRioDataJNI.getUserFaults3V3();
  }

  /**
   * Set the 3.3V rail number of faults.
   *
   * @param userFaults3V3 number of faults
   */
  public static void setUserFaults3V3(int userFaults3V3) {
    RoboRioDataJNI.setUserFaults3V3(userFaults3V3);
  }

  /**
   * Register a callback to be run whenever the Brownout voltage changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public static CallbackStore registerBrownoutVoltageCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerBrownoutVoltageCallback(callback, initialNotify);
    return new CallbackStore(uid, RoboRioDataJNI::cancelBrownoutVoltageCallback);
  }

  /**
   * Measure the Brownout voltage.
   *
   * @return the Brownout voltage
   */
  public static double getBrownoutVoltage() {
    return RoboRioDataJNI.getBrownoutVoltage();
  }

  /**
   * Define the Brownout voltage.
   *
   * @param vInVoltage the new voltage
   */
  public static void setBrownoutVoltage(double vInVoltage) {
    RoboRioDataJNI.setBrownoutVoltage(vInVoltage);
  }

  /**
   * Register a callback to be run whenever the cpu temp changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public static CallbackStore registerCPUTempCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerCPUTempCallback(callback, initialNotify);
    return new CallbackStore(uid, RoboRioDataJNI::cancelCPUTempCallback);
  }

  /**
   * Get the cpu temp.
   *
   * @return the cpu temp.
   */
  public static double getCPUTemp() {
    return RoboRioDataJNI.getCPUTemp();
  }

  /**
   * Set the cpu temp.
   *
   * @param cpuTemp the new cpu temp.
   */
  public static void setCPUTemp(double cpuTemp) {
    RoboRioDataJNI.setCPUTemp(cpuTemp);
  }

  /**
   * Register a callback to be run whenever the team number changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public static CallbackStore registerTeamNumberCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerTeamNumberCallback(callback, initialNotify);
    return new CallbackStore(uid, RoboRioDataJNI::cancelTeamNumberCallback);
  }

  /**
   * Get the team number.
   *
   * @return the team number.
   */
  public static int getTeamNumber() {
    return RoboRioDataJNI.getTeamNumber();
  }

  /**
   * Set the team number.
   *
   * @param teamNumber the new team number.
   */
  public static void setTeamNumber(int teamNumber) {
    RoboRioDataJNI.setTeamNumber(teamNumber);
  }

  /**
   * Get the serial number.
   *
   * @return The serial number.
   */
  public static String getSerialNumber() {
    return RoboRioDataJNI.getSerialNumber();
  }

  /**
   * Set the serial number.
   *
   * @param serialNumber The serial number.
   */
  public static void setSerialNumber(String serialNumber) {
    RoboRioDataJNI.setSerialNumber(serialNumber);
  }

  /**
   * Get the comments string.
   *
   * @return The comments string.
   */
  public static String getComments() {
    return RoboRioDataJNI.getComments();
  }

  /**
   * Set the comments string.
   *
   * @param comments The comments string.
   */
  public static void setComments(String comments) {
    RoboRioDataJNI.setComments(comments);
  }

  /** Reset all simulation data. */
  public static void resetData() {
    RoboRioDataJNI.resetData();
  }
}
