// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot;

import static edu.wpi.first.units.Units.Seconds;
import static edu.wpi.first.wpilibj2.command.Commands.parallel;

import edu.wpi.first.math.filter.Debouncer.DebounceType;
import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.CommandScheduler;
import edu.wpi.first.wpilibj2.command.button.CommandXboxController;
import frc.robot.subsystems.IntakeSubsystem;
import frc.robot.subsystems.RobotSignals;
import frc.robot.subsystems.RobotSignals.LEDPatternSupplier;
import frc.robot.subsystems.TargetVisionSubsystem;

public class RobotContainer {

  boolean log = false; // switch command logging on/off; a lot of output for the command execute methods
  int operatorControllerPort = 0; 
  private final CommandXboxController operatorController = new CommandXboxController(operatorControllerPort);
  private final IntakeSubsystem intake;
  private final TargetVisionSubsystem vision;

  private final RobotSignals robotSignals;
  private final LEDPattern TopDefaultSignal = LEDPattern.solid(new Color(0., 0., 1.));
  private final LEDPattern MainDefaultSignal = LEDPattern.solid(new Color(0., 1., 1.));
  private final LEDPattern disabled = LEDPattern.solid(Color.kRed).breathe(Seconds.of(2));
  private final LEDPattern enabled = LEDPattern.solid(Color.kGreen).breathe(Seconds.of(2));
  private final LEDPatternSupplier EnableDisableDefaultSignal = () -> (DriverStation.isDisabled() ? disabled : enabled);

  public RobotContainer(PeriodicTask periodicTask) {
    robotSignals = new RobotSignals(1, periodicTask);
    intake = new IntakeSubsystem(robotSignals, operatorController);
    vision = new TargetVisionSubsystem(robotSignals, operatorController);

    configureBindings();
    configureDefaultCommands();

    if(log) configureLogging();
  }

  /**
   * configure driver and operator controllers buttons
   */
  private void configureBindings() {
    operatorController.x().debounce(0.04, DebounceType.kBoth)
      .onTrue(robotSignals.Top.setSignal(colorWheel()));
  }

  /**
   * Test "rainbow" supplier
   * @return
   */
  private RobotSignals.LEDPatternSupplier colorWheel() {
    // produce a color based on the timer current seconds of the minute
    return ()->LEDPattern.solid(
      Color.fromHSV((int)(Timer.getFPGATimestamp()%60./*seconds of the minute*/)*3/*scale seconds to 180 hues per color wheel*/,
       200, 200));
  }

  /**
   * Configure the LED Signal Views Default Commands 
   */
  private void configureDefaultCommands() {

    final Command TopDefault = robotSignals.Top.setSignal(TopDefaultSignal)
                                .ignoringDisable(true).withName("TopDefault");
    final Command MainDefault = robotSignals.Main.setSignal(MainDefaultSignal)
                                .ignoringDisable(true).withName("MainDefault");
    final Command EnableDisableDefault = robotSignals.EnableDisable.setSignal(EnableDisableDefaultSignal)
                                .ignoringDisable(true).withName("EnableDisableDefault");

    robotSignals.Top.setDefaultCommand(TopDefault);
    robotSignals.Main.setDefaultCommand(MainDefault);
    robotSignals.EnableDisable.setDefaultCommand(EnableDisableDefault);
  }

  /**
   * Create a command to run in Autonomous
   * @return
   */
  public Command getAutonomousCommand() {
    LEDPattern autoTopSignal = LEDPattern.solid(new Color(0.2, 0.6, 0.2));
    LEDPattern autoMainSignal = LEDPattern.solid(new Color(0.4, 0.9, 0.4));
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

  /**
   * Configure Command logging
   */
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