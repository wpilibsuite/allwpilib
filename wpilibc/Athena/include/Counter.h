/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "HAL/HAL.hpp"
#include "AnalogTriggerOutput.h"
#include "CounterBase.h"
#include "SensorBase.h"
#include "LiveWindow/LiveWindowSendable.h"

#include <memory>

class DigitalGlitchFilter;

/**
 * Class for counting the number of ticks on a digital input channel.
 * This is a general purpose class for counting repetitive events. It can return
 * the number
 * of counts, the period of the most recent cycle, and detect when the signal
 * being counted
 * has stopped by supplying a maximum cycle time.
 *
 * All counters will immediately start counting - Reset() them if you need them
 * to be zeroed before use.
 */
class Counter : public SensorBase,
                public CounterBase,
                public LiveWindowSendable {
 public:
  explicit Counter(Mode mode = kTwoPulse);
  explicit Counter(int32_t channel);
  explicit Counter(DigitalSource *source);
  explicit Counter(std::shared_ptr<DigitalSource> source);
  DEPRECATED("Use pass-by-reference instead.")
  explicit Counter(AnalogTrigger *trigger);
  explicit Counter(const AnalogTrigger &trigger);
  Counter(EncodingType encodingType, DigitalSource *upSource,
          DigitalSource *downSource, bool inverted);
  Counter(EncodingType encodingType, std::shared_ptr<DigitalSource> upSource,
          std::shared_ptr<DigitalSource> downSource, bool inverted);
  virtual ~Counter();

  void SetUpSource(int32_t channel);
  void SetUpSource(AnalogTrigger *analogTrigger, AnalogTriggerType triggerType);
  void SetUpSource(std::shared_ptr<AnalogTrigger> analogTrigger,
                   AnalogTriggerType triggerType);
  void SetUpSource(DigitalSource *source);
  void SetUpSource(std::shared_ptr<DigitalSource> source);
  void SetUpSource(DigitalSource &source);
  void SetUpSourceEdge(bool risingEdge, bool fallingEdge);
  void ClearUpSource();

  void SetDownSource(int32_t channel);
  void SetDownSource(AnalogTrigger *analogTrigger,
                     AnalogTriggerType triggerType);
  void SetDownSource(std::shared_ptr<AnalogTrigger> analogTrigger,
                     AnalogTriggerType triggerType);
  void SetDownSource(DigitalSource *source);
  void SetDownSource(std::shared_ptr<DigitalSource> source);
  void SetDownSource(DigitalSource &source);
  void SetDownSourceEdge(bool risingEdge, bool fallingEdge);
  void ClearDownSource();

  void SetUpDownCounterMode();
  void SetExternalDirectionMode();
  void SetSemiPeriodMode(bool highSemiPeriod);
  void SetPulseLengthMode(float threshold);

  void SetReverseDirection(bool reverseDirection);

  // CounterBase interface
  int32_t Get() const override;
  void Reset() override;
  double GetPeriod() const override;
  void SetMaxPeriod(double maxPeriod) override;
  void SetUpdateWhenEmpty(bool enabled);
  bool GetStopped() const override;
  bool GetDirection() const override;

  void SetSamplesToAverage(int samplesToAverage);
  int GetSamplesToAverage() const;
  uint32_t GetFPGAIndex() const { return m_index; }

  void UpdateTable() override;
  void StartLiveWindowMode() override;
  void StopLiveWindowMode() override;
  virtual std::string GetSmartDashboardType() const override;
  void InitTable(std::shared_ptr<ITable> subTable) override;
  std::shared_ptr<ITable> GetTable() const override;

 protected:
  // Makes the counter count up.
  std::shared_ptr<DigitalSource> m_upSource;
  // Makes the counter count down.
  std::shared_ptr<DigitalSource> m_downSource;
  // The FPGA counter object
  void *m_counter = nullptr;              ///< The FPGA counter object.
 private:
  uint32_t m_index = 0;            ///< The index of this counter.

  std::shared_ptr<ITable> m_table;
  friend class DigitalGlitchFilter;
};
