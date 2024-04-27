// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/**
 * DMA HAL JNI functions.
 *
 * @see "hal/DHA.h"
 */
public class DMAJNI extends JNIWrapper {
  /**
   * Initializes an object for performing DMA transfers.
   *
   * @return the created dma handle
   * @see "HAL_InitializeDMA"
   */
  public static native int initialize();

  /**
   * Frees a DMA object.
   *
   * @param handle the dma handle
   * @see "HAL_FreeDMA"
   */
  public static native void free(int handle);

  /**
   * Pauses or unpauses a DMA transfer.
   *
   * <p>This can only be called while DMA is running.
   *
   * @param handle the dma handle
   * @param pause true to pause transfers, false to resume.
   * @see "HAL_SetDMAPause"
   */
  public static native void setPause(int handle, boolean pause);

  /**
   * Sets DMA transfers to occur at a specific timed interval.
   *
   * <p>This will remove any external triggers. Only timed or external is supported.
   *
   * <p>Only 1 timed period is supported.
   *
   * <p>This can only be called if DMA is not started.
   *
   * @param handle the dma handle
   * @param periodSeconds the period to trigger in seconds
   * @see "HAL_SetDMATimedTrigger"
   */
  public static native void setTimedTrigger(int handle, double periodSeconds);

  /**
   * Sets DMA transfers to occur at a specific timed interval in FPGA cycles.
   *
   * <p>This will remove any external triggers. Only timed or external is supported.
   *
   * <p>Only 1 timed period is supported
   *
   * <p>The FPGA currently runs at 40 MHz, but this can change.
   * HAL_GetSystemClockTicksPerMicrosecond can be used to get a computable value for this.
   *
   * <p>This can only be called if DMA is not started.
   *
   * @param handle the dma handle
   * @param cycles the period to trigger in FPGA cycles
   * @see "HAL_SetDMATimedTriggerCycles"
   */
  public static native void setTimedTriggerCycles(int handle, int cycles);

  /**
   * Adds position data for an encoder to be collected by DMA.
   *
   * <p>This can only be called if DMA is not started.
   *
   * @param handle the dma handle
   * @param encoderHandle the encoder to add
   * @see "HAL_AddDMAEncoder"
   */
  public static native void addEncoder(int handle, int encoderHandle);

  /**
   * Adds timer data for an encoder to be collected by DMA.
   *
   * <p>This can only be called if DMA is not started.
   *
   * @param handle the dma handle
   * @param encoderHandle the encoder to add
   * @see "HAL_AddDMAEncoderPeriod"
   */
  public static native void addEncoderPeriod(int handle, int encoderHandle);

  /**
   * Adds position data for an counter to be collected by DMA.
   *
   * <p>This can only be called if DMA is not started.
   *
   * @param handle the dma handle
   * @param counterHandle the counter to add
   * @see "HAL_AddDMACounter"
   */
  public static native void addCounter(int handle, int counterHandle);

  /**
   * Adds timer data for an counter to be collected by DMA.
   *
   * @param handle the dma handle
   * @param counterHandle the counter to add
   * @see "HAL_AddDMACounterPeriod"
   */
  public static native void addCounterPeriod(int handle, int counterHandle);

  /**
   * Adds a digital source to be collected by DMA.
   *
   * <p>This can only be called if DMA is not started.
   *
   * @param handle the dma handle
   * @param digitalSourceHandle the digital source to add
   * @see "HAL_AddDMADigitalSource"
   */
  public static native void addDigitalSource(int handle, int digitalSourceHandle);

  /**
   * Adds a duty cycle input to be collected by DMA.
   *
   * <p>This can only be called if DMA is not started.
   *
   * @param handle the dma handle
   * @param dutyCycleHandle the duty cycle input to add
   * @see "HAL_AddDMADutyCycle"
   */
  public static native void addDutyCycle(int handle, int dutyCycleHandle);

  /**
   * Adds an analog input to be collected by DMA.
   *
   * <p>This can only be called if DMA is not started.
   *
   * @param handle the dma handle
   * @param analogInputHandle the analog input to add
   * @see "HAL_AddDMAAnalogInput"
   */
  public static native void addAnalogInput(int handle, int analogInputHandle);

  /**
   * Adds averaged data of an analog input to be collected by DMA.
   *
   * <p>This can only be called if DMA is not started.
   *
   * @param handle the dma handle
   * @param analogInputHandle the analog input to add
   * @see "HAL_AddDMAAveragedAnalogInput"
   */
  public static native void addAveragedAnalogInput(int handle, int analogInputHandle);

  /**
   * Adds accumulator data of an analog input to be collected by DMA.
   *
   * <p>This can only be called if DMA is not started.
   *
   * @param handle the dma handle
   * @param analogInputHandle the analog input to add
   * @see "HAL_AddDMAAnalogAccumulator"
   */
  public static native void addAnalogAccumulator(int handle, int analogInputHandle);

  /**
   * Sets DMA transfers to occur on an external trigger.
   *
   * <p>This will remove any timed trigger set. Only timed or external is supported.
   *
   * <p>Up to 8 external triggers are currently supported.
   *
   * <p>This can only be called if DMA is not started.
   *
   * @param handle the dma handle
   * @param digitalSourceHandle the digital source handle (either a HAL_AnalogTriggerHandle or a
   *     HAL_DigitalHandle)
   * @param analogTriggerType the analog trigger type if the source is an analog trigger
   * @param rising true to trigger on rising edge
   * @param falling true to trigger on falling edge
   * @return the index of the trigger
   * @see "HAL_SetDMAExternalTrigger"
   */
  public static native int setExternalTrigger(
      int handle, int digitalSourceHandle, int analogTriggerType, boolean rising, boolean falling);

  /**
   * Clear all sensors from the DMA collection list.
   *
   * <p>This can only be called if DMA is not started.
   *
   * @param handle the dma handle
   * @see "HAL_ClearDMASensors"
   */
  public static native void clearSensors(int handle);

  /**
   * Clear all external triggers from the DMA trigger list.
   *
   * <p>This can only be called if DMA is not started.
   *
   * @param handle the dma handle
   * @see "HAL_ClearDMAExternalTriggers"
   */
  public static native void clearExternalTriggers(int handle);

  /**
   * Starts DMA Collection.
   *
   * @param handle the dma handle
   * @param queueDepth the number of objects to be able to queue
   * @see "HAL_StartDMA"
   */
  public static native void startDMA(int handle, int queueDepth);

  /**
   * Stops DMA Collection.
   *
   * @param handle the dma handle
   * @see "HAL_StopDMA"
   */
  public static native void stopDMA(int handle);

  /**
   * Reads a DMA sample from the queue.
   *
   * @param handle the dma handle
   * @param timeoutSeconds the time to wait for data to be queued before timing out
   * @param buffer the sample object to place data into
   * @param sampleStore index 0-21 channelOffsets, index 22: capture size, index 23: triggerChannels
   *     (bitflags), index 24: remaining, index 25: read status
   * @return timestamp of the DMA Sample
   */
  public static native long readDMA(
      int handle, double timeoutSeconds, int[] buffer, int[] sampleStore);

  /**
   * Get the sensor DMA sample.
   *
   * @param handle the dma handle
   * @return The DMA sample
   */
  public static native DMAJNISample.BaseStore getSensorReadData(int handle);

  /** Utility class. */
  private DMAJNI() {}
}
