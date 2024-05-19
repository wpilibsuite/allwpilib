// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot;

import static edu.wpi.first.units.Units.Seconds;
import static edu.wpi.first.wpilibj2.command.Commands.parallel;

import java.util.function.DoubleSupplier;

import edu.wpi.first.math.filter.Debouncer.DebounceType;
import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.CommandScheduler;
import edu.wpi.first.wpilibj2.command.button.CommandXboxController;
import edu.wpi.first.wpilibj2.command.button.Trigger;
import frc.robot.subsystems.AchieveHueGoal;
import frc.robot.subsystems.HistoryFSM;
import frc.robot.subsystems.IntakeSubsystem;
import frc.robot.subsystems.RobotSignals;
import frc.robot.subsystems.RobotSignals.LEDPatternSupplier;
import frc.robot.subsystems.TargetVisionSubsystem;

public class RobotContainer {

  private boolean logCommands = false; // switch command logging on/off; a lot of output for the command execute methods
  private int operatorControllerPort = 0; 
  private final CommandXboxController operatorController = new CommandXboxController(operatorControllerPort);
  private DoubleSupplier hueGoal = ()->operatorController.getRightTriggerAxis() * 180.; // scale joystick 0 to 1 to computer color wheel hue 0 to 180
  private final IntakeSubsystem intake;
  private final TargetVisionSubsystem vision;
  private final HistoryFSM historyFSM;
  private final AchieveHueGoal achieveHueGoal;

  private final RobotSignals robotSignals;

  public RobotContainer() {

    robotSignals = new RobotSignals(1);
    intake = new IntakeSubsystem(robotSignals.Main, operatorController);
    vision = new TargetVisionSubsystem(robotSignals.Top, operatorController);
    historyFSM = new HistoryFSM(robotSignals.HistoryDemo, operatorController);
    achieveHueGoal = new AchieveHueGoal(robotSignals.AchieveHueGoal/*, hueGoal*/);

    configureBindings();

    configureDefaultCommands();

    if(logCommands) configureLogging();
  }

  /**
   * configure driver and operator controllers buttons
   */
    private void configureBindings() {

    operatorController.x().debounce(0.03, DebounceType.kBoth)
      .onTrue(robotSignals.Top.setSignal(colorWheel()));

    
    final Trigger startAcceptingSetpoints = new Trigger(operatorController.rightTrigger(0.05))
      .onTrue(achieveHueGoal.hueGoal.setHueGoal(hueGoal));
  }

  /**
   * "color wheel" supplier runs when commanded
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
   * 
   * DANGER DANGER DANGER
   * Default commands are not run in composed commands.
   * Suggest not using default commands to prevent assuming they run
   * (Example included on how to disable the setDefaultCommand)
   * If using the default command sugegst not setting it more than once
   * to prevent confusion on which one is set.
   * (Example included on how to prevent more than one setting of the default command)
   */
  private void configureDefaultCommands() {

    final LEDPattern TopDefaultSignal = LEDPattern.solid(new Color(0., 0., 1.));
    final LEDPattern MainDefaultSignal = LEDPattern.solid(new Color(0., 1., 1.));
    final LEDPattern disabled = LEDPattern.solid(Color.kRed).breathe(Seconds.of(2));
    final LEDPattern enabled = LEDPattern.solid(Color.kGreen).breathe(Seconds.of(2));
    final LEDPatternSupplier EnableDisableDefaultSignal = () -> (DriverStation.isDisabled() ? disabled : enabled);

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
    // statements before the return are run early at intialization time
    return
    // statements returned are run later when the command is scheduled
      parallel // interrupting either of the two parallel commands with an external command interupts the group
      (
      /*parallel cmd*/
        robotSignals.Top.setSignal(autoTopSignal)
          .withTimeout(6.) // example this ends but the group continues and the default command is not activated here with or without the andThen command
          .andThen(robotSignals.Top.setSignal(autoTopSignal)),
      /*parallel cmd*/
        robotSignals.Main.setSignal(autoMainSignal)
      )
      /*composite*/
        .withName("AutoSignal");
      // command ends here so default command runs if no subsequant command runs for the subsystem
    }

    /*
     * vision target
     * intake
     *  release vision target
     *  intake
     * 
     */




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

  /**
   * Run periodically before commands are run - read sensors
   * Include all classes that have periodic inputs
   */
  public void beforeCommands() {

    intake.beforeCommands();
    vision.beforeCommands();
    robotSignals.beforeCommands();
    historyFSM.beforeCommands();
    achieveHueGoal.beforeCommands();
  }

  /**
   * Run periodically after commands are run - write logs, dashboards, indicators
   * Include all classes that have periodic outputs
   */
  public void afterCommands() {

    intake.afterCommands();
    vision.afterCommands();
    robotSignals.afterCommands();
    historyFSM.afterCommands();
    achieveHueGoal.afterCommands();
  }
}
