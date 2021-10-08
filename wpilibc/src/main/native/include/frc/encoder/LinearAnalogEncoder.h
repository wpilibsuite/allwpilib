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
#include <units/voltage.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc/AnalogInput.h"
#include "frc/AnalogTrigger.h"
#include "frc/Counter.h"
#include "frc/encoder/LinearEncoder.h"

namespace frc {

class LinearAnalogEncoder : public LinearEncoder,
                            public wpi::Sendable,
                            public wpi::SendableHelper<LinearAnalogEncoder> {
 public:
  LinearAnalogEncoder(int channel, decltype(1_V / 1_m) voltsPerMeter);
  LinearAnalogEncoder(AnalogInput& analogInput,
                      decltype(1_V / 1_m) voltsPerMeter);
  LinearAnalogEncoder(std::shared_ptr<AnalogInput> analogInput,
                      decltype(1_V / 1_m) voltsPerMeter);

  LinearAnalogEncoder(LinearAnalogEncoder&&) = default;
  LinearAnalogEncoder& operator=(LinearAnalogEncoder&&) = default;

  units::meter_t GetDisplacement() const override;

  units::meters_per_second_t GetVelocity() const override;

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
  mutable units::turn_t m_lastPosition = 0_tr;

  hal::SimDevice m_simDevice;
  hal::SimDouble m_simPosition;

  void Init(decltype(1_V / 1_m) voltsPerMeter);
};

}  // namespace frc
