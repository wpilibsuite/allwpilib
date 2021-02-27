// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.EncoderDataJNI;
import edu.wpi.first.hal.simulation.NotifyCallback;
import edu.wpi.first.wpilibj.Encoder;
import java.util.NoSuchElementException;

/** Class to control a simulated encoder. */
public class EncoderSim {
  private final int m_index;

  /**
   * Constructs from an Encoder object.
   *
   * @param encoder Encoder to simulate
   */
  public EncoderSim(Encoder encoder) {
    m_index = encoder.getFPGAIndex();
  }

  private EncoderSim(int index) {
    m_index = index;
  }

  /**
   * Creates an EncoderSim for a digital input channel. Encoders take two channels, so either one
   * may be specified.
   *
   * @param channel digital input channel
   * @return Simulated object
   * @throws NoSuchElementException if no Encoder is configured for that channel
   */
  public static EncoderSim createForChannel(int channel) {
    int index = EncoderDataJNI.findForChannel(channel);
    if (index < 0) {
      throw new NoSuchElementException("no encoder found for channel " + channel);
    }
    return new EncoderSim(index);
  }

  /**
   * Creates an EncoderSim for a simulated index. The index is incremented for each simulated
   * Encoder.
   *
   * @param index simulator index
   * @return Simulated object
   */
  public static EncoderSim createForIndex(int index) {
    return new EncoderSim(index);
  }

  /**
   * Register a callback on the Initialized property of the encoder.
   *
   * @param callback the callback that will be called whenever the Initialized property is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = EncoderDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, EncoderDataJNI::cancelInitializedCallback);
  }

  /**
   * Read the Initialized value of the encoder.
   *
   * @return true if initialized
   */
  public boolean getInitialized() {
    return EncoderDataJNI.getInitialized(m_index);
  }

  /**
   * Change the Initialized value of the encoder.
   *
   * @param initialized the new value
   */
  public void setInitialized(boolean initialized) {
    EncoderDataJNI.setInitialized(m_index, initialized);
  }

  /**
   * Register a callback on the count property of the encoder.
   *
   * @param callback the callback that will be called whenever the count property is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerCountCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = EncoderDataJNI.registerCountCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, EncoderDataJNI::cancelCountCallback);
  }

  /**
   * Read the count of the encoder.
   *
   * @return the count
   */
  public int getCount() {
    return EncoderDataJNI.getCount(m_index);
  }

  /**
   * Change the count of the encoder.
   *
   * @param count the new count
   */
  public void setCount(int count) {
    EncoderDataJNI.setCount(m_index, count);
  }

  /**
   * Register a callback on the period of the encoder.
   *
   * @param callback the callback that will be called whenever the period is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerPeriodCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = EncoderDataJNI.registerPeriodCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, EncoderDataJNI::cancelPeriodCallback);
  }

  /**
   * Read the period of the encoder.
   *
   * @return the encoder period
   */
  public double getPeriod() {
    return EncoderDataJNI.getPeriod(m_index);
  }

  /**
   * Change the encoder period.
   *
   * @param period the new period
   */
  public void setPeriod(double period) {
    EncoderDataJNI.setPeriod(m_index, period);
  }

  /**
   * Register a callback to be called whenever the encoder is reset.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback on the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerResetCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = EncoderDataJNI.registerResetCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, EncoderDataJNI::cancelResetCallback);
  }

  /**
   * Check if the encoder has been reset.
   *
   * @return true if reset
   */
  public boolean getReset() {
    return EncoderDataJNI.getReset(m_index);
  }

  /**
   * Change the reset property of the encoder.
   *
   * @param reset the new value
   */
  public void setReset(boolean reset) {
    EncoderDataJNI.setReset(m_index, reset);
  }

  /**
   * Register a callback to be run whenever the max period of the encoder is changed.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback on the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerMaxPeriodCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = EncoderDataJNI.registerMaxPeriodCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, EncoderDataJNI::cancelMaxPeriodCallback);
  }

  /**
   * Get the max period of the encoder.
   *
   * @return the max period of the encoder
   */
  public double getMaxPeriod() {
    return EncoderDataJNI.getMaxPeriod(m_index);
  }

  /**
   * Change the max period of the encoder.
   *
   * @param maxPeriod the new value
   */
  public void setMaxPeriod(double maxPeriod) {
    EncoderDataJNI.setMaxPeriod(m_index, maxPeriod);
  }

  /**
   * Register a callback on the direction of the encoder.
   *
   * @param callback the callback that will be called whenever the direction is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerDirectionCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = EncoderDataJNI.registerDirectionCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, EncoderDataJNI::cancelDirectionCallback);
  }

  /**
   * Get the direction of the encoder.
   *
   * @return the direction of the encoder
   */
  public boolean getDirection() {
    return EncoderDataJNI.getDirection(m_index);
  }

  /**
   * Set the direction of the encoder.
   *
   * @param direction the new direction
   */
  public void setDirection(boolean direction) {
    EncoderDataJNI.setDirection(m_index, direction);
  }

  /**
   * Register a callback on the reverse direction.
   *
   * @param callback the callback that will be called whenever the reverse direction is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerReverseDirectionCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = EncoderDataJNI.registerReverseDirectionCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, EncoderDataJNI::cancelReverseDirectionCallback);
  }

  /**
   * Get the reverse direction of the encoder.
   *
   * @return the reverse direction of the encoder
   */
  public boolean getReverseDirection() {
    return EncoderDataJNI.getReverseDirection(m_index);
  }

  /**
   * Set the reverse direction.
   *
   * @param reverseDirection the new value
   */
  public void setReverseDirection(boolean reverseDirection) {
    EncoderDataJNI.setReverseDirection(m_index, reverseDirection);
  }

  /**
   * Register a callback on the samples-to-average value of this encoder.
   *
   * @param callback the callback that will be called whenever the samples-to-average is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the {@link CallbackStore} object associated with this callback. Save a reference to
   *     this object so GC doesn't cancel the callback.
   */
  public CallbackStore registerSamplesToAverageCallback(
      NotifyCallback callback, boolean initialNotify) {
    int uid = EncoderDataJNI.registerSamplesToAverageCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, EncoderDataJNI::cancelSamplesToAverageCallback);
  }

  /**
   * Get the samples-to-average value.
   *
   * @return the samples-to-average value
   */
  public int getSamplesToAverage() {
    return EncoderDataJNI.getSamplesToAverage(m_index);
  }

  /**
   * Set the samples-to-average value.
   *
   * @param samplesToAverage the new value
   */
  public void setSamplesToAverage(int samplesToAverage) {
    EncoderDataJNI.setSamplesToAverage(m_index, samplesToAverage);
  }

  /**
   * Change the encoder distance.
   *
   * @param distance the new distance
   */
  public void setDistance(double distance) {
    EncoderDataJNI.setDistance(m_index, distance);
  }

  /**
   * Read the distance of the encoder.
   *
   * @return the encoder distance
   */
  public double getDistance() {
    return EncoderDataJNI.getDistance(m_index);
  }

  /**
   * Change the rate of the encoder.
   *
   * @param rate the new rate
   */
  public void setRate(double rate) {
    EncoderDataJNI.setRate(m_index, rate);
  }

  /**
   * Get the rate of the encoder.
   *
   * @return the rate of change
   */
  public double getRate() {
    return EncoderDataJNI.getRate(m_index);
  }

  /** Resets all simulation data for this encoder. */
  public void resetData() {
    EncoderDataJNI.resetData(m_index);
  }
}
