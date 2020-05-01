/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.hatchbotinlined.subsystems;

import edu.wpi.first.wpilibj.DoubleSolenoid;
import edu.wpi.first.wpilibj2.command.SubsystemBase;

import edu.wpi.first.wpilibj.examples.hatchbotinlined.Constants.HatchConstants;

import static edu.wpi.first.wpilibj.DoubleSolenoid.Value.kForward;
import static edu.wpi.first.wpilibj.DoubleSolenoid.Value.kReverse;

/**
 * A hatch mechanism actuated by a single {@link edu.wpi.first.wpilibj.DoubleSolenoid}.
 */
public class HatchSubsystem extends SubsystemBase {
  private final DoubleSolenoid m_hatchSolenoid =
      new DoubleSolenoid(HatchConstants.kHatchSolenoidModule, HatchConstants.kHatchSolenoidPorts[0],
                         HatchConstants.kHatchSolenoidPorts[1]);

  /**
   * Grabs the hatch.
   */
  public void grabHatch() {
    m_hatchSolenoid.set(kForward);
  }

  /**
   * Releases the hatch.
   */
  public void releaseHatch() {
    m_hatchSolenoid.set(kReverse);
  }
}
