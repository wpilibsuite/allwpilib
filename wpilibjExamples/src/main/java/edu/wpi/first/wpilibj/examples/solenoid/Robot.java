// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.solenoid;

import edu.wpi.first.wpilibj.Compressor;
import edu.wpi.first.wpilibj.DoubleSolenoid;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.PneumaticsModuleType;
import edu.wpi.first.wpilibj.Solenoid;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.shuffleboard.Shuffleboard;
import edu.wpi.first.wpilibj.shuffleboard.ShuffleboardTab;

/**
 * This is a sample program showing the use of the solenoid classes during operator control. Three
 * buttons from a joystick will be used to control two solenoids: One button to control the position
 * of a single solenoid and the other two buttons to control a double solenoid. Single solenoids can
 * either be on or off, such that the air diverted through them goes through either one channel or
 * the other. Double solenoids have three states: Off, Forward, and Reverse. Forward and Reverse
 * divert the air through the two channels and correspond to the on and off of a single solenoid,
 * but a double solenoid can also be "off", where the solenoid will remain in its default power off
 * state. Additionally, double solenoids take up two channels on your PCM whereas single solenoids
 * only take a single channel.
 */
public class Robot extends TimedRobot {
  private final Joystick m_stick = new Joystick(0);

  // Solenoid corresponds to a single solenoid.
  // In this case, it's connected to channel 0 of a PH with the default CAN ID.
  private final Solenoid m_solenoid = new Solenoid(PneumaticsModuleType.REVPH, 0);

  // DoubleSolenoid corresponds to a double solenoid.
  // In this case, it's connected to channels 1 and 2 of a PH with the default CAN ID.
  private final DoubleSolenoid m_doubleSolenoid =
      new DoubleSolenoid(PneumaticsModuleType.REVPH, 1, 2);

  // Compressor connected to a PH with a default CAN ID (1)
  private final Compressor m_compressor = new Compressor(PneumaticsModuleType.REVPH);

  static final int kSolenoidButton = 1;
  static final int kDoubleSolenoidForwardButton = 2;
  static final int kDoubleSolenoidReverseButton = 3;
  static final int kCompressorButton = 4;

  /** Called once at the beginning of the robot program. */
  public Robot() {
    // Publish elements to shuffleboard.
    ShuffleboardTab tab = Shuffleboard.getTab("Pneumatics");
    tab.add("Single Solenoid", m_solenoid);
    tab.add("Double Solenoid", m_doubleSolenoid);
    tab.add("Compressor", m_compressor);

    // Also publish some raw data
    // Get the pressure (in PSI) from the analog sensor connected to the PH.
    // This function is supported only on the PH!
    // On a PCM, this function will return 0.
    tab.addDouble("PH Pressure [PSI]", m_compressor::getPressure);
    // Get compressor current draw.
    tab.addDouble("Compressor Current", m_compressor::getCurrent);
    // Get whether the compressor is active.
    tab.addBoolean("Compressor Active", m_compressor::isEnabled);
    // Get the digital pressure switch connected to the PCM/PH.
    // The switch is open when the pressure is over ~120 PSI.
    tab.addBoolean("Pressure Switch", m_compressor::getPressureSwitchValue);
  }

  @SuppressWarnings("PMD.UnconditionalIfStatement")
  @Override
  public void teleopPeriodic() {
    /*
     * The output of GetRawButton is true/false depending on whether
     * the button is pressed; Set takes a boolean for whether
     * to retract the solenoid (false) or extend it (true).
     */
    m_solenoid.set(m_stick.getRawButton(kSolenoidButton));

    /*
     * GetRawButtonPressed will only return true once per press.
     * If a button is pressed, set the solenoid to the respective channel.
     */
    if (m_stick.getRawButtonPressed(kDoubleSolenoidForwardButton)) {
      m_doubleSolenoid.set(DoubleSolenoid.Value.kForward);
    } else if (m_stick.getRawButtonPressed(kDoubleSolenoidReverseButton)) {
      m_doubleSolenoid.set(DoubleSolenoid.Value.kReverse);
    }

    // On button press, toggle the compressor.
    if (m_stick.getRawButtonPressed(kCompressorButton)) {
      // Check whether the compressor is currently enabled.
      boolean isCompressorEnabled = m_compressor.isEnabled();
      if (isCompressorEnabled) {
        // Disable closed-loop mode on the compressor.
        m_compressor.disable();
      } else {
        // Change the if statements to select the closed-loop you want to use:
        if (false) {
          // Enable closed-loop mode based on the digital pressure switch connected to the PCM/PH.
          // The switch is open when the pressure is over ~120 PSI.
          m_compressor.enableDigital();
        }
        if (true) {
          // Enable closed-loop mode based on the analog pressure sensor connected to the PH.
          // The compressor will run while the pressure reported by the sensor is in the
          // specified range ([70 PSI, 120 PSI] in this example).
          // Analog mode exists only on the PH! On the PCM, this enables digital control.
          m_compressor.enableAnalog(70, 120);
        }
        if (false) {
          // Enable closed-loop mode based on both the digital pressure switch AND the analog
          // pressure sensor connected to the PH.
          // The compressor will run while the pressure reported by the analog sensor is in the
          // specified range ([70 PSI, 120 PSI] in this example) AND the digital switch reports
          // that the system is not full.
          // Hybrid mode exists only on the PH! On the PCM, this enables digital control.
          m_compressor.enableHybrid(70, 120);
        }
      }
    }
  }
}
