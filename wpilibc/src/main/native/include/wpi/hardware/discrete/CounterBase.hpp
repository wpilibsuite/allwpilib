// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <units/time.h>

namespace frc {

/**
 * Interface for counting the number of ticks on a digital input channel.
 *
 * Encoders, Gear tooth sensors, and counters should all subclass this so it can
 * be used to build more advanced classes for control and driving.
 *
 * All counters will immediately start counting - Reset() them if you need them
 * to be zeroed before use.
 */
class CounterBase {
 public:
  enum EncodingType { k1X, k2X, k4X };

  CounterBase() = default;
  virtual ~CounterBase() = default;

  CounterBase(CounterBase&&) = default;
  CounterBase& operator=(CounterBase&&) = default;

  virtual int Get() const = 0;
  virtual void Reset() = 0;
  virtual units::second_t GetPeriod() const = 0;
  virtual void SetMaxPeriod(units::second_t maxPeriod) = 0;
  virtual bool GetStopped() const = 0;
  virtual bool GetDirection() const = 0;
};

}  // namespace frc
