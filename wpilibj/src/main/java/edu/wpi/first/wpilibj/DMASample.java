// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.AnalogJNI;
import edu.wpi.first.hal.DMAJNISample;

public class DMASample {
  public enum DMAReadStatus {
    kOk(1),
    kTimeout(2),
    kError(3);

    private final int value;

    DMAReadStatus(int value) {
      this.value = value;
    }

    public int getValue() {
      return value;
    }

    /**
     * Constructs a DMAReadStatus from a raw value.
     *
     * @param value raw value
     * @return enum value
     */
    public static DMAReadStatus getValue(int value) {
      if (value == 1) {
        return kOk;
      } else if (value == 2) {
        return kTimeout;
      }
      return kError;
    }
  }

  private final DMAJNISample m_dmaSample = new DMAJNISample();

  public DMAReadStatus update(DMA dma, double timeoutSeconds) {
    return DMAReadStatus.getValue(m_dmaSample.update(dma.m_dmaHandle, timeoutSeconds));
  }

  public int getCaptureSize() {
    return m_dmaSample.getCaptureSize();
  }

  public int getTriggerChannels() {
    return m_dmaSample.getTriggerChannels();
  }

  public int getRemaining() {
    return m_dmaSample.getRemaining();
  }

  public long getTime() {
    return m_dmaSample.getTime();
  }

  public double getTimeStamp() {
    return getTime() * 1.0e-6;
  }

  public int getEncoderRaw(Encoder encoder) {
    return m_dmaSample.getEncoder(encoder.m_encoder);
  }

  /**
   * Gets the scaled encoder distance for this sample.
   *
   * @param encoder the encoder to use to read
   * @return the distance
   */
  public double getEncoderDistance(Encoder encoder) {
    double val = getEncoderRaw(encoder);
    val *= encoder.getDecodingScaleFactor();
    val *= encoder.getDistancePerPulse();
    return val;
  }

  public int getEncoderPeriodRaw(Encoder encoder) {
    return m_dmaSample.getEncoderPeriod(encoder.m_encoder);
  }

  public int getCounter(Counter counter) {
    return m_dmaSample.getCounter(counter.m_counter);
  }

  public int getCounterPeriod(Counter counter) {
    return m_dmaSample.getCounterPeriod(counter.m_counter);
  }

  public boolean getDigitalSource(DigitalSource digitalSource) {
    return m_dmaSample.getDigitalSource(digitalSource.getPortHandleForRouting());
  }

  public int getAnalogInputRaw(AnalogInput analogInput) {
    return m_dmaSample.getAnalogInput(analogInput.m_port);
  }

  public double getAnalogInputVoltage(AnalogInput analogInput) {
    return AnalogJNI.getAnalogValueToVolts(analogInput.m_port, getAnalogInputRaw(analogInput));
  }

  public int getAveragedAnalogInputRaw(AnalogInput analogInput) {
    return m_dmaSample.getAnalogInputAveraged(analogInput.m_port);
  }

  public double getAveragedAnalogInputVoltage(AnalogInput analogInput) {
    return AnalogJNI.getAnalogValueToVolts(
        analogInput.m_port, getAveragedAnalogInputRaw(analogInput));
  }

  public int getDutyCycleOutputRaw(DutyCycle dutyCycle) {
    return m_dmaSample.getDutyCycleOutput(dutyCycle.m_handle);
  }

  public double getDutyCycleOutput(DutyCycle dutyCycle) {
    return m_dmaSample.getDutyCycleOutput(dutyCycle.m_handle)
        / (double) dutyCycle.getOutputScaleFactor();
  }
}
