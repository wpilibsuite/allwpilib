// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <hal/Types.h>

#include "frc/AnalogTrigger.h"
#include "frc/CounterBase.h"
#include "frc/ErrorBase.h"
#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc {

class DigitalGlitchFilter;
class SendableBuilder;
class DMA;
class DMASample;

/**
 * Class for counting the number of ticks on a digital input channel.
 *
 * This is a general purpose class for counting repetitive events. It can return
 * the number of counts, the period of the most recent cycle, and detect when
 * the signal being counted has stopped by supplying a maximum cycle time.
 *
 * All counters will immediately start counting - Reset() them if you need them
 * to be zeroed before use.
 */
class Counter : public ErrorBase,
                public CounterBase,
                public Sendable,
                public SendableHelper<Counter> {
  friend class DMA;
  friend class DMASample;

 public:
  enum Mode {
    kTwoPulse = 0,
    kSemiperiod = 1,
    kPulseLength = 2,
    kExternalDirection = 3
  };

  /**
   * Create an instance of a counter where no sources are selected.
   *
   * They all must be selected by calling functions to specify the upsource and
   * the downsource independently.
   *
   * This creates a ChipObject counter and initializes status variables
   * appropriately.
   *
   * The counter will start counting immediately.
   *
   * @param mode The counter mode
   */
  explicit Counter(Mode mode = kTwoPulse);

  /**
   * Create an instance of a Counter object.
   *
   * Create an up-Counter instance given a channel.
   *
   * The counter will start counting immediately.
   *
   * @param channel The DIO channel to use as the up source. 0-9 are on-board,
   *                10-25 are on the MXP
   */
  explicit Counter(int channel);

  /**
   * Create an instance of a counter from a Digital Source (such as a Digital
   * Input).
   *
   * This is used if an existing digital input is to be shared by multiple other
   * objects such as encoders or if the Digital Source is not a Digital Input
   * channel (such as an Analog Trigger).
   *
   * The counter will start counting immediately.
   * @param source A pointer to the existing DigitalSource object. It will be
   *               set as the Up Source.
   */
  explicit Counter(DigitalSource* source);

  /**
   * Create an instance of a counter from a Digital Source (such as a Digital
   * Input).
   *
   * This is used if an existing digital input is to be shared by multiple other
   * objects such as encoders or if the Digital Source is not a Digital Input
   * channel (such as an Analog Trigger).
   *
   * The counter will start counting immediately.
   *
   * @param source A pointer to the existing DigitalSource object. It will be
   *               set as the Up Source.
   */
  explicit Counter(std::shared_ptr<DigitalSource> source);

  /**
   * Create an instance of a Counter object.
   *
   * Create an instance of a simple up-Counter given an analog trigger.
   * Use the trigger state output from the analog trigger.
   *
   * The counter will start counting immediately.
   *
   * @param trigger The reference to the existing AnalogTrigger object.
   */
  explicit Counter(const AnalogTrigger& trigger);

  /**
   * Create an instance of a Counter object.
   *
   * Creates a full up-down counter given two Digital Sources.
   *
   * @param encodingType The quadrature decoding mode (1x or 2x)
   * @param upSource     The pointer to the DigitalSource to set as the up
   *                     source
   * @param downSource   The pointer to the DigitalSource to set as the down
   *                     source
   * @param inverted     True to invert the output (reverse the direction)
   */
  Counter(EncodingType encodingType, DigitalSource* upSource,
          DigitalSource* downSource, bool inverted);

  /**
   * Create an instance of a Counter object.
   *
   * Creates a full up-down counter given two Digital Sources.
   *
   * @param encodingType The quadrature decoding mode (1x or 2x)
   * @param upSource     The pointer to the DigitalSource to set as the up
   *                     source
   * @param downSource   The pointer to the DigitalSource to set as the down
   *                     source
   * @param inverted     True to invert the output (reverse the direction)
   */
  Counter(EncodingType encodingType, std::shared_ptr<DigitalSource> upSource,
          std::shared_ptr<DigitalSource> downSource, bool inverted);

  ~Counter() override;

  Counter(Counter&&) = default;
  Counter& operator=(Counter&&) = default;

  /**
   * Set the upsource for the counter as a digital input channel.
   *
   * @param channel The DIO channel to use as the up source. 0-9 are on-board,
   *                10-25 are on the MXP
   */
  void SetUpSource(int channel);

  /**
   * Set the up counting source to be an analog trigger.
   *
   * @param analogTrigger The analog trigger object that is used for the Up
   *                      Source
   * @param triggerType   The analog trigger output that will trigger the
   *                      counter.
   */
  void SetUpSource(AnalogTrigger* analogTrigger, AnalogTriggerType triggerType);

  /**
   * Set the up counting source to be an analog trigger.
   *
   * @param analogTrigger The analog trigger object that is used for the Up
   *                      Source
   * @param triggerType   The analog trigger output that will trigger the
   *                      counter.
   */
  void SetUpSource(std::shared_ptr<AnalogTrigger> analogTrigger,
                   AnalogTriggerType triggerType);

  void SetUpSource(DigitalSource* source);

  /**
   * Set the source object that causes the counter to count up.
   *
   * Set the up counting DigitalSource.
   *
   * @param source Pointer to the DigitalSource object to set as the up source
   */
  void SetUpSource(std::shared_ptr<DigitalSource> source);

  /**
   * Set the source object that causes the counter to count up.
   *
   * Set the up counting DigitalSource.
   *
   * @param source Reference to the DigitalSource object to set as the up source
   */
  void SetUpSource(DigitalSource& source);

  /**
   * Set the edge sensitivity on an up counting source.
   *
   * Set the up source to either detect rising edges or falling edges or both.
   *
   * @param risingEdge  True to trigger on rising edges
   * @param fallingEdge True to trigger on falling edges
   */
  void SetUpSourceEdge(bool risingEdge, bool fallingEdge);

  /**
   * Disable the up counting source to the counter.
   */
  void ClearUpSource();

  /**
   * Set the down counting source to be a digital input channel.
   *
   * @param channel The DIO channel to use as the up source. 0-9 are on-board,
   *                10-25 are on the MXP
   */
  void SetDownSource(int channel);

  /**
   * Set the down counting source to be an analog trigger.
   *
   * @param analogTrigger The analog trigger object that is used for the Down
   *                      Source
   * @param triggerType   The analog trigger output that will trigger the
   *                      counter.
   */
  void SetDownSource(AnalogTrigger* analogTrigger,
                     AnalogTriggerType triggerType);

  /**
   * Set the down counting source to be an analog trigger.
   *
   * @param analogTrigger The analog trigger object that is used for the Down
   *                      Source
   * @param triggerType   The analog trigger output that will trigger the
   *                      counter.
   */
  void SetDownSource(std::shared_ptr<AnalogTrigger> analogTrigger,
                     AnalogTriggerType triggerType);

  /**
   * Set the source object that causes the counter to count down.
   *
   * Set the down counting DigitalSource.
   *
   * @param source Pointer to the DigitalSource object to set as the down source
   */
  void SetDownSource(DigitalSource* source);

  /**
   * Set the source object that causes the counter to count down.
   *
   * Set the down counting DigitalSource.
   *
   * @param source Reference to the DigitalSource object to set as the down
   *               source
   */
  void SetDownSource(DigitalSource& source);

  void SetDownSource(std::shared_ptr<DigitalSource> source);

  /**
   * Set the edge sensitivity on a down counting source.
   *
   * Set the down source to either detect rising edges or falling edges.
   *
   * @param risingEdge  True to trigger on rising edges
   * @param fallingEdge True to trigger on falling edges
   */
  void SetDownSourceEdge(bool risingEdge, bool fallingEdge);

  /**
   * Disable the down counting source to the counter.
   */
  void ClearDownSource();

  /**
   * Set standard up / down counting mode on this counter.
   *
   * Up and down counts are sourced independently from two inputs.
   */
  void SetUpDownCounterMode();

  /**
   * Set external direction mode on this counter.
   *
   * Counts are sourced on the Up counter input.
   * The Down counter input represents the direction to count.
   */
  void SetExternalDirectionMode();

  /**
   * Set Semi-period mode on this counter.
   *
   * Counts up on both rising and falling edges.
   */
  void SetSemiPeriodMode(bool highSemiPeriod);

  /**
   * Configure the counter to count in up or down based on the length of the
   * input pulse.
   *
   * This mode is most useful for direction sensitive gear tooth sensors.
   *
   * @param threshold The pulse length beyond which the counter counts the
   *                  opposite direction. Units are seconds.
   */
  void SetPulseLengthMode(double threshold);

  /**
   * Set the Counter to return reversed sensing on the direction.
   *
   * This allows counters to change the direction they are counting in the case
   * of 1X and 2X quadrature encoding only. Any other counter mode isn't
   * supported.
   *
   * @param reverseDirection true if the value counted should be negated.
   */
  void SetReverseDirection(bool reverseDirection);

  /**
   * Set the Samples to Average which specifies the number of samples of the
   * timer to average when calculating the period. Perform averaging to account
   * for mechanical imperfections or as oversampling to increase resolution.
   *
   * @param samplesToAverage The number of samples to average from 1 to 127.
   */
  void SetSamplesToAverage(int samplesToAverage);

  /**
   * Get the Samples to Average which specifies the number of samples of the
   * timer to average when calculating the period.
   *
   * Perform averaging to account for mechanical imperfections or as
   * oversampling to increase resolution.
   *
   * @return The number of samples being averaged (from 1 to 127)
   */
  int GetSamplesToAverage() const;

  int GetFPGAIndex() const;

  // CounterBase interface
  /**
   * Read the current counter value.
   *
   * Read the value at this instant. It may still be running, so it reflects the
   * current value. Next time it is read, it might have a different value.
   */
  int Get() const override;

  /**
   * Reset the Counter to zero.
   *
   * Set the counter value to zero. This doesn't effect the running state of the
   * counter, just sets the current value to zero.
   */
  void Reset() override;

  /**
   * Get the Period of the most recent count.
   *
   * Returns the time interval of the most recent count. This can be used for
   * velocity calculations to determine shaft speed.
   *
   * @returns The period between the last two pulses in units of seconds.
   */
  double GetPeriod() const override;

  /**
   * Set the maximum period where the device is still considered "moving".
   *
   * Sets the maximum period where the device is considered moving. This value
   * is used to determine the "stopped" state of the counter using the
   * GetStopped method.
   *
   * @param maxPeriod The maximum period where the counted device is considered
   *                  moving in seconds.
   */
  void SetMaxPeriod(double maxPeriod) final;

  /**
   * Select whether you want to continue updating the event timer output when
   * there are no samples captured.
   *
   * The output of the event timer has a buffer of periods that are averaged and
   * posted to a register on the FPGA.  When the timer detects that the event
   * source has stopped (based on the MaxPeriod) the buffer of samples to be
   * averaged is emptied.  If you enable the update when empty, you will be
   * notified of the stopped source and the event time will report 0 samples.
   * If you disable update when empty, the most recent average will remain on
   * the output until a new sample is acquired.  You will never see 0 samples
   * output (except when there have been no events since an FPGA reset) and you
   * will likely not see the stopped bit become true (since it is updated at the
   * end of an average and there are no samples to average).
   *
   * @param enabled True to enable update when empty
   */
  void SetUpdateWhenEmpty(bool enabled);

  /**
   * Determine if the clock is stopped.
   *
   * Determine if the clocked input is stopped based on the MaxPeriod value set
   * using the SetMaxPeriod method. If the clock exceeds the MaxPeriod, then the
   * device (and counter) are assumed to be stopped and it returns true.
   *
   * @return Returns true if the most recent counter period exceeds the
   *         MaxPeriod value set by SetMaxPeriod.
   */
  bool GetStopped() const override;

  /**
   * The last direction the counter value changed.
   *
   * @return The last direction the counter value changed.
   */
  bool GetDirection() const override;

  void InitSendable(SendableBuilder& builder) override;

 protected:
  // Makes the counter count up.
  std::shared_ptr<DigitalSource> m_upSource;

  // Makes the counter count down.
  std::shared_ptr<DigitalSource> m_downSource;

  // The FPGA counter object
  hal::Handle<HAL_CounterHandle> m_counter;

 private:
  int m_index = 0;  // The index of this counter.

  friend class DigitalGlitchFilter;
};

}  // namespace frc
