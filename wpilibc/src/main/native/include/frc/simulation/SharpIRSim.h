// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/SimDevice.h>
#include <units/length.h>

#include "frc/SharpIR.h"

namespace frc {

/** Simulation class for Sharp IR sensors. */
class SharpIRSim {
 public:
  /**
   * Constructor.
   *
   * @param sharpIR The real sensor to simulate
   */
  explicit SharpIRSim(const SharpIR& sharpIR);

  /**
   * Constructor.
   *
   * @param channel Analog channel for this sensor
   */
  explicit SharpIRSim(int channel);

  /**
   * Set the range returned by the distance sensor.
   *
   * @param rng range of the target returned by the sensor
   */
  void SetRange(units::centimeter_t rng);

 private:
  hal::SimDouble m_simRange;
};

}  // namespace frc
