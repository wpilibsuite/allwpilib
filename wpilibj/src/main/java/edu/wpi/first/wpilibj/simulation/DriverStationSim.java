/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.AllianceStationID;
import edu.wpi.first.hal.simulation.DriverStationDataJNI;
import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.wpilibj.DriverStation;

/**
 * Class to control a simulated driver station.
 */
@SuppressWarnings({"PMD.UseUtilityClass", "PMD.GodClass", "PMD.ExcessivePublicCount"})
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

  public static CallbackStore registerAllianceStationIdCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DriverStationDataJNI.registerAllianceStationIdCallback(callback, initialNotify);
    return new CallbackStore(uid, DriverStationDataJNI::cancelAllianceStationIdCallback);
  }
  public static AllianceStationID getAllianceStationId() {
    switch (DriverStationDataJNI.getAllianceStationId()) {
      case 0:
        return AllianceStationID.Red1;
      case 1:
        return AllianceStationID.Red2;
      case 2:
        return AllianceStationID.Red3;
      case 3:
        return AllianceStationID.Blue1;
      case 4:
        return AllianceStationID.Blue2;
      case 5:
        return AllianceStationID.Blue3;
      default:
        return null;
    }
  }
  public static void setAllianceStationId(AllianceStationID allianceStationId) {
    int allianceStation;
    switch (allianceStationId) {
      case Red1:
        allianceStation = 0;
        break;
      case Red2:
        allianceStation = 1;
        break;
      case Red3:
        allianceStation = 2;
        break;
      case Blue1:
        allianceStation = 3;
        break;
      case Blue2:
        allianceStation = 4;
        break;
      case Blue3:
        allianceStation = 5;
        break;
      default:
        return;
    }
    DriverStationDataJNI.setAllianceStationId(allianceStation);
  }

  public static CallbackStore registerMatchTimeCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = DriverStationDataJNI.registerMatchTimeCallback(callback, initialNotify);
    return new CallbackStore(uid, DriverStationDataJNI::cancelMatchTimeCallback);
  }
  public static double getMatchTime() {
    return DriverStationDataJNI.getMatchTime();
  }
  public static void setMatchTime(double matchTime) {
    DriverStationDataJNI.setMatchTime(matchTime);
  }

  /**
   * Updates DriverStation data so that new values are visible to the user
   * program.
   */
  public static void notifyNewData() {
    DriverStationDataJNI.notifyNewData();
    DriverStation.getInstance().waitForData();
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
  public static long getJoystickOutputs(int stick) {
    return DriverStationDataJNI.getJoystickOutputs(stick);
  }

  /**
   * Gets the joystick rumble.
   *
   * @param stick The joystick number
   * @param rumbleNum Rumble to get (0=left, 1=right)
   * @return The joystick rumble value
   */
  public static int getJoystickRumble(int stick, int rumbleNum) {
    return DriverStationDataJNI.getJoystickRumble(stick, rumbleNum);
  }

  /**
   * Sets the state of one joystick button. Button indexes begin at 1.
   *
   * @param stick  The joystick number
   * @param button The button index, beginning at 1
   * @param state The state of the joystick button
   */
  public static void setJoystickButton(int stick, int button, boolean state) {
    DriverStationDataJNI.setJoystickButton(stick, button, state);
  }

  /**
   * Gets the value of the axis on a joystick.
   *
   * @param stick The joystick number
   * @param axis  The analog axis number
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
   * @param value the angle of the POV in degrees, or -1 for not pressed
   */
  public static void setJoystickPOV(int stick, int pov, int value) {
    DriverStationDataJNI.setJoystickPOV(stick, pov, value);
  }

  /**
   * Sets the state of all the buttons on a joystick.
   *
   * @param stick The joystick number
   * @param buttons The bitmap state of the buttons on the joystick
   */
  public static void setJoystickButtons(int stick, int buttons) {
    DriverStationDataJNI.setJoystickButtonsValue(stick, buttons);
  }

  /**
   * Sets the number of axes for a joystick.
   *
   * @param stick The joystick number
   * @param count The number of axes on the indicated joystick
   */
  public static void setJoystickAxisCount(int stick, int count) {
    DriverStationDataJNI.setJoystickAxisCount(stick, count);
  }

  /**
   * Sets the number of POVs for a joystick.
   *
   * @param stick The joystick number
   * @param count The number of POVs on the indicated joystick
   */
  public static void setJoystickPOVCount(int stick, int count) {
    DriverStationDataJNI.setJoystickPOVCount(stick, count);
  }

  /**
   * Sets the number of buttons for a joystick.
   *
   * @param stick The joystick number
   * @param count The number of buttons on the indicated joystick
   */
  public static void setJoystickButtonCount(int stick, int count) {
    DriverStationDataJNI.setJoystickButtonCount(stick, count);
  }

  /**
   * Sets the value of isXbox for a joystick.
   *
   * @param stick The joystick number
   * @param isXbox The value of isXbox
   */
  public static void setJoystickIsXbox(int stick, boolean isXbox) {
    DriverStationDataJNI.setJoystickIsXbox(stick, isXbox);
  }

  /**
   * Sets the value of type for a joystick.
   *
   * @param stick The joystick number
   * @param type The value of type
   */
  public static void setJoystickType(int stick, int type) {
    DriverStationDataJNI.setJoystickType(stick, type);
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
   * Sets the types of Axes for a joystick.
   *
   * @param stick The joystick number
   * @param axis The target axis
   * @param type The type of axis
   */
  public static void setJoystickAxisType(int stick, int axis, int type) {
    DriverStationDataJNI.setJoystickAxisType(stick, axis, type);
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
    int matchType;
    switch (type) {
      case Practice:
        matchType = 1;
        break;
      case Qualification:
        matchType = 2;
        break;
      case Elimination:
        matchType = 3;
        break;
      case None:
        matchType = 0;
        break;
      default:
        return;
    }
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

  public static void resetData() {
    DriverStationDataJNI.resetData();
  }
}
