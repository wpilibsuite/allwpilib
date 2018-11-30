/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.testablerobot.test.commands;

import java.util.concurrent.ExecutionException;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.DigitalOutput;
import edu.wpi.first.wpilibj.command.SchedulerTestHelper;

import edu.wpi.first.wpilibj.examples.testablerobot.commands.SayHelloCommand;
import edu.wpi.first.wpilibj.examples.testablerobot.subsystems.HelloWorldSubsystem;

import static org.mockito.Mockito.atLeast;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.verify;

/**
 * Integration tests that make sure that when our command acts on a subsystem,
 * that the hardware within the subsystem does what we expect. It is called
 * integration because we are testing the integration of classes together
 * to see if they act on each other as expected.
 */
public class SayHelloCommandIntegrationTest {
  // A command is typically executed by the WPILib scheduler. But in a unit test,
  // we really want to skinny down the number of dependencies we involve in doing 
  // the testing. And further, we want to control those dependencies so that our
  // tests don't fail because of a dependency change. So, I have implemented a fake
  // and very skinny command scheduler. Critics will say that the implementation
  // of that scheduler is woefully incomplete. Correct. It is that way on purpose.
  // We just need the ability to exercise our command in the most basic way possible.
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
   * A test that checks that the command will turn on the LED within the
   * hello world subsystem the requisite number of times once run
   * under the scheduler. Exceptions can be thrown by the scheduler
   * and if they are, then the test will fail. That's fine. One test
   * success criteria is defined by the Test decorator parameter that
   * says we should not exit the method with anything on the stack
   * (an exception, for example).
   */
  @Test /* no exception expected */
  public void itCanFlashMyLEDEvery200ms() throws InterruptedException, ExecutionException {
    // Assemble
    // Because we can mock the hardware here, we don't need a roboRio or the HAL, and because
    // the subsystem can be passed its external dependencies, it does not
    // care that the hardware is not real. For the test, we will mock whatever the
    // hardware is supposed to do to pass the test.
    DigitalOutput ledMock = mock(DigitalOutput.class);

    // Note that we do not mock the subsystem because we are going to
    // test the result on the subsystem by exercising behavior on the command.
    HelloWorldSubsystem classUnderTest = new HelloWorldSubsystem(ledMock);
    SayHelloCommand sayHelloCommand = new SayHelloCommand(classUnderTest);
    sayHelloCommand.start();

    // Act
    m_scheduler.runForDuration(3);

    // Assert
    verify(ledMock, atLeast(7)).set(true);
  }
}
