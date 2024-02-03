// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.AnalogJNI;
import edu.wpi.first.hal.DMAJNISample;

/** DMA sample. */
public class DMASample {
  /** DMA read status. */
  public enum DMAReadStatus {
    /** OK status. */
    kOk(1),
    /** Timeout status. */
    kTimeout(2),
    /** Error status. */
    kError(3);

    private final int value;

    DMAReadStatus(int value) {
      this.value = value;
    }

    /**
     * Returns the DMAReadStatus value.
     *
     * @return The DMAReadStatus value.
     */
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

  /** Default constructor. */
  public DMASample() {}

  /**
   * Retrieves a new DMA sample.
   *
   * @param dma DMA object.
   * @param timeoutSeconds Timeout in seconds for retrieval.
   * @return DMA read status.
   */
  public DMAReadStatus update(DMA dma, double timeoutSeconds) {
    return DMAReadStatus.getValue(m_dmaSample.update(dma.m_dmaHandle, timeoutSeconds));
  }

  /**
   * Returns the DMA sample time in microseconds.
   *
   * @return The DMA sample time in microseconds.
   */
  public long getTime() {
    return m_dmaSample.getTime();
  }

  /**
   * Returns the DMA sample timestamp in seconds.
   *
   * @return The DMA sample timestamp in seconds.
   */
  public double getTimeStamp() {
    return getTime() * 1.0e-6;
  }

  /**
   * Returns the DMA sample capture size.
   *
   * @return The DMA sample capture size.
   */
  public int getCaptureSize() {
    return m_dmaSample.getCaptureSize();
  }

  /**
   * Returns the number of DMA trigger channels.
   *
   * @return The number of DMA trigger channels.
   */
  public int getTriggerChannels() {
    return m_dmaSample.getTriggerChannels();
  }

  /**
   * Returns the number of remaining samples.
   *
   * @return The number of remaining samples.
   */
  public int getRemaining() {
    return m_dmaSample.getRemaining();
  }

  /**
   * Returns raw encoder value from DMA.
   *
   * @param encoder Encoder used for DMA.
   * @return Raw encoder value from DMA.
   */
  public int getEncoderRaw(Encoder encoder) {
    return m_dmaSample.getEncoder(encoder.m_encoder);
  }

  /**
   * Returns encoder distance from DMA.
   *
   * @param encoder Encoder used for DMA.
   * @return Encoder distance from DMA.
   */
  public double getEncoderDistance(Encoder encoder) {
    double val = getEncoderRaw(encoder);
    val *= encoder.getDecodingScaleFactor();
    val *= encoder.getDistancePerPulse();
    return val;
  }

  /**
   * Returns raw encoder period from DMA.
   *
   * @param encoder Encoder used for DMA.
   * @return Raw encoder period from DMA.
   */
  public int getEncoderPeriodRaw(Encoder encoder) {
    return m_dmaSample.getEncoderPeriod(encoder.m_encoder);
  }

  /**
   * Returns counter value from DMA.
   *
   * @param counter Counter used for DMA.
   * @return Counter value from DMA.
   */
  public int getCounter(Counter counter) {
    return m_dmaSample.getCounter(counter.m_counter);
  }

  /**
   * Returns counter period from DMA.
   *
   * @param counter Counter used for DMA.
   * @return Counter period from DMA.
   */
  public int getCounterPeriod(Counter counter) {
    return m_dmaSample.getCounterPeriod(counter.m_counter);
  }

  /**
   * Returns digital source value from DMA.
   *
   * @param digitalSource DigitalSource used for DMA.
   * @return DigitalSource value from DMA.
   */
  public boolean getDigitalSource(DigitalSource digitalSource) {
    return m_dmaSample.getDigitalSource(digitalSource.getPortHandleForRouting());
  }

  /**
   * Returns raw analog input value from DMA.
   *
   * @param analogInput AnalogInput used for DMA.
   * @return Raw analog input value from DMA.
   */
  public int getAnalogInputRaw(AnalogInput analogInput) {
    return m_dmaSample.getAnalogInput(analogInput.m_port);
  }

  /**
   * Returns analog input voltage from DMA.
   *
   * @param analogInput AnalogInput used for DMA.
   * @return Analog input voltage from DMA.
   */
  public double getAnalogInputVoltage(AnalogInput analogInput) {
    return AnalogJNI.getAnalogValueToVolts(analogInput.m_port, getAnalogInputRaw(analogInput));
  }

  /**
   * Returns averaged raw analog input value from DMA.
   *
   * @param analogInput AnalogInput used for DMA.
   * @return Averaged raw analog input value from DMA.
   */
  public int getAveragedAnalogInputRaw(AnalogInput analogInput) {
    return m_dmaSample.getAnalogInputAveraged(analogInput.m_port);
  }

  /**
   * Returns averaged analog input voltage from DMA.
   *
   * @param analogInput AnalogInput used for DMA.
   * @return Averaged analog input voltage from DMA.
   */
  public double getAveragedAnalogInputVoltage(AnalogInput analogInput) {
    return AnalogJNI.getAnalogValueToVolts(
        analogInput.m_port, getAveragedAnalogInputRaw(analogInput));
  }

  /**
   * Returns raw duty cycle output from DMA.
   *
   * @param dutyCycle DutyCycle used for DMA.
   * @return Raw duty cycle output from DMA.
   */
  public int getDutyCycleOutputRaw(DutyCycle dutyCycle) {
    return m_dmaSample.getDutyCycleOutput(dutyCycle.m_handle);
  }

  /**
   * Returns duty cycle output (0-1) from DMA.
   *
   * @param dutyCycle DutyCycle used for DMA.
   * @return Duty cycle output (0-1) from DMA.
   */
  public double getDutyCycleOutput(DutyCycle dutyCycle) {
    return m_dmaSample.getDutyCycleOutput(dutyCycle.m_handle)
        / (double) dutyCycle.getOutputScaleFactor();
  }
}
