// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.simulation;

import org.wpilib.driverstation.DriverStation;
import org.wpilib.hardware.hal.AllianceStationID;
import org.wpilib.hardware.hal.DriverStationJNI;
import org.wpilib.hardware.hal.simulation.DriverStationDataJNI;
import org.wpilib.hardware.hal.simulation.NotifyCallback;
import org.wpilib.util.WPIUtilJNI;

/** Class to control a simulated driver station. */
public final class DriverStationSim {
  private DriverStationSim() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Register a callback on whether the DS is enabled.
   *
   * @param callback the callback that will be called whenever the enabled state is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public static CallbackStore registerEnabledCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = DriverStationDataJNI.registerEnabledCallback(callback, initialNotify);
    return new CallbackStore(uid, DriverStationDataJNI::cancelEnabledCallback);
  }

  /**
   * Check if the DS is enabled.
   *
   * @return true if enabled
   */
  public static boolean getEnabled() {
    return DriverStationDataJNI.getEnabled();
  }

  /**
   * Change whether the DS is enabled.
   *
   * @param enabled the new value
   */
  public static void setEnabled(boolean enabled) {
    DriverStationDataJNI.setEnabled(enabled);
  }

  /**
   * Register a callback on whether the DS is in autonomous mode.
   *
   * @param callback the callback that will be called on autonomous mode entrance/exit
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public static CallbackStore registerAutonomousCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = DriverStationDataJNI.registerAutonomousCallback(callback, initialNotify);
    return new CallbackStore(uid, DriverStationDataJNI::cancelAutonomousCallback);
  }

  /**
   * Check if the DS is in autonomous.
   *
   * @return true if autonomous
   */
  public static boolean getAutonomous() {
    return DriverStationDataJNI.getAutonomous();
  }

  /**
   * Change whether the DS is in autonomous.
   *
   * @param autonomous the new value
   */
  public static void setAutonomous(boolean autonomous) {
    DriverStationDataJNI.setAutonomous(autonomous);
  }

  /**
   * Register a callback on whether the DS is in test mode.
   *
   * @param callback the callback that will be called whenever the test mode is entered or left
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public static CallbackStore registerTestCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DriverStationDataJNI.registerTestCallback(callback, initialNotify);
    return new CallbackStore(uid, DriverStationDataJNI::cancelTestCallback);
  }

  /**
   * Check if the DS is in test.
   *
   * @return true if test
   */
  public static boolean getTest() {
    return DriverStationDataJNI.getTest();
  }

  /**
   * Change whether the DS is in test.
   *
   * @param test the new value
   */
  public static void setTest(boolean test) {
    DriverStationDataJNI.setTest(test);
  }

  /**
   * Register a callback on the eStop state.
   *
   * @param callback the callback that will be called whenever the eStop state changes
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public static CallbackStore registerEStopCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = DriverStationDataJNI.registerEStopCallback(callback, initialNotify);
    return new CallbackStore(uid, DriverStationDataJNI::cancelEStopCallback);
  }

  /**
   * Check if eStop has been activated.
   *
   * @return true if eStopped
   */
  public static boolean getEStop() {
    return DriverStationDataJNI.getEStop();
  }

  /**
   * Set whether eStop is active.
   *
   * @param eStop true to activate
   */
  public static void setEStop(boolean eStop) {
    DriverStationDataJNI.setEStop(eStop);
  }

  /**
   * Register a callback on whether the FMS is connected.
   *
   * @param callback the callback that will be called whenever the FMS connection changes
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public static CallbackStore registerFmsAttachedCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = DriverStationDataJNI.registerFmsAttachedCallback(callback, initialNotify);
    return new CallbackStore(uid, DriverStationDataJNI::cancelFmsAttachedCallback);
  }

  /**
   * Check if the FMS is connected.
   *
   * @return true if FMS is connected
   */
  public static boolean getFmsAttached() {
    return DriverStationDataJNI.getFmsAttached();
  }

  /**
   * Change whether the FMS is connected.
   *
   * @param fmsAttached the new value
   */
  public static void setFmsAttached(boolean fmsAttached) {
    DriverStationDataJNI.setFmsAttached(fmsAttached);
  }

  /**
   * Register a callback on whether the DS is connected.
   *
   * @param callback the callback that will be called whenever the DS connection changes
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public static CallbackStore registerDsAttachedCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = DriverStationDataJNI.registerDsAttachedCallback(callback, initialNotify);
    return new CallbackStore(uid, DriverStationDataJNI::cancelDsAttachedCallback);
  }

  /**
   * Check if the DS is attached.
   *
   * @return true if attached
   */
  public static boolean getDsAttached() {
    return DriverStationDataJNI.getDsAttached();
  }

  /**
   * Change whether the DS is attached.
   *
   * @param dsAttached the new value
   */
  public static void setDsAttached(boolean dsAttached) {
    DriverStationDataJNI.setDsAttached(dsAttached);
  }

  /**
   * Register a callback on the alliance station ID.
   *
   * @param callback the callback that will be called whenever the alliance station changes
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public static CallbackStore registerAllianceStationIdCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = DriverStationDataJNI.registerAllianceStationIdCallback(callback, initialNotify);
    return new CallbackStore(uid, DriverStationDataJNI::cancelAllianceStationIdCallback);
  }

  /**
   * Get the alliance station ID (color + number).
   *
   * @return the alliance station color and number
   */
  public static AllianceStationID getAllianceStationId() {
    return switch (DriverStationDataJNI.getAllianceStationId()) {
      case DriverStationJNI.kUnknownAllianceStation -> AllianceStationID.Unknown;
      case DriverStationJNI.kRed1AllianceStation -> AllianceStationID.Red1;
      case DriverStationJNI.kRed2AllianceStation -> AllianceStationID.Red2;
      case DriverStationJNI.kRed3AllianceStation -> AllianceStationID.Red3;
      case DriverStationJNI.kBlue1AllianceStation -> AllianceStationID.Blue1;
      case DriverStationJNI.kBlue2AllianceStation -> AllianceStationID.Blue2;
      case DriverStationJNI.kBlue3AllianceStation -> AllianceStationID.Blue3;
      default -> AllianceStationID.Unknown;
    };
  }

  /**
   * Change the alliance station.
   *
   * @param allianceStationId the new alliance station
   */
  public static void setAllianceStationId(AllianceStationID allianceStationId) {
    int allianceStation =
        switch (allianceStationId) {
          case Unknown -> DriverStationJNI.kUnknownAllianceStation;
          case Red1 -> DriverStationJNI.kRed1AllianceStation;
          case Red2 -> DriverStationJNI.kRed2AllianceStation;
          case Red3 -> DriverStationJNI.kRed3AllianceStation;
          case Blue1 -> DriverStationJNI.kBlue1AllianceStation;
          case Blue2 -> DriverStationJNI.kBlue2AllianceStation;
          case Blue3 -> DriverStationJNI.kBlue3AllianceStation;
        };
    DriverStationDataJNI.setAllianceStationId(allianceStation);
  }

  /**
   * Register a callback on match time.
   *
   * @param callback the callback that will be called whenever match time changes
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback.
   */
  public static CallbackStore registerMatchTimeCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = DriverStationDataJNI.registerMatchTimeCallback(callback, initialNotify);
    return new CallbackStore(uid, DriverStationDataJNI::cancelMatchTimeCallback);
  }

  /**
   * Get the current value of the match timer.
   *
   * @return the current match time
   */
  public static double getMatchTime() {
    return DriverStationDataJNI.getMatchTime();
  }

  /**
   * Sets the match timer.
   *
   * @param matchTime the new match time
   */
  public static void setMatchTime(double matchTime) {
    DriverStationDataJNI.setMatchTime(matchTime);
  }

  /** Updates DriverStation data so that new values are visible to the user program. */
  public static void notifyNewData() {
    int handle = WPIUtilJNI.createEvent(false, false);
    DriverStationJNI.provideNewDataEventHandle(handle);
    DriverStationDataJNI.notifyNewData();
    try {
      WPIUtilJNI.waitForObject(handle);
    } catch (InterruptedException e) {
      e.printStackTrace();
    }
    DriverStationJNI.removeNewDataEventHandle(handle);
    WPIUtilJNI.destroyEvent(handle);
    DriverStation.refreshData();
  }

  /**
   * Sets suppression of DriverStation.reportError and reportWarning messages.
   *
   * @param shouldSend If false then messages will be suppressed.
   */
  public static void setSendError(boolean shouldSend) {
    DriverStationDataJNI.setSendError(shouldSend);
  }

  /**
   * Sets suppression of DriverStation.sendConsoleLine messages.
   *
   * @param shouldSend If false then messages will be suppressed.
   */
  public static void setSendConsoleLine(boolean shouldSend) {
    DriverStationDataJNI.setSendConsoleLine(shouldSend);
  }

  /**
   * Gets the joystick outputs.
   *
   * @param stick The joystick number
   * @return The joystick outputs
   */
  public static int getJoystickLeds(int stick) {
    return DriverStationDataJNI.getJoystickLeds(stick);
  }

  /**
   * Gets the joystick rumble.
   *
   * @param stick The joystick number
   * @param rumbleNum Rumble to get (0=left, 1=right, 2=left trigger, 3=right trigger)
   * @return The joystick rumble value
   */
  public static int getJoystickRumble(int stick, int rumbleNum) {
    return DriverStationDataJNI.getJoystickRumble(stick, rumbleNum);
  }

  /**
   * Sets the state of one joystick button. Button indexes begin at 0.
   *
   * @param stick The joystick number
   * @param button The button index, beginning at 0
   * @param state The state of the joystick button
   */
  public static void setJoystickButton(int stick, int button, boolean state) {
    DriverStationDataJNI.setJoystickButton(stick, button, state);
  }

  /**
   * Gets the value of the axis on a joystick.
   *
   * @param stick The joystick number
   * @param axis The analog axis number
   * @param value The value of the axis on the joystick
   */
  public static void setJoystickAxis(int stick, int axis, double value) {
    DriverStationDataJNI.setJoystickAxis(stick, axis, value);
  }

  /**
   * Gets the state of a POV on a joystick.
   *
   * @param stick The joystick number
   * @param pov The POV number
   * @param value the angle of the POV
   */
  public static void setJoystickPOV(int stick, int pov, DriverStation.POVDirection value) {
    DriverStationDataJNI.setJoystickPOV(stick, pov, value.value);
  }

  /**
   * Sets the maximum index that an axis is available at.
   *
   * @param stick The joystick number
   * @param maximumIndex The maximum index an axis is available at.
   */
  public static void setJoystickAxesMaximumIndex(int stick, int maximumIndex) {
    setJoystickAxesAvailable(stick, (1 << maximumIndex) - 1);
  }

  /**
   * Sets the number of axes for a joystick.
   *
   * @param stick The joystick number
   * @param count The number of axes on the indicated joystick
   */
  public static void setJoystickAxesAvailable(int stick, int count) {
    DriverStationDataJNI.setJoystickAxesAvailable(stick, count);
  }

  /**
   * Sets the maximum index that a pov is available at.
   *
   * @param stick The joystick number
   * @param maximumIndex The maximum index a pov is available at.
   */
  public static void setJoystickPOVsMaximumIndex(int stick, int maximumIndex) {
    setJoystickPOVsAvailable(stick, (1 << maximumIndex) - 1);
  }

  /**
   * Sets the number of POVs for a joystick.
   *
   * @param stick The joystick number
   * @param count The number of POVs on the indicated joystick
   */
  public static void setJoystickPOVsAvailable(int stick, int count) {
    DriverStationDataJNI.setJoystickPOVsAvailable(stick, count);
  }

  /**
   * Sets the maximum index that a button is available at.
   *
   * @param stick The joystick number
   * @param maximumIndex The maximum index a button is available at.
   */
  public static void setJoystickButtonsMaximumIndex(int stick, int maximumIndex) {
    if (maximumIndex >= 64) {
      setJoystickButtonsAvailable(stick, 0xFFFFFFFFFFFFFFFFL);
    } else {
      setJoystickButtonsAvailable(stick, (1L << maximumIndex) - 1);
    }
  }

  /**
   * Sets the number of buttons for a joystick.
   *
   * @param stick The joystick number
   * @param count The number of buttons on the indicated joystick
   */
  public static void setJoystickButtonsAvailable(int stick, long count) {
    DriverStationDataJNI.setJoystickButtonsAvailable(stick, count);
  }

  /**
   * Sets the value of isGamepad for a joystick.
   *
   * @param stick The joystick number
   * @param isGamepad The value of isGamepad
   */
  public static void setJoystickIsGamepad(int stick, boolean isGamepad) {
    DriverStationDataJNI.setJoystickIsGamepad(stick, isGamepad);
  }

  /**
   * Sets the value of type for a joystick.
   *
   * @param stick The joystick number
   * @param type The value of type
   */
  public static void setJoystickGamepadType(int stick, int type) {
    DriverStationDataJNI.setJoystickGamepadType(stick, type);
  }

  /**
   * Sets the value of supported outputs for a joystick.
   *
   * @param stick The joystick number
   * @param supportedOutputs The value of supported outputs
   */
  public static void setJoystickSupportedOutputs(int stick, int supportedOutputs) {
    DriverStationDataJNI.setJoystickSupportedOutputs(stick, supportedOutputs);
  }

  /**
   * Sets the name of a joystick.
   *
   * @param stick The joystick number
   * @param name The value of name
   */
  public static void setJoystickName(int stick, String name) {
    DriverStationDataJNI.setJoystickName(stick, name);
  }

  /**
   * Sets the game specific message.
   *
   * @param message the game specific message
   */
  public static void setGameSpecificMessage(String message) {
    DriverStationDataJNI.setGameSpecificMessage(message);
  }

  /**
   * Sets the event name.
   *
   * @param name the event name
   */
  public static void setEventName(String name) {
    DriverStationDataJNI.setEventName(name);
  }

  /**
   * Sets the match type.
   *
   * @param type the match type
   */
  public static void setMatchType(DriverStation.MatchType type) {
    int matchType =
        switch (type) {
          case Practice -> 1;
          case Qualification -> 2;
          case Elimination -> 3;
          case None -> 0;
        };
    DriverStationDataJNI.setMatchType(matchType);
  }

  /**
   * Sets the match number.
   *
   * @param matchNumber the match number
   */
  public static void setMatchNumber(int matchNumber) {
    DriverStationDataJNI.setMatchNumber(matchNumber);
  }

  /**
   * Sets the replay number.
   *
   * @param replayNumber the replay number
   */
  public static void setReplayNumber(int replayNumber) {
    DriverStationDataJNI.setReplayNumber(replayNumber);
  }

  /** Reset all simulation data for the Driver Station. */
  public static void resetData() {
    DriverStationDataJNI.resetData();
  }
}
