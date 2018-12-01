/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.testablerobot.test.commands;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.examples.testablerobot.commands.SayHelloCommand;
import edu.wpi.first.wpilibj.examples.testablerobot.subsystems.HelloWorldSubsystem;
import edu.wpi.first.wpilibj.examples.testablerobot.test.helpers.TestWithScheduler;
import edu.wpi.first.wpilibj.examples.testablerobot.test.mocks.DigitalOutputMock;

import static edu.wpi.first.wpilibj.examples.testablerobot.test.helpers.SchedulerPumpHelper.runForDuration;
import static org.mockito.Mockito.atLeast;
import static org.mockito.Mockito.verify;

/**
 * Integration tests that make sure that when our command acts on a subsystem,
 * that the hardware within the subsystem does what we expect. It is called
 * integration because we are testing the integration of classes together
 * to see if they act on each other as expected.
 */
public class SayHelloCommandIntegrationTest extends TestWithScheduler {
  /**
   * A test that checks that the command will turn on the LED within the
   * hello world subsystem the requisite number of times once run
   * under the scheduler. Exceptions can be thrown by the scheduler
   * and if they are, then the test will fail. That's fine.
   */
  @Test
  public void itCanFlashMyLEDEvery200ms() throws InterruptedException {
    // Assemble
    // Because we can mock the hardware here, we don't need a roboRio or the HAL, and because
    // the subsystem can be passed its external dependencies, it does not
    // care that the hardware is not real. For the test, we will mock whatever the
    // hardware is supposed to do to pass the test.
    DigitalOutputMock ledMock = new DigitalOutputMock(false);

    // Note that we do not mock the subsystem because we are going to
    // test the result on the subsystem by exercising behavior on the command.
    HelloWorldSubsystem classUnderTest = new HelloWorldSubsystem(ledMock.getDigitalOutput());
    try
    (SayHelloCommand sayHelloCommand = new SayHelloCommand(classUnderTest)) {
      sayHelloCommand.start();

      // Act
      runForDuration(3000);
    }
    
    // Assert
    verify(ledMock.getDigitalOutput(), atLeast(7)).set(true);
  }
}
