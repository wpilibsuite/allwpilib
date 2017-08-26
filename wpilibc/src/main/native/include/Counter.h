/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include <HAL/Counter.h>
#include <HAL/Types.h>

#include "AnalogTrigger.h"
#include "CounterBase.h"
#include "LiveWindow/LiveWindowSendable.h"
#include "SensorBase.h"

namespace frc {

class DigitalGlitchFilter;

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
class Counter : public SensorBase,
                public CounterBase,
                public LiveWindowSendable {
 public:
  enum Mode {
    kTwoPulse = 0,
    kSemiperiod = 1,
    kPulseLength = 2,
    kExternalDirection = 3
  };
  explicit Counter(Mode mode = kTwoPulse);
  explicit Counter(int channel);
  explicit Counter(DigitalSource* source);
  explicit Counter(std::shared_ptr<DigitalSource> source);
  explicit Counter(const AnalogTrigger& trigger);
  Counter(EncodingType encodingType, DigitalSource* upSource,
          DigitalSource* downSource, bool inverted);
  Counter(EncodingType encodingType, std::shared_ptr<DigitalSource> upSource,
          std::shared_ptr<DigitalSource> downSource, bool inverted);
  virtual ~Counter();

  void SetUpSource(int channel);
  void SetUpSource(AnalogTrigger* analogTrigger, AnalogTriggerType triggerType);
  void SetUpSource(std::shared_ptr<AnalogTrigger> analogTrigger,
                   AnalogTriggerType triggerType);
  void SetUpSource(DigitalSource* source);
  void SetUpSource(std::shared_ptr<DigitalSource> source);
  void SetUpSource(DigitalSource& source);
  void SetUpSourceEdge(bool risingEdge, bool fallingEdge);
  void ClearUpSource();

  void SetDownSource(int channel);
  void SetDownSource(AnalogTrigger* analogTrigger,
                     AnalogTriggerType triggerType);
  void SetDownSource(std::shared_ptr<AnalogTrigger> analogTrigger,
                     AnalogTriggerType triggerType);
  void SetDownSource(DigitalSource* source);
  void SetDownSource(std::shared_ptr<DigitalSource> source);
  void SetDownSource(DigitalSource& source);
  void SetDownSourceEdge(bool risingEdge, bool fallingEdge);
  void ClearDownSource();

  void SetUpDownCounterMode();
  void SetExternalDirectionMode();
  void SetSemiPeriodMode(bool highSemiPeriod);
  void SetPulseLengthMode(double threshold);

  void SetReverseDirection(bool reverseDirection);

  // CounterBase interface
  int Get() const override;
  void Reset() override;
  double GetPeriod() const override;
  void SetMaxPeriod(double maxPeriod) override;
  void SetUpdateWhenEmpty(bool enabled);
  bool GetStopped() const override;
  bool GetDirection() const override;

  void SetSamplesToAverage(int samplesToAverage);
  int GetSamplesToAverage() const;
  int GetFPGAIndex() const { return m_index; }

  void UpdateTable() override;
  void StartLiveWindowMode() override;
  void StopLiveWindowMode() override;
  std::string GetSmartDashboardType() const override;
  void InitTable(std::shared_ptr<ITable> subTable) override;
  std::shared_ptr<ITable> GetTable() const override;

 protected:
  // Makes the counter count up.
  std::shared_ptr<DigitalSource> m_upSource;
  // Makes the counter count down.
  std::shared_ptr<DigitalSource> m_downSource;
  // The FPGA counter object
  HAL_CounterHandle m_counter = HAL_kInvalidHandle;

 private:
  int m_index = 0;  ///< The index of this counter.

  std::shared_ptr<ITable> m_table;
  friend class DigitalGlitchFilter;
};

}  // namespace frc
