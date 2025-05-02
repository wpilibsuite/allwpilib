// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/AnalogPotentiometer.h>
#include <frc/Compressor.h>
#include <frc/PneumaticsControlModule.h>
#include <frc2/command/CommandPtr.h>
#include <frc2/command/SubsystemBase.h>
#include <units/pressure.h>

#include "Constants.h"

class Pneumatics : frc2::SubsystemBase {
 public:
  Pneumatics();
  /** Returns a command that disables the compressor indefinitely. */
  [[nodiscard]]
  frc2::CommandPtr DisableCompressorCommand();

  /**
   * Query the analog pressure sensor.
   *
   * @return the measured pressure, in PSI
   */
  units::pounds_per_square_inch_t GetPressure();

 private:
  // External analog pressure sensor
  // product-specific voltage->pressure conversion, see product manual
  // in this case, 250(V/5)-25
  // the scale parameter in the AnalogPotentiometer constructor is scaled from
  // 1 instead of 5, so if r is the raw AnalogPotentiometer output, the
  // pressure is 250r-25
  static constexpr double SCALE = 250;
  static constexpr double OFFSET = -25;
  frc::AnalogPotentiometer m_pressureTransducer{/* the AnalogIn port*/ 2,
                                                SCALE, OFFSET};

  // Compressor connected to a PH with a default CAN ID
  frc::Compressor m_compressor{0, frc::PneumaticsModuleType::CTREPCM};
};
