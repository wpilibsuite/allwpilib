/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.romireference.commands;
import edu.wpi.first.wpilibj2.command.SequentialCommandGroup;
import edu.wpi.first.wpilibj.examples.romireference.subsystems.Drivetrain;

public class AutonomousDistance extends SequentialCommandGroup {
  /**
   * Creates a new Autonomous Drive based on distance. This will drive out for a 
   * specified distance, turn around and drive back.
   * 
   * @param drivetrain   The drivetrain subsystem on which this command will run
   */
  public AutonomousDistance(Drivetrain drivetrain) {
    addCommands(
      new DriveDistance(-0.5, 10, drivetrain),
      new TurnDegrees(-0.5, 180, drivetrain),
      new DriveDistance(-0.5, 10, drivetrain),
      new TurnDegrees(0.5, 180, drivetrain)
    ); 
  }
}
