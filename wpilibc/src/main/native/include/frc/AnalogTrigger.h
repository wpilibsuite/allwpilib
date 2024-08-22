// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <hal/AnalogTrigger.h>
#include <hal/Types.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc/AnalogTriggerOutput.h"

namespace frc {

class AnalogInput;
class DutyCycle;

class AnalogTrigger : public wpi::Sendable,
                      public wpi::SendableHelper<AnalogTrigger> {
  friend class AnalogTriggerOutput;

 public:
  /**
   * Constructor for an analog trigger given a channel number.
   *
   * @param channel The channel number on the roboRIO to represent. 0-3 are
   *                on-board 4-7 are on the MXP port.
   */
  explicit AnalogTrigger(int channel);

  /**
   * Construct an analog trigger using an existing analog input.
   *
   * This should be used in the case of sharing an analog channel between the
   * trigger and an analog input object.
   *
   * @param input A reference to the existing AnalogInput object
   */
  explicit AnalogTrigger(AnalogInput& input);

  /**
   * Construct an analog trigger using an existing analog input.
   *
   * This should be used in the case of sharing an analog channel between the
   * trigger and an analog input object.
   *
   * @param input A pointer to the existing AnalogInput object
   */
  explicit AnalogTrigger(AnalogInput* input);

  /**
   * Construct an analog trigger using an existing analog input.
   *
   * This should be used in the case of sharing an analog channel between the
   * trigger and an analog input object.
   *
   * @param input A shared_ptr to the existing AnalogInput object
   */
  explicit AnalogTrigger(std::shared_ptr<AnalogInput> input);

  /**
   * Construct an analog trigger using an existing duty cycle input.
   *
   * @param dutyCycle A reference to the existing DutyCycle object
   */
  explicit AnalogTrigger(DutyCycle& dutyCycle);

  /**
   * Construct an analog trigger using an existing duty cycle input.
   *
   * @param dutyCycle A pointer to the existing DutyCycle object
   */
  explicit AnalogTrigger(DutyCycle* dutyCycle);

  /**
   * Construct an analog trigger using an existing duty cycle input.
   *
   * @param dutyCycle A shared_ptr to the existing DutyCycle object
   */
  explicit AnalogTrigger(std::shared_ptr<DutyCycle> dutyCycle);

  AnalogTrigger(AnalogTrigger&&) = default;
  AnalogTrigger& operator=(AnalogTrigger&&) = default;

  ~AnalogTrigger() override = default;

  /**
   * Set the upper and lower limits of the analog trigger.
   *
   * The limits are given as floating point voltage values.
   *
   * @param lower The lower limit of the trigger in Volts.
   * @param upper The upper limit of the trigger in Volts.
   */
  void SetLimitsVoltage(double lower, double upper);

  /**
   * Set the upper and lower duty cycle limits of the analog trigger.
   *
   * The limits are given as floating point values between 0 and 1.
   *
   * @param lower The lower limit of the trigger in percentage.
   * @param upper The upper limit of the trigger in percentage.
   */
  void SetLimitsDutyCycle(double lower, double upper);

  /**
   * Set the upper and lower limits of the analog trigger.
   *
   * The limits are given in ADC codes.  If oversampling is used, the units must
   * be scaled appropriately.
   *
   * @param lower The lower limit of the trigger in ADC codes (12-bit values).
   * @param upper The upper limit of the trigger in ADC codes (12-bit values).
   */
  void SetLimitsRaw(int lower, int upper);

  /**
   * Configure the analog trigger to use the averaged vs. raw values.
   *
   * If the value is true, then the averaged value is selected for the analog
   * trigger, otherwise the immediate value is used.
   *
   * @param useAveragedValue If true, use the Averaged value, otherwise use the
   *                         instantaneous reading
   */
  void SetAveraged(bool useAveragedValue);

  /**
   * Configure the analog trigger to use a filtered value.
   *
   * The analog trigger will operate with a 3 point average rejection filter.
   * This is designed to help with 360 degree pot applications for the period
   * where the pot crosses through zero.
   *
   * @param useFilteredValue If true, use the 3 point rejection filter,
   *                         otherwise use the unfiltered value
   */
  void SetFiltered(bool useFilteredValue);

  /**
   * Return the index of the analog trigger.
   *
   * This is the FPGA index of this analog trigger instance.
   *
   * @return The index of the analog trigger.
   */
  int GetIndex() const;

  /**
   * Return the InWindow output of the analog trigger.
   *
   * True if the analog input is between the upper and lower limits.
   *
   * @return True if the analog input is between the upper and lower limits.
   */
  bool GetInWindow();

  /**
   * Return the TriggerState output of the analog trigger.
   *
   * True if above upper limit.
   * False if below lower limit.
   * If in Hysteresis, maintain previous state.
   *
   * @return True if above upper limit. False if below lower limit. If in
   *         Hysteresis, maintain previous state.
   */
  bool GetTriggerState();

  /**
   * Creates an AnalogTriggerOutput object.
   *
   * @param type An enum of the type of output object to create.
   * @return A pointer to a new AnalogTriggerOutput object.
   */
  std::shared_ptr<AnalogTriggerOutput> CreateOutput(
      AnalogTriggerType type) const;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  int GetSourceChannel() const;

  std::shared_ptr<AnalogInput> m_analogInput;
  std::shared_ptr<DutyCycle> m_dutyCycle;
  hal::Handle<HAL_AnalogTriggerHandle, HAL_CleanAnalogTrigger> m_trigger;
  bool m_ownsAnalog = false;
};

}  // namespace frc
