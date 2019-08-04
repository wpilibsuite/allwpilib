/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

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
  virtual double GetPeriod() const = 0;
  virtual void SetMaxPeriod(double maxPeriod) = 0;
  virtual bool GetStopped() const = 0;
  virtual bool GetDirection() const = 0;
};

}  // namespace frc
