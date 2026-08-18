// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.rebuiltcmdv3.opmodes.auto;

import org.wpilib.command3.Command;
import org.wpilib.command3.Trigger;
import org.wpilib.driverstation.RobotState;
import org.wpilib.examples.rebuiltcmdv3.Robot;
import org.wpilib.examples.rebuiltcmdv3.constants.FieldConstants;
import org.wpilib.opmode.Autonomous;
import org.wpilib.opmode.OpMode;

@Autonomous
public class SweepAuto implements OpMode {
  private final Trigger enabled = new Trigger(RobotState::isEnabled);

  /**
   * Creates a new autonomous mode that sweeps the left trench and scores on the hub. This
   * constructor is called automatically by the OpModeRobot framework when the program starts up.
   *
   * @param robot The robot instance to control.
   */
  public SweepAuto(Robot robot) {
    // Start the intake stowed
    robot.intake.setDefaultCommand(robot.intake.stow());

    // Extend the intake once we cross into the neutral zone and leave it extended for the entire
    // opmode
    robot.inNeutralZone.onTrue(robot.intake.intake());

    // Once the robot is enabled, start following a sweep path through the left trench,
    // into the neutral zone, then back over the bump.
    // When we return to the alliance zone, aim at the hub and start shooting.
    enabled.onTrue(sweepAndScore(robot));
  }

  /**
   * Creates a command that drives the robot in a path that sweeps through the neutral zone to
   * gather fuel, returns to the alliance zone, and then shoots at the hub until the end of the
   * autonomous period.
   *
   * @param robot The robot instance to control
   * @return A sweep-and-score command
   */
  private Command sweepAndScore(Robot robot) {
    return Command.noRequirements(
            coroutine -> {
              coroutine.await(robot.swerveDrive.followPath("nz-sweep-left-trench"));

              FieldConstants.targetHub()
                  .ifPresent(
                      hub -> {
                        coroutine.await(robot.shootAt(hub));
                      });
            })
        .named("Sweep and Score");
  }
}
