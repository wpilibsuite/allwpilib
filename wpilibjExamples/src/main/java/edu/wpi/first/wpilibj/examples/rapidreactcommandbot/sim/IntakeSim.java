// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.rapidreactcommandbot.sim;

import edu.wpi.first.wpilibj.DoubleSolenoid;
import edu.wpi.first.wpilibj.PneumaticsModuleType;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.Constants.IntakeConstants;
import edu.wpi.first.wpilibj.simulation.DoubleSolenoidSim;
import edu.wpi.first.wpilibj.simulation.PWMSim;

/** Simulation controller for the intake subsystem. */
public class IntakeSim {
  private final PWMSim m_motor = new PWMSim(IntakeConstants.kMotorPort);
  private final DoubleSolenoidSim m_piston = new DoubleSolenoidSim(PneumaticsModuleType.CTREPCM, 
                      IntakeConstants.kSolenoidPorts[0], IntakeConstants.kSolenoidPorts[1]);

  /** Call this to advance the simulation by 20 ms. */
  public void simulationPeriodic() {}

  /**
   * Get the duty cycle commanded to the intake motor.
   * 
   * @return currently-commanded duty cycle
   */
  public double getMotor() {
    return m_motor.getSpeed();
  }

  /**
   * Is the intake currently deployed.
   * 
   * @return true if deployed.
   */
  public boolean isDeployed() {
    return m_piston.get() == DoubleSolenoid.Value.kForward;
  }

  public void reset() {
    m_motor.resetData();
    m_piston.getModuleSim().resetData();
  }
}
