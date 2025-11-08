// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Pneumatics.hpp"

Pneumatics::Pneumatics() {}

wpi::cmd::CommandPtr Pneumatics::DisableCompressorCommand() {
  return StartEnd(
      [&] {
        // Disable closed-loop mode on the compressor.
        m_compressor.Disable();
      },
      [&] {
        // Enable closed-loop mode based on the digital pressure switch
        // connected to the PCM/PH. The switch is open when the pressure is over
        // ~120 PSI.
        m_compressor.EnableDigital();
      });
}

wpi::units::pounds_per_square_inch_t Pneumatics::GetPressure() {
  // Get the pressure (in PSI) from an analog pressure sensor connected to
  // the RIO.
  return wpi::units::pounds_per_square_inch_t{m_pressureTransducer.Get()};
}
