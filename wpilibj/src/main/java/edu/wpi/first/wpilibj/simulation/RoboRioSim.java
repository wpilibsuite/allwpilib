// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.hal.simulation.RoboRioDataJNI;

/** Class to control a simulated RoboRIO. */
@SuppressWarnings({"PMD.ExcessivePublicCount", "PMD.UseUtilityClass"})
public class RoboRioSim {
  /**
   * Register a callback to be run when the FPGA button state changes.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  @SuppressWarnings("AbbreviationAsWordInName")
  public static CallbackStore registerFPGAButtonCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerFPGAButtonCallback(callback, initialNotify);
    return new CallbackStore(uid, RoboRioDataJNI::cancelFPGAButtonCallback);
  }

  /**
   * Query the state of the FPGA button.
   *
   * @return the FPGA button state
   */
  @SuppressWarnings("AbbreviationAsWordInName")
  public static boolean getFPGAButton() {
    return RoboRioDataJNI.getFPGAButton();
  }

  /**
   * Define the state of the FPGA button.
   *
   * @param fpgaButton the new state
   */
  @SuppressWarnings("AbbreviationAsWordInName")
  public static void setFPGAButton(boolean fpgaButton) {
    RoboRioDataJNI.setFPGAButton(fpgaButton);
  }

  /**
   * Register a callback to be run whenever the Vin voltage changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
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
  @SuppressWarnings("ParameterName")
  public static void setVInVoltage(double vInVoltage) {
    RoboRioDataJNI.setVInVoltage(vInVoltage);
  }

  /**
   * Register a callback to be run whenever the Vin current changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
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
  @SuppressWarnings("ParameterName")
  public static void setVInCurrent(double vInCurrent) {
    RoboRioDataJNI.setVInCurrent(vInCurrent);
  }

  /**
   * Register a callback to be run whenever the 6V rail voltage changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public static CallbackStore registerUserVoltage6VCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerUserVoltage6VCallback(callback, initialNotify);
    return new CallbackStore(uid, RoboRioDataJNI::cancelUserVoltage6VCallback);
  }

  /**
   * Measure the 6V rail voltage.
   *
   * @return the 6V rail voltage
   */
  public static double getUserVoltage6V() {
    return RoboRioDataJNI.getUserVoltage6V();
  }

  /**
   * Define the 6V rail voltage.
   *
   * @param userVoltage6V the new voltage
   */
  public static void setUserVoltage6V(double userVoltage6V) {
    RoboRioDataJNI.setUserVoltage6V(userVoltage6V);
  }

  /**
   * Register a callback to be run whenever the 6V rail current changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public static CallbackStore registerUserCurrent6VCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerUserCurrent6VCallback(callback, initialNotify);
    return new CallbackStore(uid, RoboRioDataJNI::cancelUserCurrent6VCallback);
  }

  /**
   * Measure the 6V rail current.
   *
   * @return the 6V rail current
   */
  public static double getUserCurrent6V() {
    return RoboRioDataJNI.getUserCurrent6V();
  }

  /**
   * Define the 6V rail current.
   *
   * @param userCurrent6V the new current
   */
  public static void setUserCurrent6V(double userCurrent6V) {
    RoboRioDataJNI.setUserCurrent6V(userCurrent6V);
  }

  /**
   * Register a callback to be run whenever the 6V rail active state changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public static CallbackStore registerUserActive6VCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerUserActive6VCallback(callback, initialNotify);
    return new CallbackStore(uid, RoboRioDataJNI::cancelUserActive6VCallback);
  }

  /**
   * Get the 6V rail active state.
   *
   * @return true if the 6V rail is active
   */
  public static boolean getUserActive6V() {
    return RoboRioDataJNI.getUserActive6V();
  }

  /**
   * Set the 6V rail active state.
   *
   * @param userActive6V true to make rail active
   */
  public static void setUserActive6V(boolean userActive6V) {
    RoboRioDataJNI.setUserActive6V(userActive6V);
  }

  /**
   * Register a callback to be run whenever the 5V rail voltage changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public static CallbackStore registerUserVoltage5VCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerUserVoltage5VCallback(callback, initialNotify);
    return new CallbackStore(uid, RoboRioDataJNI::cancelUserVoltage5VCallback);
  }

  /**
   * Measure the 5V rail voltage.
   *
   * @return the 5V rail voltage
   */
  public static double getUserVoltage5V() {
    return RoboRioDataJNI.getUserVoltage5V();
  }

  /**
   * Define the 5V rail voltage.
   *
   * @param userVoltage5V the new voltage
   */
  public static void setUserVoltage5V(double userVoltage5V) {
    RoboRioDataJNI.setUserVoltage5V(userVoltage5V);
  }

  /**
   * Register a callback to be run whenever the 5V rail current changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public static CallbackStore registerUserCurrent5VCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerUserCurrent5VCallback(callback, initialNotify);
    return new CallbackStore(uid, RoboRioDataJNI::cancelUserCurrent5VCallback);
  }

  /**
   * Measure the 5V rail current.
   *
   * @return the 5V rail current
   */
  public static double getUserCurrent5V() {
    return RoboRioDataJNI.getUserCurrent5V();
  }

  /**
   * Define the 5V rail current.
   *
   * @param userCurrent5V the new current
   */
  public static void setUserCurrent5V(double userCurrent5V) {
    RoboRioDataJNI.setUserCurrent5V(userCurrent5V);
  }

  /**
   * Register a callback to be run whenever the 5V rail active state changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the initial state
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public static CallbackStore registerUserActive5VCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerUserActive5VCallback(callback, initialNotify);
    return new CallbackStore(uid, RoboRioDataJNI::cancelUserActive5VCallback);
  }

  /**
   * Get the 5V rail active state.
   *
   * @return true if the 5V rail is active
   */
  public static boolean getUserActive5V() {
    return RoboRioDataJNI.getUserActive5V();
  }

  /**
   * Set the 5V rail active state.
   *
   * @param userActive5V true to make rail active
   */
  public static void setUserActive5V(boolean userActive5V) {
    RoboRioDataJNI.setUserActive5V(userActive5V);
  }

  /**
   * Register a callback to be run whenever the 3.3V rail voltage changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
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
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
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
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
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
   * Register a callback to be run whenever the 6V rail number of faults changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public static CallbackStore registerUserFaults6VCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerUserFaults6VCallback(callback, initialNotify);
    return new CallbackStore(uid, RoboRioDataJNI::cancelUserFaults6VCallback);
  }

  /**
   * Get the 6V rail number of faults.
   *
   * @return number of faults
   */
  public static int getUserFaults6V() {
    return RoboRioDataJNI.getUserFaults6V();
  }

  /**
   * Set the 6V rail number of faults.
   *
   * @param userFaults6V number of faults
   */
  public static void setUserFaults6V(int userFaults6V) {
    RoboRioDataJNI.setUserFaults6V(userFaults6V);
  }

  /**
   * Register a callback to be run whenever the 5V rail number of faults changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public static CallbackStore registerUserFaults5VCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerUserFaults5VCallback(callback, initialNotify);
    return new CallbackStore(uid, RoboRioDataJNI::cancelUserFaults5VCallback);
  }

  /**
   * Get the 5V rail number of faults.
   *
   * @return number of faults
   */
  public static int getUserFaults5V() {
    return RoboRioDataJNI.getUserFaults5V();
  }

  /**
   * Set the 5V rail number of faults.
   *
   * @param userFaults5V number of faults
   */
  public static void setUserFaults5V(int userFaults5V) {
    RoboRioDataJNI.setUserFaults5V(userFaults5V);
  }

  /**
   * Register a callback to be run whenever the 3.3V rail number of faults changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
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

  /** Reset all simulation data. */
  public static void resetData() {
    RoboRioDataJNI.resetData();
  }
}
