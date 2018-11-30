/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.testablerobot.subsystems;

import edu.wpi.first.wpilibj.DigitalOutput;
import edu.wpi.first.wpilibj.command.Subsystem;

/**
 * An example subsystem which encapsuates an LED.
 */
public class HelloWorldSubsystem extends Subsystem {

  /**
   * LED example hardware that the subsystem encapsulates. We keep it private
   * because the user of the class should have no business fooling with
   * internal state directly. Use instance methods to perform actions on hardware.
   * We make it final because the instance needs to count on the fact that the
   * hardware will not change out from under it throughout its life (fancy term is
   * immutable).
   */
  private final DigitalOutput m_led;

  /**
   * We create a constructor to inject "dependencies" into our class. Do not instantiate
   * external dependencies directly. Instead, invite them in and simply use them. In this
   * way, we can always instantiate this class, even if hardware does not actually exist.
   * The requirement of instantiation is an "invariant". In order to even have the hope
   * of automated testing, we must always be able to instantiate our class regardless of
   * the conditions of the outside world.
   * 
   * @param led   Digital output hardware our led is connected to
   */
  public HelloWorldSubsystem(DigitalOutput led) {

    // Another invariant is that our class always assumes that a
    // valid reference to our LED will exist. So complain 
    // if our invited guest is nothingness.
    if (led == null) {
      throw new IllegalArgumentException("I must have what at least looks like an LED!");
    }

    // Assign our invited guest to an instance variable, so we can use it later.
    this.m_led = led;
  }

  /**
   * Instance method to interact with our hardware...turning on LED.
   */
  public void turnOnLED() {
    m_led.set(true);
  }

  /**
   * Instance method to interact with our hardware...turning off LED.
   */
  public void turnOffLED() {
    m_led.set(false);
  }

  /**
   * Instance method to flip the state of our LED.
   */
  public void toggleLED() {
    m_led.set(!m_led.get());
  }

  @Override
  public void initDefaultCommand() {
    // To init a default command, pass an instantiated command into the constructor.
    // setDefaultCommand(new mySpecialCommand());
  }
}
