/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.pacgoat.subsystems;

import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.DoubleSolenoid;
import edu.wpi.first.wpilibj.Solenoid;
import edu.wpi.first.wpilibj.command.Subsystem;

/**
 * The Shooter subsystem handles shooting. The mechanism for shooting is
 * slightly complicated because it has to pneumatic cylinders for shooting, and
 * a third latch to allow the pressure to partially build up and reduce the
 * effect of the airflow. For shorter shots, when full power isn't needed, only
 * one cylinder fires.
 *
 * <p>NOTE: Simulation currently approximates this as as single pneumatic
 * cylinder and ignores the latch.
 */
public class Shooter extends Subsystem {
  // Devices
  DoubleSolenoid m_piston1 = new DoubleSolenoid(1, 3, 4);
  DoubleSolenoid m_piston2 = new DoubleSolenoid(1, 5, 6);
  Solenoid m_latchPiston = new Solenoid(1, 2);
  DigitalInput m_piston1ReedSwitchFront = new DigitalInput(9);
  DigitalInput m_piston1ReedSwitchBack = new DigitalInput(11);
  //NOTE: currently ignored in simulation
  DigitalInput m_hotGoalSensor = new DigitalInput(7);

  /**
   * Create a new shooter subsystem.
   */
  public Shooter() {
    // Put everything to the LiveWindow for testing.
    addChild("Hot Goal Sensor", m_hotGoalSensor);
    addChild("Piston1 Reed Switch Front ", m_piston1ReedSwitchFront);
    addChild("Piston1 Reed Switch Back ", m_piston1ReedSwitchBack);
    addChild("Latch Piston", m_latchPiston);
  }

  /**
   * No default command.
   */
  @Override
  public void initDefaultCommand() {
  }

  /**
   * Extend both solenoids to shoot.
   */
  public void extendBoth() {
    m_piston1.set(DoubleSolenoid.Value.kForward);
    m_piston2.set(DoubleSolenoid.Value.kForward);
  }

  /**
   * Retract both solenoids to prepare to shoot.
   */
  public void retractBoth() {
    m_piston1.set(DoubleSolenoid.Value.kReverse);
    m_piston2.set(DoubleSolenoid.Value.kReverse);
  }

  /**
   * Extend solenoid 1 to shoot.
   */
  public void extend1() {
    m_piston1.set(DoubleSolenoid.Value.kForward);
  }

  /**
   * Retract solenoid 1 to prepare to shoot.
   */
  public void retract1() {
    m_piston1.set(DoubleSolenoid.Value.kReverse);
  }

  /**
   * Extend solenoid 2 to shoot.
   */
  public void extend2() {
    m_piston2.set(DoubleSolenoid.Value.kReverse);
  }

  /**
   * Retract solenoid 2 to prepare to shoot.
   */
  public void retract2() {
    m_piston2.set(DoubleSolenoid.Value.kForward);
  }

  /**
   * Turns off the piston1 double solenoid. This won't actuate anything
   * because double solenoids preserve their state when turned off. This
   * should be called in order to reduce the amount of time that the coils
   * are powered.
   */
  public void off1() {
    m_piston1.set(DoubleSolenoid.Value.kOff);
  }

  /**
   * Turns off the piston2 double solenoid. This won't actuate anything
   * because double solenoids preserve their state when turned off. This
   * should be called in order to reduce the amount of time that the coils
   * are powered.
   */
  public void off2() {
    m_piston2.set(DoubleSolenoid.Value.kOff);
  }

  /**
   * Release the latch so that we can shoot.
   */
  public void unlatch() {
    m_latchPiston.set(true);
  }

  /**
   * Latch so that pressure can build up and we aren't limited by air flow.
   */
  public void latch() {
    m_latchPiston.set(false);
  }

  /**
   * Toggles the latch postions.
   */
  public void toggleLatchPosition() {
    m_latchPiston.set(!m_latchPiston.get());
  }

  /**
   * Is Piston 1 extended (after shooting).
   *
   * @return Whether or not piston 1 is fully extended.
   */
  public boolean piston1IsExtended() {
    return !m_piston1ReedSwitchFront.get();
  }

  /**
   * Is Piston 1 retracted (before shooting).
   *
   * @return Whether or not piston 1 is fully retracted.
   */
  public boolean piston1IsRetracted() {
    return !m_piston1ReedSwitchBack.get();
  }

  /**
   * Turns off all double solenoids. Double solenoids hold their position when
   * they are turned off. We should turn them off whenever possible to extend
   * the life of the coils.
   */
  public void offBoth() {
    m_piston1.set(DoubleSolenoid.Value.kOff);
    m_piston2.set(DoubleSolenoid.Value.kOff);
  }

  /**
   * Return whether the goal is hot as read by the banner sensor.
   *
   * <p>NOTE: doesn't work in simulation.
   *
   * @return Whether or not the goal is hot
   */
  public boolean goalIsHot() {
    return m_hotGoalSensor.get();
  }
}
