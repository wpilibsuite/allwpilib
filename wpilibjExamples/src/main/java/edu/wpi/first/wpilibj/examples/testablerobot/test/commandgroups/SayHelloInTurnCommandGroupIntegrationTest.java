/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.testablerobot.test.commandgroups;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.DigitalOutput;

import edu.wpi.first.wpilibj.examples.testablerobot.commandgroups.SayHelloInTurnCommandGroup;
import edu.wpi.first.wpilibj.examples.testablerobot.commands.SayHelloCommand;
import edu.wpi.first.wpilibj.examples.testablerobot.commands.SayHelloUntilSilencedCommand;
import edu.wpi.first.wpilibj.examples.testablerobot.subsystems.HelloWorldSubsystem;
import edu.wpi.first.wpilibj.examples.testablerobot.subsystems.SilenceableHelloWorldSubsystem;
import edu.wpi.first.wpilibj.examples.testablerobot.test.helpers.TestWithScheduler;
import edu.wpi.first.wpilibj.examples.testablerobot.test.mocks.DigitalOutputMock;

import static edu.wpi.first.wpilibj.examples.testablerobot.test.helpers.SchedulerPumpHelper.runForDuration;
import static org.mockito.Mockito.atMost;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

public class SayHelloInTurnCommandGroupIntegrationTest extends TestWithScheduler {
  @Test
  public void itCanSayHelloASecondTimeOnlyAfterSilencingTheFirst() throws InterruptedException {
    //Assemble
    // The only thing we mock out here is the hardware...run everything else.
    // This is an integration test to make sure our command group operates
    // on the hardware as intended when external stimulus is encountered.
    DigitalOutputMock firstHelloLEDMock = new DigitalOutputMock(false);
    DigitalOutputMock secondHelloLEDMock = new DigitalOutputMock(false);
    DigitalOutput firstHelloDigitalOutputMock = firstHelloLEDMock.getDigitalOutput();
    DigitalOutput secondHelloDigitalOutputMock = secondHelloLEDMock.getDigitalOutput();
    DigitalInput silencerMock = mock(DigitalInput.class);

    // Mock up our sensor to not call for silence at first
    when(silencerMock.get()).thenReturn(false);

    // Our two subsystems
    SilenceableHelloWorldSubsystem silenceableHelloWorldSubsystem = 
        new SilenceableHelloWorldSubsystem(firstHelloDigitalOutputMock, silencerMock);
    HelloWorldSubsystem helloWorldSubsystem = new HelloWorldSubsystem(secondHelloDigitalOutputMock);

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
      runForDuration(1000);

      // Pre-assertion...second led should not have come on, but first should have
      verify(firstHelloDigitalOutputMock, atMost(3)).set(true);
      verify(secondHelloDigitalOutputMock, times(0)).set(true);

      // Trigger our sensor...Silence!
      // You could start to get fancy and for reusability, hide this
      // in a static function with a clever name.
      when(silencerMock.get()).thenReturn(true);

      // Start your engines, again...
      runForDuration(1000);

      // Final Assert...second LED came on and ran for a second
      // First LED was cut off and should not have flashed any more
      verify(firstHelloDigitalOutputMock, atMost(3)).set(true);
      verify(secondHelloDigitalOutputMock, atMost(3)).set(true);
    }
  }
}
