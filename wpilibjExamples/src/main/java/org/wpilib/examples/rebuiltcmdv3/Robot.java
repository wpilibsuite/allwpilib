// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.rebuiltcmdv3;

import static org.wpilib.examples.rebuiltcmdv3.constants.FieldConstants.NEUTRAL_ZONE;
import static org.wpilib.units.Units.Meters;

import org.wpilib.command3.Command;
import org.wpilib.command3.Scheduler;
import org.wpilib.command3.Trigger;
import org.wpilib.command3.button.CommandGamepad;
import org.wpilib.epilogue.Epilogue;
import org.wpilib.epilogue.Logged;
import org.wpilib.examples.rebuiltcmdv3.mechanisms.Intake;
import org.wpilib.examples.rebuiltcmdv3.mechanisms.Shooter;
import org.wpilib.examples.rebuiltcmdv3.mechanisms.SwerveDrive;
import org.wpilib.framework.OpModeRobot;
import org.wpilib.math.geometry.Pose2d;

@Logged
public class Robot extends OpModeRobot {
  public final SwerveDrive swerveDrive = new SwerveDrive();
  public final Shooter shooter = new Shooter();
  public final Intake intake = new Intake();
  public final PoseEstimator poseEstimator = new PoseEstimator();
  public final CommandGamepad controller = new CommandGamepad(1);

  public final Trigger inNeutralZone = new Trigger(() -> poseEstimator.inZone(NEUTRAL_ZONE));

  /** Initializes the robot class and sets safe default commands for all its mechanisms. */
  public Robot() {
    swerveDrive.setDefaultCommand(swerveDrive.idle());
    shooter.setDefaultCommand(shooter.idle());
    intake.setDefaultCommand(intake.idle());
  }

  @Override
  public void robotPeriodic() {
    // 1. Update odometry so commands will be working with up-to-date information.
    poseEstimator.odometryUpdate(swerveDrive.getGyroHeading(), swerveDrive.getModulePositions());

    // 2. Run the scheduler to poll triggers and execute our commands.
    Scheduler.getDefault().run();

    // 3. Update telemetry.
    Epilogue.update(this);
  }

  /**
   * A command that uses all the mechanisms on the robot to aim and shoot at a specific target.
   *
   * @param targetPose The target position to shoot at.
   * @return A command to shoot at a target
   */
  public Command shootAt(Pose2d targetPose) {
    return Command.noRequirements(
            coroutine -> {
              coroutine.await(swerveDrive.aimAt(poseEstimator::getEstimatedPose, targetPose));

              coroutine.awaitAll(
                  intake.agitate(),
                  shooter.shootAtHub(
                      () -> {
                        return Meters.of(
                            targetPose
                                .minus(poseEstimator.getEstimatedPose())
                                .getTranslation()
                                .getNorm());
                      }));
            })
        .named("Robot.ShootAt[" + targetPose + "]");
  }
}
