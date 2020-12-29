/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.romireference.commands;
import edu.wpi.first.wpilibj2.command.SequentialCommandGroup;
import edu.wpi.first.wpilibj.examples.romireference.subsystems.Drivetrain;

public class AutonomousTime extends SequentialCommandGroup {
  /**
   * Creates a new Autonomous Drive based on time. This will drive out for a 
   * period of time, turn around for time (equivalent to time to turn around) 
   * and drive forward again. This should mimic driving out, turning around
   * and driving back.
   * 
   * @param drivetrain   The drive subsystem on which this command will run
   */
  public AutonomousTime(Drivetrain drivetrain) {
   addCommands(
      new DriveTime(-0.6, 2.0, drivetrain),
      new TurnTime(-0.5, 1.3, drivetrain),
      new DriveTime(-0.6, 2.0, drivetrain),
      new TurnTime(0.5, 1.3, drivetrain)
    );
  }
}
