// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <hal/SimDevice.h>
#include <hal/Types.h>
#include <units/angle.h>
#include <units/angular_velocity.h>

#include "frc/AnalogTrigger.h"
#include "frc/Counter.h"
#include "frc/DigitalSource.h"
#include "frc/DutyCycle.h"
#include "frc/encoder/RotaryEncoder.h"

namespace frc {

class RotaryDutyCycleEncoder : public RotaryEncoder {
 public:
  enum class EncodingType { k1X, k2X, k4X };

  RotaryDutyCycleEncoder(int channel, decltype(1.0 / 1_tr) cyclesPerRevolution);
  RotaryDutyCycleEncoder(DutyCycle& dutyCycle,
                         decltype(1.0 / 1_tr) cyclesPerRevolution);
  RotaryDutyCycleEncoder(std::shared_ptr<DutyCycle> dutyCycle,
                         decltype(1.0 / 1_tr) cyclesPerRevolution);
  RotaryDutyCycleEncoder(DigitalSource& digitalSource,
                         decltype(1.0 / 1_tr) cyclesPerRevolution);
  RotaryDutyCycleEncoder(std::shared_ptr<DigitalSource> digitalSource,
                         decltype(1.0 / 1_tr) cyclesPerRevolution);

  RotaryDutyCycleEncoder(RotaryDutyCycleEncoder&&) = default;
  RotaryDutyCycleEncoder& operator=(RotaryDutyCycleEncoder&&) = default;

  units::radian_t GetAngle() const override;

  units::radians_per_second_t GetAngularVelocity() const override;

  void SetInverted(bool inverted) override;

  void Reset() override;

  void SetSimDevice(HAL_SimDeviceHandle device);

  int GetFPGAIndex() const;

 private:
  std::shared_ptr<DutyCycle> m_dutyCycle;
  std::unique_ptr<AnalogTrigger> m_analogTrigger;
  std::unique_ptr<Counter> m_counter;
  int m_frequencyThreshold = 100;
  double m_positionOffset = 0;
  double m_distancePerRotation = 1.0;
  mutable units::turn_t m_lastPosition{0.0};

  hal::SimDevice m_simDevice;
  hal::SimDouble m_simPosition;
  hal::SimDouble m_simDistancePerRotation;
  hal::SimBoolean m_simIsConnected;
};

}  // namespace frc
