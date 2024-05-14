// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot;

import java.util.function.Supplier;

import static edu.wpi.first.wpilibj2.command.Commands.parallel;
import edu.wpi.first.math.filter.Debouncer.DebounceType;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.util.Color;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.CommandScheduler;
import edu.wpi.first.wpilibj2.command.button.CommandXboxController;
import frc.robot.subsystems.IntakeSubsystem;
import frc.robot.subsystems.RobotSignals;
import frc.robot.subsystems.TargetVisionSubsystem;

public class RobotContainer {

  boolean log = false;
  int operatorControllerPort = 0; 
  private final CommandXboxController operatorController = new CommandXboxController(operatorControllerPort);
  private final IntakeSubsystem intake;
  private final TargetVisionSubsystem vision;

  private final RobotSignals robotSignals;

  public RobotContainer(PeriodicTask periodicTask) {
    robotSignals = new RobotSignals(1, periodicTask);
    intake = new IntakeSubsystem(robotSignals, operatorController);
    vision = new TargetVisionSubsystem(robotSignals, operatorController);

    configureBindings();

    if(log) configureLogging();
  }

  private void configureBindings() {
    operatorController.x().debounce(1., DebounceType.kBoth)
      .onTrue(robotSignals.Top.setSignal(colorWheel()));
  }

  private Supplier<Color> colorWheel() {
    // produce a color based on the timer current seconds
    return ()->Color.fromHSV((int)(Timer.getFPGATimestamp()%60.)*3, 200, 200);
  }

  public Command getAutonomousCommand() {
    Color autoTopSignal = new Color(0.2, 0.6, 0.2);
    Color autoMainSignal = new Color(0.4, 0.9, 0.4);
    return
      parallel // interrupting either of the two parallel commands with an external command interupts the group
      (
      /*parallel cmd*/
        robotSignals.Top.setSignal(autoTopSignal)
          .withTimeout(6.) // this ends but the group continues and the default command is not activated here with or without the andThen command
        .andThen(robotSignals.Top.setSignal(autoTopSignal)),
      /*parallel cmd*/
        robotSignals.Main.setSignal(autoMainSignal)
      )
      /*composite*/
      .withName("AutoSignal");
    }

  private void configureLogging() {
    //_________________________________________________________________________________
    CommandScheduler.getInstance()
        .onCommandInitialize(
            command ->
            {
              System.out.println(/*command.getClass() + " " +*/ command.getName() + " initialized " + command.getRequirements());
            }
        );
    //_________________________________________________________________________________
    CommandScheduler.getInstance()
        .onCommandInterrupt(
            command ->
            {
              System.out.println(/*command.getClass() + " " +*/ command.getName() + " interrupted " + command.getRequirements());
            }
        );
    //_________________________________________________________________________________
    CommandScheduler.getInstance()
        .onCommandFinish(
            command ->
            {
              System.out.println(/*command.getClass() + " " +*/ command.getName() + " finished " + command.getRequirements());
            }
        );
    //_________________________________________________________________________________
    CommandScheduler.getInstance()
        .onCommandExecute( // this can generate a lot of events
            command ->
            {
              System.out.println(/*command.getClass() + " " +*/ command.getName() + " executed " + command.getRequirements());
            }
        );
    //_________________________________________________________________________________
  }
}



/* Parking Lot
  private final LEDPattern disabled = LEDPattern.solid(Color.kRed).breathe(Seconds.of(2));
  private final LEDPattern enabled = LEDPattern.solid(Color.kGreen).breathe(Seconds.of(2));
  private final LEDPattern defaultPattern = () -> (DriverStation.isDisabled() ? disabled : enabled).applyTo();
  private final LEDPattern blink = LEDPattern.solid(Color.kMagenta).blink(Seconds.of(0.2));
  private final LEDPattern orange = LEDPattern.solid(Color.kOrange);

  ledTop.setDefaultCommand(ledTop.runPattern(defaultPattern).ignoringDisable(true).withName("LedDefaultTop"));
  ledMain.setDefaultCommand(ledMain.runPattern(defaultPattern).ignoringDisable(true).withName("LedDefaultMain"));

  intake.gamePieceAcquired.onTrue(
    ledMain.runPattern(blink).withTimeout(1.0).withName("LedAcquiredGamePiece"));

  vision.targetAcquired.whileTrue(
    ledTop.runPattern(orange).withName("LedVisionTargetInSight"));
*/
