// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.hpp"

#include "wpi/smartdashboard/SmartDashboard.hpp"
#include "wpi/units/pressure.hpp"

Robot::Robot() {
  // Publish elements to shuffleboard.
  wpi::SmartDashboard::PutData("Single Solenoid", &solenoid);
  wpi::SmartDashboard::PutData("Double Solenoid", &doubleSolenoid);
  wpi::SmartDashboard::PutData("Compressor", &compressor);
}

void Robot::TeleopPeriodic() {
  // Publish some raw data

  // Get the pressure (in PSI) from the analog sensor connected to the PH.
  // This function is supported only on the PH!
  // On a PCM, this function will return 0.
  wpi::SmartDashboard::PutNumber("PH Pressure [PSI]",
                                 compressor.GetPressure().value());
  // Get compressor current draw.
  wpi::SmartDashboard::PutNumber("Compressor Current",
                                 compressor.GetCurrent().value());
  // Get whether the compressor is active.
  wpi::SmartDashboard::PutBoolean("Compressor Active", compressor.IsEnabled());
  // Get the digital pressure switch connected to the PCM/PH.
  // The switch is open when the pressure is over ~120 PSI.
  wpi::SmartDashboard::PutBoolean("Pressure Switch",
                                  compressor.GetPressureSwitchValue());

  /*
   * The output of GetRawButton is true/false depending on whether
   * the button is pressed; Set takes a boolean for whether
   * to retract the solenoid (false) or extend it (true).
   */
  solenoid.Set(stick.GetRawButton(kSolenoidButton));

  /*
   * GetRawButtonPressed will only return true once per press.
   * If a button is pressed, set the solenoid to the respective channel.
   */
  if (stick.GetRawButtonPressed(kDoubleSolenoidForward)) {
    doubleSolenoid.Set(wpi::DoubleSolenoid::FORWARD);
  } else if (stick.GetRawButtonPressed(kDoubleSolenoidReverse)) {
    doubleSolenoid.Set(wpi::DoubleSolenoid::REVERSE);
  }

  // On button press, toggle the compressor with the mode selected from the
  // dashboard.
  if (stick.GetRawButtonPressed(kCompressorButton)) {
    // Check whether the compressor is currently enabled.
    bool isCompressorEnabled = compressor.IsEnabled();
    if (isCompressorEnabled) {
      // Disable closed-loop mode on the compressor.
      compressor.Disable();
    } else {
      // Change the if directives to select the closed-loop mode you want to
      // use:
#if 0
      // Enable closed-loop mode based on the digital pressure switch
      // connected to the PCM/PH.
      compressor.EnableDigital();
#endif
#if 1
      // Enable closed-loop mode based on the analog pressure sensor connected
      // to the PH. The compressor will run while the pressure reported by the
      // sensor is in the specified range ([70 PSI, 120 PSI] in this example).
      // Analog mode exists only on the PH! On the PCM, this enables digital
      // control.
      compressor.EnableAnalog(70_psi, 120_psi);
#endif
#if 0
      // Enable closed-loop mode based on both the digital pressure switch AND the analog
      // pressure sensor connected to the PH.
      // The compressor will run while the pressure reported by the analog sensor is in the
      // specified range ([70 PSI, 120 PSI] in this example) AND the digital switch reports
      // that the system is not full.
      // Hybrid mode exists only on the PH! On the PCM, this enables digital control.
      compressor.EnableHybrid(70_psi, 120_psi);
#endif
    }
  }
}

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
