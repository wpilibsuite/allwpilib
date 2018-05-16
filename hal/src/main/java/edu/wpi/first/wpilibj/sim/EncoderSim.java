/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.sim;

import edu.wpi.first.hal.sim.mockdata.EncoderDataJNI;

public class EncoderSim {
  private int m_index;

  public EncoderSim(int index) {
    m_index = index;
  }

  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = EncoderDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, EncoderDataJNI::cancelInitializedCallback);
  }
  public boolean getInitialized() {
    return EncoderDataJNI.getInitialized(m_index);
  }
  public void setInitialized(boolean initialized) {
    EncoderDataJNI.setInitialized(m_index, initialized);
  }

  public CallbackStore registerCountCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = EncoderDataJNI.registerCountCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, EncoderDataJNI::cancelCountCallback);
  }
  public int getCount() {
    return EncoderDataJNI.getCount(m_index);
  }
  public void setCount(int count) {
    EncoderDataJNI.setCount(m_index, count);
  }

  public CallbackStore registerPeriodCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = EncoderDataJNI.registerPeriodCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, EncoderDataJNI::cancelPeriodCallback);
  }
  public double getPeriod() {
    return EncoderDataJNI.getPeriod(m_index);
  }
  public void setPeriod(double period) {
    EncoderDataJNI.setPeriod(m_index, period);
  }

  public CallbackStore registerResetCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = EncoderDataJNI.registerResetCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, EncoderDataJNI::cancelResetCallback);
  }
  public boolean getReset() {
    return EncoderDataJNI.getReset(m_index);
  }
  public void setReset(boolean reset) {
    EncoderDataJNI.setReset(m_index, reset);
  }

  public CallbackStore registerMaxPeriodCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = EncoderDataJNI.registerMaxPeriodCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, EncoderDataJNI::cancelMaxPeriodCallback);
  }
  public double getMaxPeriod() {
    return EncoderDataJNI.getMaxPeriod(m_index);
  }
  public void setMaxPeriod(double maxPeriod) {
    EncoderDataJNI.setMaxPeriod(m_index, maxPeriod);
  }

  public CallbackStore registerDirectionCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = EncoderDataJNI.registerDirectionCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, EncoderDataJNI::cancelDirectionCallback);
  }
  public boolean getDirection() {
    return EncoderDataJNI.getDirection(m_index);
  }
  public void setDirection(boolean direction) {
    EncoderDataJNI.setDirection(m_index, direction);
  }

  public CallbackStore registerReverseDirectionCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = EncoderDataJNI.registerReverseDirectionCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, EncoderDataJNI::cancelReverseDirectionCallback);
  }
  public boolean getReverseDirection() {
    return EncoderDataJNI.getReverseDirection(m_index);
  }
  public void setReverseDirection(boolean reverseDirection) {
    EncoderDataJNI.setReverseDirection(m_index, reverseDirection);
  }

  public CallbackStore registerSamplesToAverageCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = EncoderDataJNI.registerSamplesToAverageCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, EncoderDataJNI::cancelSamplesToAverageCallback);
  }
  public int getSamplesToAverage() {
    return EncoderDataJNI.getSamplesToAverage(m_index);
  }
  public void setSamplesToAverage(int samplesToAverage) {
    EncoderDataJNI.setSamplesToAverage(m_index, samplesToAverage);
  }

  public void resetData() {
    EncoderDataJNI.resetData(m_index);
  }
}
