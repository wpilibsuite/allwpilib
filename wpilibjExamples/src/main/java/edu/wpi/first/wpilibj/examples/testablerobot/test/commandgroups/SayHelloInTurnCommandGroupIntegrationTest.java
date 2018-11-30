/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.testablerobot.test.commandgroups;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;

import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.DigitalOutput;
import edu.wpi.first.wpilibj.MockHardwareExtension;
import edu.wpi.first.wpilibj.command.SchedulerTestHelper;

import edu.wpi.first.wpilibj.examples.testablerobot.commandgroups.SayHelloInTurnCommandGroup;
import edu.wpi.first.wpilibj.examples.testablerobot.commands.SayHelloCommand;
import edu.wpi.first.wpilibj.examples.testablerobot.commands.SayHelloUntilSilencedCommand;
import edu.wpi.first.wpilibj.examples.testablerobot.subsystems.HelloWorldSubsystem;
import edu.wpi.first.wpilibj.examples.testablerobot.subsystems.SilenceableHelloWorldSubsystem;

import static org.mockito.Mockito.atLeast;
import static org.mockito.Mockito.atMost;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

@ExtendWith(MockHardwareExtension.class)
public class SayHelloInTurnCommandGroupIntegrationTest {
  private SchedulerTestHelper m_scheduler;

  @BeforeEach
  public void schedulerSetup() {
    m_scheduler = new SchedulerTestHelper(20);
  }

  @AfterEach
  public void schedulerTeardown() {
    m_scheduler.destroy();
  }

  @Test
  public void itCanSayHelloASecondTimeOnlyAfterSilencingTheFirst() throws InterruptedException {
    //Assemble
    // The only thing we mock out here is the hardware...run everything else.
    // This is an integration test to make sure our command group operates
    // on the hardware as intended when external stimulus is encountered.
    DigitalOutput firstHelloLEDMock = mock(DigitalOutput.class);
    DigitalOutput secondHelloLEDMock = mock(DigitalOutput.class);
    DigitalInput silencerMock = mock(DigitalInput.class);

    // Mock up our sensor to not call for silence at first
    when(silencerMock.get()).thenReturn(false);

    // Our two subsystems
    SilenceableHelloWorldSubsystem silenceableHelloWorldSubsystem = 
        new SilenceableHelloWorldSubsystem(firstHelloLEDMock, silencerMock);
    HelloWorldSubsystem helloWorldSubsystem = new HelloWorldSubsystem(secondHelloLEDMock);

    // Our two commands
    SayHelloCommand sayHelloCommand = new SayHelloCommand(helloWorldSubsystem);
    SayHelloUntilSilencedCommand sayHelloUntilSilencedCommand = 
        new SayHelloUntilSilencedCommand(silenceableHelloWorldSubsystem);

    // And finally the command group
    // This try syntax makes sure that close gets called on the command when it is done
    try
    (SayHelloInTurnCommandGroup classUnderTest = 
        new SayHelloInTurnCommandGroup(sayHelloUntilSilencedCommand, sayHelloCommand)) {
      // You must start the command for the scheduler to run it
      // (whenPressed does this for you automatically)
      classUnderTest.start();

      //Act
      // Start your engines
      m_scheduler.runForDuration(1000);
    }

    // Pre-assertion...second led should not have come on, but first should have
    verify(firstHelloLEDMock, atLeast(1)).set(true);
    verify(secondHelloLEDMock, times(0)).set(true);

    // Trigger our sensor...Silence!
    // You could start to get fancy and for reusability, hide this
    // in a static function with a clever name.
    when(silencerMock.get()).thenReturn(true);

    // Start your engines, again...
    m_scheduler.runForDuration(1000);

    // Final Assert...second LED came on and run until interrupted
    verify(firstHelloLEDMock, atMost(5)).set(true);
    verify(secondHelloLEDMock, atLeast(4)).set(true);
    verify(secondHelloLEDMock, atMost(5)).set(true);
  }
}
