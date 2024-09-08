// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/DMA.h>
#include <units/time.h>

namespace frc {
class Encoder;
class Counter;
class DigitalSource;
class DutyCycle;
class AnalogInput;
class DMASample;
class PWM;
class PWMMotorController;

/**
 * Class for configuring Direct Memory Access (DMA) of FPGA inputs.
 */
class DMA {
  friend class DMASample;

 public:
  DMA();

  DMA& operator=(DMA&& other) = default;
  DMA(DMA&& other) = default;

  /**
   * Sets whether DMA is paused.
   *
   * @param pause True pauses DMA.
   */
  void SetPause(bool pause);

  /**
   * Sets DMA to trigger at an interval.
   *
   * @param period Period at which to trigger DMA.
   */
  void SetTimedTrigger(units::second_t period);

  /**
   * Sets number of DMA cycles to trigger.
   *
   * @param cycles Number of cycles.
   */
  void SetTimedTriggerCycles(int cycles);

  /**
   * Adds position data for an encoder to be collected by DMA.
   *
   * This can only be called if DMA is not started.
   *
   * @param encoder Encoder to add to DMA.
   */
  void AddEncoder(const Encoder* encoder);

  /**
   * Adds timer data for an encoder to be collected by DMA.
   *
   * This can only be called if DMA is not started.
   *
   * @param encoder Encoder to add to DMA.
   */
  void AddEncoderPeriod(const Encoder* encoder);

  /**
   * Adds position data for an counter to be collected by DMA.
   *
   * This can only be called if DMA is not started.
   *
   * @param counter Counter to add to DMA.
   */
  void AddCounter(const Counter* counter);

  /**
   * Adds timer data for an counter to be collected by DMA.
   *
   * This can only be called if DMA is not started.
   *
   * @param counter Counter to add to DMA.
   */
  void AddCounterPeriod(const Counter* counter);

  /**
   * Adds a digital source to be collected by DMA.
   *
   * This can only be called if DMA is not started.
   *
   * @param digitalSource DigitalSource to add to DMA.
   */
  void AddDigitalSource(const DigitalSource* digitalSource);

  /**
   * Adds a digital source to be collected by DMA.
   *
   * This can only be called if DMA is not started.
   *
   * @param digitalSource DigitalSource to add to DMA.
   */
  void AddDutyCycle(const DutyCycle* digitalSource);

  /**
   * Adds an analog input to be collected by DMA.
   *
   * This can only be called if DMA is not started.
   *
   * @param analogInput AnalogInput to add to DMA.
   */
  void AddAnalogInput(const AnalogInput* analogInput);

  /**
   * Adds averaged data of an analog input to be collected by DMA.
   *
   * This can only be called if DMA is not started.
   *
   * @param analogInput AnalogInput to add to DMA.
   */
  void AddAveragedAnalogInput(const AnalogInput* analogInput);

  /**
   * Adds accumulator data of an analog input to be collected by DMA.
   *
   * This can only be called if DMA is not started.
   *
   * @param analogInput AnalogInput to add to DMA.
   */
  void AddAnalogAccumulator(const AnalogInput* analogInput);

  /**
   * Sets an external DMA trigger.
   *
   * @param source the source to trigger from.
   * @param rising trigger on rising edge.
   * @param falling trigger on falling edge.
   * @return the index of the trigger
   */
  int SetExternalTrigger(DigitalSource* source, bool rising, bool falling);

  /**
   * Sets a DMA PWM edge trigger.
   *
   * @param pwm the PWM to trigger from.
   * @param rising trigger on rising edge.
   * @param falling trigger on falling edge.
   * @return the index of the trigger
   */
  int SetPwmEdgeTrigger(PWM* pwm, bool rising, bool falling);

  /**
   * Sets a DMA PWMMotorController edge trigger.
   *
   * @param pwm the PWMMotorController to trigger from.
   * @param rising trigger on rising edge.
   * @param falling trigger on falling edge.
   * @return the index of the trigger
   */
  int SetPwmEdgeTrigger(PWMMotorController* pwm, bool rising, bool falling);

  /**
   * Clear all sensors from the DMA collection list.
   *
   * This can only be called if DMA is not started.
   */
  void ClearSensors();

  /**
   * Clear all external triggers from the DMA trigger list.
   *
   * This can only be called if DMA is not started.
   */
  void ClearExternalTriggers();

  /**
   * Starts DMA Collection.
   *
   * @param queueDepth The number of objects to be able to queue.
   */
  void Start(int queueDepth);

  /**
   * Stops DMA Collection.
   */
  void Stop();

 private:
  hal::Handle<HAL_DMAHandle, HAL_FreeDMA> dmaHandle;
};
}  // namespace frc
