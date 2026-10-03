// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.rebuiltcmdv3.opmodes.teleop;

import org.wpilib.examples.rebuiltcmdv3.Robot;
import org.wpilib.examples.rebuiltcmdv3.constants.FieldConstants;
import org.wpilib.opmode.OpMode;
import org.wpilib.opmode.Teleop;

@Teleop
public class DefaultTeleop implements OpMode {
  /**
   * Creates a new default teleop mode. This constructor is called automatically by the OpModeRobot
   * framework when the program starts up.
   *
   * @param robot The robot instance to control.
   */
  public DefaultTeleop(Robot robot) {
    robot.swerveDrive.setDefaultCommand(robot.swerveDrive.driverControl(robot.controller));

    robot.controller.faceUp().onTrue(robot.intake.intake());
    robot.controller.faceDown().onTrue(robot.intake.stow());
    robot.controller.faceRight().whileTrue(robot.intake.agitate());

    // The right stick is used for turning, but switch to aim assist when the driver presses down
    // on the stick. Un-pressing the stick will return to normal driver control.
    // If, for some reason, no target hub is found, the robot's current pose will be used as the
    // target location.
    robot
        .controller
        .rightStick()
        .whileTrue(
            robot.swerveDrive.aimAssist(
                robot.controller,
                robot.poseEstimator::getEstimatedPose,
                () -> FieldConstants.targetHub().orElseGet(robot.poseEstimator::getEstimatedPose)));
  }
}
