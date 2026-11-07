// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.rebuiltcmdv3.mechanisms;

import org.wpilib.command3.Command;
import org.wpilib.command3.Mechanism;
import org.wpilib.epilogue.Logged;
import org.wpilib.examples.rebuiltcmdv3.constants.IntakeConstants;

@Logged
public class Intake implements Mechanism {
  private final IntakeWrist wrist = new IntakeWrist();
  private final IntakeRoller roller = new IntakeRoller();

  /**
   * Intakes fuel off the ground. This command will run until canceled or interrupted by another
   * command.
   *
   * @return An intake command
   */
  public Command intake() {
    return run(coroutine -> {
          coroutine.awaitAll(wrist.down(), roller.intake());
        })
        .named("Intake.Intake");
  }

  /**
   * Stops the intake mechanism. This stops the roller and depowers the wrist, which may sag under
   * gravity.
   *
   * @return A command that stops the intake mechanism
   */
  public Command stop() {
    return run(coroutine -> {
          coroutine.awaitAll(wrist.stop(), roller.stop());
        })
        .withPriority(Command.LOWEST_PRIORITY)
        .named("Intake.Idle");
  }

  @Override
  public Command idle() {
    return stop();
  }

  /**
   * Stows the intake mechanism. This raises the wrist and stops the roller.
   *
   * @return A command that stows the intake mechanism
   */
  public Command stow() {
    return run(coroutine -> {
          coroutine.awaitAll(wrist.stow(), roller.stop());
        })
        .named("Intake.Stow");
  }

  /**
   * Uses the intake mechanism to agitate fuel inside the hopper. The roller will spin to encourage
   * fuel to move around and help unblock jams.
   *
   * @return A command that agitates the intake mechanism
   */
  public Command agitate() {
    return run(coroutine -> {
          coroutine.fork(roller.intake());

          while (true) {
            // await() will yield internally until the wrist has moved to the desired position,
            // so we don't need to explicitly yield here
            coroutine.await(wrist.moveToAngle(IntakeConstants.WRIST_AGITATE_UP));
            coroutine.await(wrist.moveToAngle(IntakeConstants.WRIST_AGITATE_DOWN));
          }
        })
        .named("Intake.Agitate");
  }

  /**
   * Uses the intake mechanism to expel fuel from the hopper.
   *
   * @return A command that expels fuel from the intake mechanism
   */
  public Command outtake() {
    return run(coroutine -> {
          coroutine.awaitAll(wrist.down(), roller.expel());
        })
        .named("Intake.Outtake");
  }
}
