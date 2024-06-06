// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled;

/**
 * Create subsystems, triggers, and commands; bind buttons to commands and triggers; define command
 * logging;, manage the details of what is periodically processed before and after the command
 * scheduler loop; - everything until it got too big and some logical splits to other classes had to
 * be made.
 */
import static edu.wpi.first.units.Units.Seconds;
import static edu.wpi.first.wpilibj2.command.Commands.parallel;

import edu.wpi.first.math.filter.Debouncer.DebounceType;
import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems.AchieveHueGoal;
import edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems.HistoryFSM;
import edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems.Intake;
import edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems.RobotSignals;
import edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems.RobotSignals.LEDPatternSupplier;
import edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems.TargetVision;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.CommandScheduler;
import edu.wpi.first.wpilibj2.command.button.CommandXboxController;
import edu.wpi.first.wpilibj2.command.button.Trigger;

public class RobotContainer {
  // runtime options; too rigid - could be made easier to find and change but this is just a
  // "simple" example program
  private final boolean logCommands =
      false; // switch command logging on/off; a lot of output for the
  // command execute methods

  // define all the subsystems
  private static final int m_operatorControllerPort = 0;
  private final CommandXboxController m_operatorController =
      new CommandXboxController(m_operatorControllerPort);
  private final Intake m_intake;
  private final TargetVision m_targetVision;
  private final HistoryFSM m_historyFSM;
  private final AchieveHueGoal m_achieveHueGoal;
  private final RobotSignals m_robotSignals; // container and creator of all the LEDView subsystems
  private final GroupDisjointTest m_groupDisjointTest =
      GroupDisjointTest.getInstance(); // container and creator of all the group/disjoint tests

  // subsystems

  public RobotContainer() {
    m_robotSignals = new RobotSignals(1);
    m_intake = new Intake(m_robotSignals.m_main, m_operatorController);
    m_targetVision = new TargetVision(m_robotSignals.m_top, m_operatorController);
    m_historyFSM = new HistoryFSM(m_robotSignals.m_historyDemo, m_operatorController);
    m_achieveHueGoal = new AchieveHueGoal(m_robotSignals.m_achieveHueGoal);

    configureBindings();

    configureDefaultCommands();

    if (logCommands) configureLogging();
  }

  /** configure driver and operator controllers' buttons (if they haven't been defined) */
  private void configureBindings() {
    m_operatorController
        .x()
        .debounce(0.03, DebounceType.kBoth)
        .onTrue(m_robotSignals.m_top.setSignal(colorWheel()));

    new Trigger(m_operatorController.rightTrigger(0.05)) // triggers if past a small threshold
        .onTrue(
            m_achieveHueGoal.m_hueGoal.setHueGoal( // then it's always on
                () ->
                    m_operatorController.getRightTriggerAxis() * 180. // supplying the current value
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
    return () ->
        LEDPattern.solid(
            Color.fromHSV(
                (int) (Timer.getFPGATimestamp() % 60. /* seconds of the minute */)
                    * 3 /* scale seconds to 180 hues per color wheel */,
                200,
                200));
  }

  /**
   * Configure some of the Default Commands
   *
   * <p>WARNING - heed the advice in the Robot.java comments about default commands
   */
  private void configureDefaultCommands() {
    final LEDPattern TopDefaultSignal = LEDPattern.solid(new Color(0., 0., 1.));
    final LEDPattern MainDefaultSignal = LEDPattern.solid(new Color(0., 1., 1.));
    final LEDPattern disabled = LEDPattern.solid(Color.kRed).breathe(Seconds.of(2));
    final LEDPattern enabled = LEDPattern.solid(Color.kGreen).breathe(Seconds.of(2));
    final LEDPatternSupplier EnableDisableDefaultSignal =
        () -> DriverStation.isDisabled() ? disabled : enabled;

    final Command TopDefault =
        m_robotSignals
            .m_top
            .setSignal(TopDefaultSignal)
            .ignoringDisable(true)
            .withName("TopDefault");
    final Command MainDefault =
        m_robotSignals
            .m_main
            .setSignal(MainDefaultSignal)
            .ignoringDisable(true)
            .withName("MainDefault");
    final Command EnableDisableDefault =
        m_robotSignals
            .m_enableDisable
            .setSignal(EnableDisableDefaultSignal)
            .ignoringDisable(true)
            .withName("EnableDisableDefault");

    m_robotSignals.m_top.setDefaultCommand(TopDefault);
    m_robotSignals.m_main.setDefaultCommand(MainDefault);
    m_robotSignals.m_enableDisable.setDefaultCommand(EnableDisableDefault);
  }

  /**
   * Create a command to run in Autonomous
   *
   * <p>Example of setting signals by contrived example of composed commands
   *
   * @return
   */
  public Command getAutonomousCommand() {
    LEDPattern autoTopSignal =
        LEDPattern.solid(new Color(0.1, 0.2, 0.2))
            .blend(LEDPattern.solid(new Color(0.7, 0.2, 0.2)).blink(Seconds.of(0.1)));
    LEDPattern autoMainSignal = LEDPattern.solid(new Color(0.3, 1., 0.3));
    // statements before the return are run early at initialization time
    return
    // statements returned are run later when the command is scheduled
    parallel // interrupting either of the two parallel commands with an external command interrupts
        // the group
        (
            m_robotSignals
                .m_top
                .setSignal(autoTopSignal)
                .withTimeout(6.) // example this ends but the group
                // continues and the default command is not activated here with or without
                // the ".andThen" command
                .andThen(m_robotSignals.m_top.setSignal(autoTopSignal)),
            m_robotSignals.m_main.setSignal(autoMainSignal))
        .withName("AutoSignal");
  }

  /** Configure Command logging */
  private void configureLogging() {
    CommandScheduler.getInstance()
        .onCommandInitialize(
            command -> {
              if (!"LedSet".equals(command.getName())) {
                System.out.println(
                    /* command.getClass() + " " + */ command.getName()
                        + " initialized "
                        + command.getRequirements());
              }
            });

    CommandScheduler.getInstance()
        .onCommandInterrupt(
            command -> {
              if (!"LedSet".equals(command.getName())) {
                System.out.println(
                    /* command.getClass() + " " + */ command.getName()
                        + " interrupted "
                        + command.getRequirements());
              }
            });

    CommandScheduler.getInstance()
        .onCommandFinish(
            command -> {
              if (!"LedSet".equals(command.getName())) {
                System.out.println(
                    /* command.getClass() + " " + */ command.getName()
                        + " finished "
                        + command.getRequirements());
              }
            });

    CommandScheduler.getInstance()
        .onCommandExecute( // this can generate a lot of events
            command -> {
              if (!"LedSet".equals(command.getName())) {
                System.out.println(
                    /* command.getClass() + " " + */ command.getName()
                        + " executed "
                        + command.getRequirements());
              }
            });
  }

  /**
   * Run before commands and triggers
   *
   * <p>Run periodically before commands are run - read sensors, etc. Include all classes that have
   * periodic inputs or other need to run periodically.
   *
   * <p>There are clever ways to register classes so they are automatically included in a list but
   * this example is simplistic - remember to type them in.
   */
  public void beforeCommands() {
    m_intake.beforeCommands();
    m_targetVision.beforeCommands();
    m_robotSignals.beforeCommands();
    m_historyFSM.beforeCommands();
    m_achieveHueGoal.beforeCommands();
    m_groupDisjointTest.beforeCommands();
  }

  /**
   * Run after commands and triggers
   *
   * <p>Run periodically after commands are run - write logs, dashboards, indicators Include all
   * classes that have periodic outputs
   *
   * <p>There are clever ways to register classes so they are automatically included in a list but
   * this example isn't it; simplistic - remember to type them in.
   */
  public void afterCommands() {
    m_intake.afterCommands();
    m_targetVision.afterCommands();
    m_robotSignals.afterCommands();
    m_historyFSM.afterCommands();
    m_achieveHueGoal.afterCommands();
    m_groupDisjointTest.afterCommands();
  }
}
