// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.DMAJNI;

public class DMA implements AutoCloseable {
  final int m_dmaHandle;

  public DMA() {
    m_dmaHandle = DMAJNI.initialize();
  }

  @Override
  public void close() {
    DMAJNI.free(m_dmaHandle);
  }

  public void setPause(boolean pause) {
    DMAJNI.setPause(m_dmaHandle, pause);
  }

  public void setTimedTrigger(double periodSeconds) {
    DMAJNI.setTimedTrigger(m_dmaHandle, periodSeconds);
  }

  public void setTimedTriggerCycles(int cycles) {
    DMAJNI.setTimedTriggerCycles(m_dmaHandle, cycles);
  }

  public void addEncoder(Encoder encoder) {
    DMAJNI.addEncoder(m_dmaHandle, encoder.m_encoder);
  }

  public void addEncoderPeriod(Encoder encoder) {
    DMAJNI.addEncoderPeriod(m_dmaHandle, encoder.m_encoder);
  }

  public void addCounter(Counter counter) {
    DMAJNI.addCounter(m_dmaHandle, counter.m_counter);
  }

  public void addCounterPeriod(Counter counter) {
    DMAJNI.addCounterPeriod(m_dmaHandle, counter.m_counter);
  }

  public void addDigitalSource(DigitalSource digitalSource) {
    DMAJNI.addDigitalSource(m_dmaHandle, digitalSource.getPortHandleForRouting());
  }

  public void addDutyCycle(DutyCycle dutyCycle) {
    DMAJNI.addDutyCycle(m_dmaHandle, dutyCycle.m_handle);
  }

  public void addAnalogInput(AnalogInput analogInput) {
    DMAJNI.addAnalogInput(m_dmaHandle, analogInput.m_port);
  }

  public void addAveragedAnalogInput(AnalogInput analogInput) {
    DMAJNI.addAveragedAnalogInput(m_dmaHandle, analogInput.m_port);
  }

  public void addAnalogAccumulator(AnalogInput analogInput) {
    DMAJNI.addAnalogAccumulator(m_dmaHandle, analogInput.m_port);
  }

  public void setExternalTrigger(DigitalSource source, boolean rising, boolean falling) {
    DMAJNI.setExternalTrigger(
        m_dmaHandle,
        source.getPortHandleForRouting(),
        source.getAnalogTriggerTypeForRouting(),
        rising,
        falling);
  }

  public void clearSensors() {
    DMAJNI.clearSensors(m_dmaHandle);
  }

  public void clearExternalTriggers() {
    DMAJNI.clearExternalTriggers(m_dmaHandle);
  }

  public void startDMA(int queueDepth) {
    DMAJNI.startDMA(m_dmaHandle, queueDepth);
  }

  public void stopDMA() {
    DMAJNI.stopDMA(m_dmaHandle);
  }
}
