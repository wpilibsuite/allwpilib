/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.testablerobot.test.commands;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;

import edu.wpi.first.wpilibj.MockHardwareExtension;
import edu.wpi.first.wpilibj.command.CommandTestHelper;
import edu.wpi.first.wpilibj.command.SchedulerTestHelper;

import edu.wpi.first.wpilibj.examples.testablerobot.commands.SayHelloCommand;
import edu.wpi.first.wpilibj.examples.testablerobot.subsystems.HelloWorldSubsystem;

import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.Mockito.atLeast;
import static org.mockito.Mockito.atMost;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;

/**
 * These are unit tests for the say hello command, which blinks an LED.
 */
@ExtendWith(MockHardwareExtension.class)
public class SayHelloCommandUnitTest {
  // We need to run the scheduler
  private SchedulerTestHelper m_scheduler;

  /**
   * Before a test starts, wire up the command scheduler to run
   * our scheduled command every 20 milliseconds.
   */
  @BeforeEach
  public void setupSchedulerHelper() {
    m_scheduler = new SchedulerTestHelper(20);
  }

  /**
   * When a test finishes, tear down the scheduler, because it uses threading
   * to do its work, and if you do not tear it down, the test runner will never
   * terminate.
   */
  @AfterEach
  public void cleanUpSchedulerHelper() {
    m_scheduler.destroy();
  }

  /**
   * A test that checks that the command will flash the LED through the
   * hello world subsystem the requisite number of times once run
   * under the scheduler. Exceptions can be thrown by the scheduler
   * and if they are, then the test will fail. That's fine.
   */
  @Test
  public void itCanFlashMyLEDEvery200ms() throws InterruptedException {
    // Assemble
    HelloWorldSubsystem mockHelloWorldSubsystem = mock(HelloWorldSubsystem.class);
    // This try syntax makes sure that close gets called on the command when it is done
    try
    (SayHelloCommand classUnderTest = new SayHelloCommand(mockHelloWorldSubsystem)) {
      // You must start the command for the scheduler to run it
      // (whenPressed does this for you automatically)
      classUnderTest.start();

      // Act
      // Run our command for a given number of milliseconds.
      m_scheduler.runForDuration(3000);  
    }
    

    // Assert
    // A range is tested here because we are counting nano seconds, which is not deterministic.
    // There may be a way to test a range in one statement, but it was not immediately obvious.
    verify(mockHelloWorldSubsystem, atLeast(14)).toggleLED();
    verify(mockHelloWorldSubsystem, atMost(15)).toggleLED();
  }

  /**
   * A test that validates that the state of the LED is off after the command is
   * interrupted (because the scheduler stopped in a manner similar to how commands
   * will be interrupted within WPILib).
   */
  @Test
  public void itTurnsOffMyLEDWhenStopping() throws InterruptedException {
    // Assemble
    HelloWorldSubsystem mockHelloWorldSubsystem = mock(HelloWorldSubsystem.class);
    // This try syntax makes sure that close gets called on the command when it is done
    try
    (SayHelloCommand classUnderTest = new SayHelloCommand(mockHelloWorldSubsystem)) {
      // You must start the command for the scheduler to run it
      // (whenPressed does this for you automatically)
      classUnderTest.start();

      // Act
      // Run our command for a given number of milliseconds.
      m_scheduler.runForDuration(1000);
    }

    // Assert
    // What we really want is a verification that the last call to the mock
    // is turning off the LED. This test only reports IF it was called...not when.
    // Hmmmm...
    verify(mockHelloWorldSubsystem, times(1)).turnOffLED();
  }

  /**
   * A test that verifies that the correct subsystems have been registered to the
   * command. This is mega important because this is how the WPILib scheduler knows
   * how to "preempt" a command already running with another command that is using
   * the same subsystem. Note that testing a commands' registered subsystems is a
   * protected method on the command. So a helper exposes a method for the
   * purpose of testing.
   */
  @Test
  public void itRegistersTheCorrectSubsystems() {
    // Assemble
    HelloWorldSubsystem mockHelloWorldSubsystem = mock(HelloWorldSubsystem.class);

    //Act
    SayHelloCommand classUnderTest = new SayHelloCommand(mockHelloWorldSubsystem);

    //Assert
    assertTrue(CommandTestHelper.doesRequire(classUnderTest, mockHelloWorldSubsystem));
  }
}
