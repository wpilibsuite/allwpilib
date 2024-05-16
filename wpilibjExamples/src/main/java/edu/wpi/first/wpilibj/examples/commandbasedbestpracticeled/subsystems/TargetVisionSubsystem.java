// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot.subsystems;

import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.Commands;
import edu.wpi.first.wpilibj2.command.SubsystemBase;
import edu.wpi.first.wpilibj2.command.button.CommandXboxController;
import edu.wpi.first.wpilibj2.command.button.Trigger;
import frc.robot.Color;
import frc.robot.LEDPattern;

public class TargetVisionSubsystem extends SubsystemBase {
  private final RobotSignals robotSignals;
  private final CommandXboxController operatorController;
  public final Trigger targetAcquired = new Trigger(this::canSeeTarget);

  public TargetVisionSubsystem(RobotSignals robotSignals, CommandXboxController operatorController) {
    this.robotSignals = robotSignals;
    this.operatorController = operatorController;
    targetAcquired.whileTrue(targetAcquired());
  }
  
  public Command targetAcquired() {
    LEDPattern targetAcquiredSignal = LEDPattern.solid(Color.kOrange);
    return
      /*sequential*/
      robotSignals.Top.setSignal(targetAcquiredSignal) // this command locks the robotSignals.Top subsystem only
      /*sequential*/
      .andThen(Commands.idle(this).withTimeout(0.)) // command created in this subsystem will lock this subsystem also
      /*composite*/
        .withName("LedVisionTargetInSight")
        .ignoringDisable(true); // ignore disable true must be here on the composite; on the first command doesn't do anything
  }

  private boolean canSeeTarget() {
    return operatorController.getHID().getAButton(); // fake event source for can see target
  }
}
