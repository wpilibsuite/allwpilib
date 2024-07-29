// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.DMAJNI;
import edu.wpi.first.wpilibj.motorcontrol.PWMMotorController;

/** Class for configuring Direct Memory Access (DMA) of FPGA inputs. */
public class DMA implements AutoCloseable {
  final int m_dmaHandle;

  /** Default constructor. */
  public DMA() {
    m_dmaHandle = DMAJNI.initialize();
  }

  @Override
  public void close() {
    DMAJNI.free(m_dmaHandle);
  }

  /**
   * Sets whether DMA is paused.
   *
   * @param pause True pauses DMA.
   */
  public void setPause(boolean pause) {
    DMAJNI.setPause(m_dmaHandle, pause);
  }

  /**
   * Sets DMA to trigger at an interval.
   *
   * @param periodSeconds Period at which to trigger DMA in seconds.
   */
  public void setTimedTrigger(double periodSeconds) {
    DMAJNI.setTimedTrigger(m_dmaHandle, periodSeconds);
  }

  /**
   * Sets number of DMA cycles to trigger.
   *
   * @param cycles Number of cycles.
   */
  public void setTimedTriggerCycles(int cycles) {
    DMAJNI.setTimedTriggerCycles(m_dmaHandle, cycles);
  }

  /**
   * Adds position data for an encoder to be collected by DMA.
   *
   * <p>This can only be called if DMA is not started.
   *
   * @param encoder Encoder to add to DMA.
   */
  public void addEncoder(Encoder encoder) {
    DMAJNI.addEncoder(m_dmaHandle, encoder.m_encoder);
  }

  /**
   * Adds timer data for an encoder to be collected by DMA.
   *
   * <p>This can only be called if DMA is not started.
   *
   * @param encoder Encoder to add to DMA.
   */
  public void addEncoderPeriod(Encoder encoder) {
    DMAJNI.addEncoderPeriod(m_dmaHandle, encoder.m_encoder);
  }

  /**
   * Adds position data for an counter to be collected by DMA.
   *
   * <p>This can only be called if DMA is not started.
   *
   * @param counter Counter to add to DMA.
   */
  public void addCounter(Counter counter) {
    DMAJNI.addCounter(m_dmaHandle, counter.m_counter);
  }

  /**
   * Adds timer data for an counter to be collected by DMA.
   *
   * <p>This can only be called if DMA is not started.
   *
   * @param counter Counter to add to DMA.
   */
  public void addCounterPeriod(Counter counter) {
    DMAJNI.addCounterPeriod(m_dmaHandle, counter.m_counter);
  }

  /**
   * Adds a digital source to be collected by DMA.
   *
   * <p>This can only be called if DMA is not started.
   *
   * @param digitalSource DigitalSource to add to DMA.
   */
  public void addDigitalSource(DigitalSource digitalSource) {
    DMAJNI.addDigitalSource(m_dmaHandle, digitalSource.getPortHandleForRouting());
  }

  /**
   * Adds a duty cycle input to be collected by DMA.
   *
   * <p>This can only be called if DMA is not started.
   *
   * @param dutyCycle DutyCycle to add to DMA.
   */
  public void addDutyCycle(DutyCycle dutyCycle) {
    DMAJNI.addDutyCycle(m_dmaHandle, dutyCycle.m_handle);
  }

  /**
   * Adds an analog input to be collected by DMA.
   *
   * <p>This can only be called if DMA is not started.
   *
   * @param analogInput AnalogInput to add to DMA.
   */
  public void addAnalogInput(AnalogInput analogInput) {
    DMAJNI.addAnalogInput(m_dmaHandle, analogInput.m_port);
  }

  /**
   * Adds averaged data of an analog input to be collected by DMA.
   *
   * <p>This can only be called if DMA is not started.
   *
   * @param analogInput AnalogInput to add to DMA.
   */
  public void addAveragedAnalogInput(AnalogInput analogInput) {
    DMAJNI.addAveragedAnalogInput(m_dmaHandle, analogInput.m_port);
  }

  /**
   * Adds accumulator data of an analog input to be collected by DMA.
   *
   * <p>This can only be called if DMA is not started.
   *
   * @param analogInput AnalogInput to add to DMA.
   */
  public void addAnalogAccumulator(AnalogInput analogInput) {
    DMAJNI.addAnalogAccumulator(m_dmaHandle, analogInput.m_port);
  }

  /**
   * Sets an external DMA trigger.
   *
   * @param source the source to trigger from.
   * @param rising trigger on rising edge.
   * @param falling trigger on falling edge.
   * @return the index of the trigger
   */
  public int setExternalTrigger(DigitalSource source, boolean rising, boolean falling) {
    return DMAJNI.setExternalTrigger(
        m_dmaHandle,
        source.getPortHandleForRouting(),
        source.getAnalogTriggerTypeForRouting(),
        rising,
        falling);
  }

  /**
   * Sets a DMA PWM edge trigger.
   *
   * @param pwm the PWM to trigger from.
   * @param rising trigger on rising edge.
   * @param falling trigger on falling edge.
   * @return the index of the trigger
   */
  public int setPwmEdgeTrigger(PWM pwm, boolean rising, boolean falling) {
    return DMAJNI.setExternalTrigger(m_dmaHandle, pwm.getHandle(), 0, rising, falling);
  }

  /**
   * Sets a DMA PWMMotorController edge trigger.
   *
   * @param pwm the PWMMotorController to trigger from.
   * @param rising trigger on rising edge.
   * @param falling trigger on falling edge.
   * @return the index of the trigger
   */
  public int setPwmEdgeTrigger(PWMMotorController pwm, boolean rising, boolean falling) {
    return DMAJNI.setExternalTrigger(m_dmaHandle, pwm.getPwmHandle(), 0, rising, falling);
  }

  /**
   * Clear all sensors from the DMA collection list.
   *
   * <p>This can only be called if DMA is not started.
   */
  public void clearSensors() {
    DMAJNI.clearSensors(m_dmaHandle);
  }

  /**
   * Clear all external triggers from the DMA trigger list.
   *
   * <p>This can only be called if DMA is not started.
   */
  public void clearExternalTriggers() {
    DMAJNI.clearExternalTriggers(m_dmaHandle);
  }

  /**
   * Starts DMA Collection.
   *
   * @param queueDepth The number of objects to be able to queue.
   */
  public void start(int queueDepth) {
    DMAJNI.startDMA(m_dmaHandle, queueDepth);
  }

  /** Stops DMA Collection. */
  public void stop() {
    DMAJNI.stopDMA(m_dmaHandle);
  }
}
