// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot;

/**
 * Create subsystems, triggers, and commands; bind buttons to commands and triggers; define command
 * logging;, manage the details of what is periodically processed before and after the command
 * scheduler loop; - everything until it got too big and some logical splits to other classes had to
 * be made.
 */

import static edu.wpi.first.units.Units.Seconds;
import static edu.wpi.first.wpilibj2.command.Commands.parallel;

import frc.robot.subsystems.AchieveHueGoal;
import frc.robot.subsystems.HistoryFSM;
import frc.robot.subsystems.Intake;
import frc.robot.subsystems.RobotSignals;
import frc.robot.subsystems.RobotSignals.LEDPatternSupplier;
import frc.robot.subsystems.TargetVision;

import edu.wpi.first.math.filter.Debouncer.DebounceType;
import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.CommandScheduler;
import edu.wpi.first.wpilibj2.command.button.CommandXboxController;
import edu.wpi.first.wpilibj2.command.button.Trigger;

public class RobotContainer {

  // runtime options; too rigid - could be made easier to find and change but this is just a
  // "simple" example program
  private final boolean logCommands = false; // switch command logging on/off; a lot of output for the
                                       // command execute methods

  // define all the subsystems
  private static final int m_OperatorControllerPort = 0;
  private final CommandXboxController m_OperatorController =
      new CommandXboxController(m_OperatorControllerPort);
  private final Intake m_Intake;
  private final TargetVision m_TargetVision;
  private final HistoryFSM m_HistoryFSM;
  private final AchieveHueGoal m_AchieveHueGoal;
  private final RobotSignals m_RobotSignals; // container and creator of all the LEDView subsystems
  private final GroupDisjointTest m_GroupDisjointTest =
      GroupDisjointTest.getInstance(); // container and creator of all the group/disjoint tests
                                       // subsystems

  public RobotContainer() {

    m_RobotSignals = new RobotSignals(1);
    m_Intake = new Intake(m_RobotSignals.m_Main, m_OperatorController);
    m_TargetVision = new TargetVision(m_RobotSignals.m_Top, m_OperatorController);
    m_HistoryFSM = new HistoryFSM(m_RobotSignals.m_HistoryDemo, m_OperatorController);
    m_AchieveHueGoal = new AchieveHueGoal(m_RobotSignals.m_AchieveHueGoal);

    configureBindings();

    configureDefaultCommands();

    if (logCommands)
      configureLogging();
  }

  /**
   * configure driver and operator controllers' buttons (if they haven't been defined)
   */
  private void configureBindings() {

    m_OperatorController.x().debounce(0.03, DebounceType.kBoth)
        .onTrue(m_RobotSignals.m_Top.setSignal(colorWheel()));

    new Trigger(m_OperatorController.rightTrigger(0.05)) // triggers if past a small threshold
        .onTrue(m_AchieveHueGoal.m_HueGoal.setHueGoal( // then it's always on
            () -> m_OperatorController.getRightTriggerAxis() * 180. // supplying the current value
        // scale joystick's 0 to 1 to computer color wheel hue 0 to 180
        ));
  }

  /**
   * "color wheel" supplier runs when commanded
   * 
   * @return
   */
  private RobotSignals.LEDPatternSupplier colorWheel() {

    // produce a color based on the timer current seconds of the minute
    return () -> LEDPattern.solid(Color.fromHSV(
        (int) (Timer.getFPGATimestamp() % 60./* seconds of the minute */)
            * 3/* scale seconds to 180 hues per color wheel */,
        200, 200));
  }

  /**
   * Configure some of the Default Commands
   * 
   * WARNING - heed the advice in the Robot.java comments about default commands
   * 
   */
  private void configureDefaultCommands() {

    final LEDPattern TopDefaultSignal = LEDPattern.solid(new Color(0., 0., 1.));
    final LEDPattern MainDefaultSignal =
        LEDPattern.solid(new Color(0., 1., 1.));
    final LEDPattern disabled =
        LEDPattern.solid(Color.kRed).breathe(Seconds.of(2));
    final LEDPattern enabled =
        LEDPattern.solid(Color.kGreen).breathe(Seconds.of(2));
    final LEDPatternSupplier EnableDisableDefaultSignal =
        () -> DriverStation.isDisabled() ? disabled : enabled;

    final Command TopDefault = m_RobotSignals.m_Top.setSignal(TopDefaultSignal)
        .ignoringDisable(true).withName("TopDefault");
    final Command MainDefault = m_RobotSignals.m_Main.setSignal(MainDefaultSignal)
        .ignoringDisable(true).withName("MainDefault");
    final Command EnableDisableDefault =
        m_RobotSignals.m_EnableDisable.setSignal(EnableDisableDefaultSignal)
            .ignoringDisable(true).withName("EnableDisableDefault");

    m_RobotSignals.m_Top.setDefaultCommand(TopDefault);
    m_RobotSignals.m_Main.setDefaultCommand(MainDefault);
    m_RobotSignals.m_EnableDisable.setDefaultCommand(EnableDisableDefault);
  }

  /**
   * Create a command to run in Autonomous
   * 
   * Example of setting signals by contrived example of composed commands
   * 
   * @return
   */
  public Command getAutonomousCommand() {

    LEDPattern autoTopSignal = LEDPattern.solid(new Color(0.1, 0.2, 0.2)).blend(
        LEDPattern.solid(new Color(0.7, 0.2, 0.2)).blink(Seconds.of(0.1)));
    LEDPattern autoMainSignal = LEDPattern.solid(new Color(0.3, 1., 0.3));
    // statements before the return are run early at initialization time
    return
    // statements returned are run later when the command is scheduled
    parallel // interrupting either of the two parallel commands with an external command interrupts
             // the group
    (m_RobotSignals.m_Top.setSignal(autoTopSignal).withTimeout(6.) // example this ends but the group
        // continues and the default command is not activated here with or without
        // the ".andThen" command
        .andThen(m_RobotSignals.m_Top.setSignal(autoTopSignal)),

        m_RobotSignals.m_Main.setSignal(autoMainSignal)).withName("AutoSignal");
  }

  /**
   * Configure Command logging
   */
  private void configureLogging() {

    CommandScheduler.getInstance().onCommandInitialize(command -> {
      if (!"LedSet".equals(command.getName())) {
        System.out.println(/* command.getClass() + " " + */ command.getName()
            + " initialized " + command.getRequirements());
      }
    });

    CommandScheduler.getInstance().onCommandInterrupt(command -> {
      if (!"LedSet".equals(command.getName())) {
        System.out.println(/* command.getClass() + " " + */ command.getName()
            + " interrupted " + command.getRequirements());
      }
    });

    CommandScheduler.getInstance().onCommandFinish(command -> {
      if (!"LedSet".equals(command.getName())) {
        System.out.println(/* command.getClass() + " " + */ command.getName()
            + " finished " + command.getRequirements());
      }
    });

    CommandScheduler.getInstance().onCommandExecute( // this can generate a lot of events
        command -> {
          if (!"LedSet".equals(command.getName())) {
            System.out
                .println(/* command.getClass() + " " + */ command.getName()
                    + " executed " + command.getRequirements());
          }
        });
  }

  /**
   * Run before commands and triggers
   * 
   * <p>Run periodically before commands are run - read sensors, etc. Include all classes that have
   * periodic inputs or other need to run periodically.
   *
   * <p>There are clever ways to register classes so they are automatically included in a list but this
   * example is simplistic - remember to type them in.
   */
  public void beforeCommands() {

    m_Intake.beforeCommands();
    m_TargetVision.beforeCommands();
    m_RobotSignals.beforeCommands();
    m_HistoryFSM.beforeCommands();
    m_AchieveHueGoal.beforeCommands();
    m_GroupDisjointTest.beforeCommands();
  }

  /**
   * Run after commands and triggers
   * 
   * <p>Run periodically after commands are run - write logs, dashboards, indicators Include all
   * classes that have periodic outputs
   * 
   * <p>There are clever ways to register classes so they are automatically included in a list but this
   * example isn't it; simplistic - remember to type them in.
   */
  public void afterCommands() {

    m_Intake.afterCommands();
    m_TargetVision.afterCommands();
    m_RobotSignals.afterCommands();
    m_HistoryFSM.afterCommands();
    m_AchieveHueGoal.afterCommands();
    m_GroupDisjointTest.afterCommands();
  }
}
