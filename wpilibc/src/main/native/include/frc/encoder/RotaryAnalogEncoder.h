// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <hal/SimDevice.h>
#include <hal/Types.h>
#include <units/angle.h>
#include <units/angular_velocity.h>
#include <units/voltage.h>

#include "frc/AnalogInput.h"
#include "frc/AnalogTrigger.h"
#include "frc/Counter.h"
#include "frc/DigitalSource.h"
#include "frc/encoder/RotaryEncoder.h"

namespace frc {

class RotaryAnalogEncoder : public RotaryEncoder {
 public:
  RotaryAnalogEncoder(int channel, decltype(1_V / 1_tr) voltsPerRevolution);
  RotaryAnalogEncoder(AnalogInput& analogInput,
                      decltype(1_V / 1_tr) voltsPerRevolution);
  RotaryAnalogEncoder(std::shared_ptr<AnalogInput> analogInput,
                      decltype(1_V / 1_tr) voltsPerRevolution);

  RotaryAnalogEncoder(RotaryAnalogEncoder&&) = default;
  RotaryAnalogEncoder& operator=(RotaryAnalogEncoder&&) = default;

  units::radian_t GetAngle() const override;

  units::radians_per_second_t GetAngularVelocity() const override;

  void SetInverted(bool inverted) override;

  void Reset() override;

  void SetSimDevice(HAL_SimDeviceHandle device);

  int GetChannel() const;

 private:
  std::shared_ptr<AnalogInput> m_analogInput;
  AnalogTrigger m_analogTrigger;
  Counter m_counter;
  double m_positionOffset = 0;
  double m_distancePerRotation = 1.0;
  mutable units::turn_t m_lastPosition{0.0};

  hal::SimDevice m_simDevice;
  hal::SimDouble m_simPosition;
};

}  // namespace frc
