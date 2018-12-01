/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.testablerobot.test.subsystems;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.DigitalOutput;

import edu.wpi.first.wpilibj.examples.testablerobot.RobotMap;
import edu.wpi.first.wpilibj.examples.testablerobot.subsystems.HelloWorldSubsystem;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

/**
 * Unit tests for the hello world subsystem. It consists of a simple
 * LED to demonstrate how you might go about unit testing your
 * subsystems.
 */
public class HelloWorldSubsystemUnitTest {
  /**
   * A test to make sure no one has hard-wired dependencies such
   * that our class is no longer testable.
   */
  @Test
  public void itShouldInstantiateGivenAnLED() {
    // Assemble
    DigitalOutput mockLED = mock(DigitalOutput.class);

    // Act and Assert
    assertDoesNotThrow( () -> {
      new HelloWorldSubsystem(mockLED);
    });
  }

  /**
   * A test to make sure that if a sneaky user tries to give us
   * a null LED, we do not let them.
   */
  @Test
  public void itShouldNotInstantiateWithANullLED() {
    // Assemble
    // Nothing to do

    // Act
    // Nothing to do

    //Assert
    assertThrows(IllegalArgumentException.class, () -> {
      new HelloWorldSubsystem(null);
    });
  }

  /**
   * A test to catch a developer adding presumably another constructor. Enforce 
   * invariant to make sure class can always be instantiated.
   */
  @Test
  public void itShouldHaveOnlyOneConstructor() {
    // Assemble
    // Nothing to do

    // Act
    // Nothing to do

    // Assert
    assertEquals(1, (HelloWorldSubsystem.class).getDeclaredConstructors().length);
  }

  /**
   * A test to make sure we have set the state of the led digital output to true.
   */
  @Test
  public void itShouldTurnOnTheLED() {
    // Assemble
    DigitalOutput mockLED = mock(DigitalOutput.class);
    try
    (HelloWorldSubsystem classUnderTest = new HelloWorldSubsystem(mockLED)) {

      // Act
      classUnderTest.turnOnLED();
    }
    
    // Assert
    // Make sure that we have set the digital output to true.
    // This may seem useless, but for the purpose of this test, we don't
    // care whether the DigitalOutput actually works (we assume it does).
    // We just want to make sure we have done our job by setting the state
    // properly.
    verify(mockLED, times(1)).set(true);
  }

  /**
   * A test to make sure we have set the state of the led digital output to false.
   */
  @Test
  public void itShouldTurnOffTheLED() {
    // Assemble
    DigitalOutput mockLED = mock(DigitalOutput.class);
    try
    (HelloWorldSubsystem classUnderTest = new HelloWorldSubsystem(mockLED)) {

      // Act
      classUnderTest.turnOffLED();
    }

    // Assert
    // Make sure that we have set the digital output to false.
    // This may seem useless, but for the purpose of this test, we don't
    // care whether the DigitalOutput actually works (we assume it does).
    // We just want to make sure we have done our job by setting the state
    // properly.
    verify(mockLED, times(1)).set(false);
  }

  /**
   * A test to make sure we can toggle the state of the LED.
   */
  @Test
  public void itShouldToggleTheLED() {
    // Assemble
    DigitalOutput mockLED = mock(DigitalOutput.class);
    try
    (HelloWorldSubsystem classUnderTest = new HelloWorldSubsystem(mockLED)) {
      // Wire up some state so we know what our digital IO port will return.
      // Note that you can stack the return state when your matcher is called
      // more than once.
      when(mockLED.get()).thenReturn(false).thenReturn(true);

      // Act
      classUnderTest.toggleLED();
      
      // Assert
      // Make sure that we have set the digital output to the opposite
      // of the state that it is currently at, as defined by the when matcher above.
      verify(mockLED, times(1)).set(true);

      // Act, again...normally we want to limit assertions in any given test,
      // but it's useful to demonstrate the use of Mockito matchers. The more
      // assertions you have in any given test, the more it is telling you to
      // break up the test into smaller parts.
      classUnderTest.toggleLED();
      
      // Assert
      // Make sure that we have set the digital output to the opposite
      // of the state that it is currently at, as defined by the when matcher above.
      verify(mockLED, times(1)).set(false);
    }
  }

  /**
   * A test to make sure we know what port the LED is plugged into.
   * You could use this as a workflow for notifying your electrical
   * subteam that a change has been made...you could even send an email
   * or text. That would be fancy.
   * 
   * <p>NOTE: This does not test whether DigitalOutput in the ROBOT class
   * uses the correct constant. That would be in the unit test for that
   * class, assuming it is testable...
   */
  @Test
  public void itShouldReferencePort0ForLED() {
    // Assemble
    // Nothing to do

    // Act
    // Nothing to do

    // Assert
    assertEquals(0, RobotMap.silenceableLED, 
        "Tell electrical that the LED port has changed, and then update this test.");
  }
}
