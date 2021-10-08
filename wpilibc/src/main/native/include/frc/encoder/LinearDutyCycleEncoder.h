// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <hal/SimDevice.h>
#include <hal/Types.h>
#include <units/angle.h>
#include <units/length.h>
#include <units/velocity.h>

#include "frc/AnalogTrigger.h"
#include "frc/Counter.h"
#include "frc/DigitalSource.h"
#include "frc/DutyCycle.h"
#include "frc/encoder/LinearEncoder.h"

namespace frc {

class LinearDutyCycleEncoder : public LinearEncoder {
 public:
  LinearDutyCycleEncoder(int channel, decltype(1.0 / 1_m) cyclesPerMeter);
  LinearDutyCycleEncoder(DutyCycle& dutyCycle,
                         decltype(1.0 / 1_m) cyclesPerMeter);
  LinearDutyCycleEncoder(std::shared_ptr<DutyCycle> dutyCycle,
                         decltype(1.0 / 1_m) cyclesPerMeter);
  LinearDutyCycleEncoder(DigitalSource& digitalSource,
                         decltype(1.0 / 1_m) cyclesPerMeter);
  LinearDutyCycleEncoder(std::shared_ptr<DigitalSource> digitalSource,
                         decltype(1.0 / 1_m) cyclesPerMeter);

  LinearDutyCycleEncoder(LinearDutyCycleEncoder&&) = default;
  LinearDutyCycleEncoder& operator=(LinearDutyCycleEncoder&&) = default;

  units::meter_t GetDisplacement() const override;

  units::meters_per_second_t GetVelocity() const override;

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

  void Init();
};

}  // namespace frc
