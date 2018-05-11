/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.sim;

import edu.wpi.first.hal.sim.mockdata.RoboRioDataJNI;

public class RoboRioSim {
  private int m_index;

  public RoboRioSim(int index) {
    m_index = index;
  }

  public CallbackStore registerFPGAButtonCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerFPGAButtonCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, RoboRioDataJNI::cancelFPGAButtonCallback);
  }
  public boolean getFPGAButton() {
    return RoboRioDataJNI.getFPGAButton(m_index);
  }
  public void setFPGAButton(boolean fPGAButton) {
    RoboRioDataJNI.setFPGAButton(m_index, fPGAButton);
  }

  public CallbackStore registerVInVoltageCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerVInVoltageCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, RoboRioDataJNI::cancelVInVoltageCallback);
  }
  public double getVInVoltage() {
    return RoboRioDataJNI.getVInVoltage(m_index);
  }
  public void setVInVoltage(double vInVoltage) {
    RoboRioDataJNI.setVInVoltage(m_index, vInVoltage);
  }

  public CallbackStore registerVInCurrentCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerVInCurrentCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, RoboRioDataJNI::cancelVInCurrentCallback);
  }
  public double getVInCurrent() {
    return RoboRioDataJNI.getVInCurrent(m_index);
  }
  public void setVInCurrent(double vInCurrent) {
    RoboRioDataJNI.setVInCurrent(m_index, vInCurrent);
  }

  public CallbackStore registerUserVoltage6VCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerUserVoltage6VCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, RoboRioDataJNI::cancelUserVoltage6VCallback);
  }
  public double getUserVoltage6V() {
    return RoboRioDataJNI.getUserVoltage6V(m_index);
  }
  public void setUserVoltage6V(double userVoltage6V) {
    RoboRioDataJNI.setUserVoltage6V(m_index, userVoltage6V);
  }

  public CallbackStore registerUserCurrent6VCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerUserCurrent6VCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, RoboRioDataJNI::cancelUserCurrent6VCallback);
  }
  public double getUserCurrent6V() {
    return RoboRioDataJNI.getUserCurrent6V(m_index);
  }
  public void setUserCurrent6V(double userCurrent6V) {
    RoboRioDataJNI.setUserCurrent6V(m_index, userCurrent6V);
  }

  public CallbackStore registerUserActive6VCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerUserActive6VCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, RoboRioDataJNI::cancelUserActive6VCallback);
  }
  public boolean getUserActive6V() {
    return RoboRioDataJNI.getUserActive6V(m_index);
  }
  public void setUserActive6V(boolean userActive6V) {
    RoboRioDataJNI.setUserActive6V(m_index, userActive6V);
  }

  public CallbackStore registerUserVoltage5VCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerUserVoltage5VCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, RoboRioDataJNI::cancelUserVoltage5VCallback);
  }
  public double getUserVoltage5V() {
    return RoboRioDataJNI.getUserVoltage5V(m_index);
  }
  public void setUserVoltage5V(double userVoltage5V) {
    RoboRioDataJNI.setUserVoltage5V(m_index, userVoltage5V);
  }

  public CallbackStore registerUserCurrent5VCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerUserCurrent5VCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, RoboRioDataJNI::cancelUserCurrent5VCallback);
  }
  public double getUserCurrent5V() {
    return RoboRioDataJNI.getUserCurrent5V(m_index);
  }
  public void setUserCurrent5V(double userCurrent5V) {
    RoboRioDataJNI.setUserCurrent5V(m_index, userCurrent5V);
  }

  public CallbackStore registerUserActive5VCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerUserActive5VCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, RoboRioDataJNI::cancelUserActive5VCallback);
  }
  public boolean getUserActive5V() {
    return RoboRioDataJNI.getUserActive5V(m_index);
  }
  public void setUserActive5V(boolean userActive5V) {
    RoboRioDataJNI.setUserActive5V(m_index, userActive5V);
  }

  public CallbackStore registerUserVoltage3V3Callback(NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerUserVoltage3V3Callback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, RoboRioDataJNI::cancelUserVoltage3V3Callback);
  }
  public double getUserVoltage3V3() {
    return RoboRioDataJNI.getUserVoltage3V3(m_index);
  }
  public void setUserVoltage3V3(double userVoltage3V3) {
    RoboRioDataJNI.setUserVoltage3V3(m_index, userVoltage3V3);
  }

  public CallbackStore registerUserCurrent3V3Callback(NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerUserCurrent3V3Callback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, RoboRioDataJNI::cancelUserCurrent3V3Callback);
  }
  public double getUserCurrent3V3() {
    return RoboRioDataJNI.getUserCurrent3V3(m_index);
  }
  public void setUserCurrent3V3(double userCurrent3V3) {
    RoboRioDataJNI.setUserCurrent3V3(m_index, userCurrent3V3);
  }

  public CallbackStore registerUserActive3V3Callback(NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerUserActive3V3Callback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, RoboRioDataJNI::cancelUserActive3V3Callback);
  }
  public boolean getUserActive3V3() {
    return RoboRioDataJNI.getUserActive3V3(m_index);
  }
  public void setUserActive3V3(boolean userActive3V3) {
    RoboRioDataJNI.setUserActive3V3(m_index, userActive3V3);
  }

  public CallbackStore registerUserFaults6VCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerUserFaults6VCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, RoboRioDataJNI::cancelUserFaults6VCallback);
  }
  public int getUserFaults6V() {
    return RoboRioDataJNI.getUserFaults6V(m_index);
  }
  public void setUserFaults6V(int userFaults6V) {
    RoboRioDataJNI.setUserFaults6V(m_index, userFaults6V);
  }

  public CallbackStore registerUserFaults5VCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerUserFaults5VCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, RoboRioDataJNI::cancelUserFaults5VCallback);
  }
  public int getUserFaults5V() {
    return RoboRioDataJNI.getUserFaults5V(m_index);
  }
  public void setUserFaults5V(int userFaults5V) {
    RoboRioDataJNI.setUserFaults5V(m_index, userFaults5V);
  }

  public CallbackStore registerUserFaults3V3Callback(NotifyCallback callback, boolean initialNotify) {
    int uid = RoboRioDataJNI.registerUserFaults3V3Callback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, RoboRioDataJNI::cancelUserFaults3V3Callback);
  }
  public int getUserFaults3V3() {
    return RoboRioDataJNI.getUserFaults3V3(m_index);
  }
  public void setUserFaults3V3(int userFaults3V3) {
    RoboRioDataJNI.setUserFaults3V3(m_index, userFaults3V3);
  }

  public void resetData() {
    RoboRioDataJNI.resetData(m_index);
  }
}
