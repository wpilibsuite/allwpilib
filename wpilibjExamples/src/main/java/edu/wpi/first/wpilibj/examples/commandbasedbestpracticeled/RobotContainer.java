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

import static edu.wpi.first.units.Units.Milliseconds;
import static edu.wpi.first.units.Units.Seconds;
import static edu.wpi.first.wpilibj2.command.Commands.parallel;

import edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems.AchieveHueGoal;
import edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems.GroupDisjointTest;
import edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems.HistoryFSM;
import edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems.Intake;
import edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems.MooreLikeFSM;
import edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems.RobotSignals;
import edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems.RobotSignals.LEDPatternSupplier;
import edu.wpi.first.wpilibj.util.Color;
import edu.wpi.first.math.filter.Debouncer.DebounceType;
import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.LEDPattern;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.CommandScheduler;
import edu.wpi.first.wpilibj2.command.button.CommandXboxController;

public class RobotContainer {
  // define all the subsystems
  private final CommandXboxController m_operatorController;
  private final Intake m_intake;
  private final HistoryFSM m_historyFSM;
  private final AchieveHueGoal m_achieveHueGoal;
  private final MooreLikeFSM m_mooreLikeFSMtop;
  private final MooreLikeFSM m_mooreLikeFSMbottom;
  // container and creator of all the LEDView subsystems
  private final RobotSignals m_robotSignals;
  // container and creator of all the group/disjoint tests
  private final GroupDisjointTest m_groupDisjointTest;

  /**
   * Set the final values
   */
  public RobotContainer() {
    final int operatorControllerPort = 0;
    m_operatorController = new CommandXboxController(operatorControllerPort);
      
    // subsystems
    m_robotSignals = new RobotSignals();
    m_intake = new Intake(m_robotSignals.m_main);
    m_historyFSM = new HistoryFSM(m_robotSignals.m_historyDemo);
    m_achieveHueGoal = new AchieveHueGoal(m_robotSignals.m_achieveHueGoal);
    m_mooreLikeFSMtop = new MooreLikeFSM(m_robotSignals.m_knightRider, 10.0, Color.kRed);
    m_mooreLikeFSMbottom = new MooreLikeFSM(m_robotSignals.m_imposter, 9.9, Color.kOrange);
    m_groupDisjointTest = new GroupDisjointTest();

    configureBindings();

    configureDefaultCommands();

    // switch command logging on/off; a lot of output for the command execute methods
    final boolean logCommands = false;
    if (logCommands) {
      configureLogging();
    }
  }

  /**
   * configure driver and operator controllers' buttons
   */
  private void configureBindings() {

    /**
     * Use operator controller for a fake indicator game piece is acquired
     */
    m_operatorController.b().whileTrue(m_intake.gamePieceIsAcquired());

    /**
     * History FSM Control
     * Trigger if operator pressed "Y" button or it's time for a new color
     */
    var yButtonDebounceTime = Milliseconds.of(40.0);
    m_operatorController.y().debounce(yButtonDebounceTime.in(Seconds)).or(m_historyFSM::timesUp)
        .onTrue(m_historyFSM.newColor());

    /**
     * Start a color wheel display
     */
    var xButtonDebounceTime = Milliseconds.of(30.0);
    m_operatorController
        .x()
        .debounce(xButtonDebounceTime.in(Seconds), DebounceType.kBoth)
        .onTrue(m_robotSignals.m_top.setSignal(colorWheel()));

   /**
      * Goal setting demo control
    *
    * The PID controller is not running initially until a setpoint is set by moving the Xbox
    * right trigger axis past the threshold at which time a command runs forever to accept new
    * setpoints. The reset function on the Xbox A button interrupts the command that accepts new
    * setpoints and stops the underlying controller process.
    */
    var triggerHueGoalDeadBand = 0.05; // triggers if past a small threshold (scale of 0 to 1)
    m_operatorController.rightTrigger(triggerHueGoalDeadBand)
        .onTrue(
            m_achieveHueGoal.m_hueGoal.setHueGoal( // goal-acceptance command
                () ->
                    m_operatorController.getRightTriggerAxis()
                        * 180.0 // supplying the current value
                // scale joystick's 0 to 1 to computer color wheel hue 0 to 180
                ));

    m_operatorController
        .a()
        .onTrue(m_achieveHueGoal.m_hueGoal.reset()); // stops accepting goals by interrupting the
                                          // goal-acceptance command and halts the underlying
                                          // process by sending a message to it.
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
                (int) (Timer.getFPGATimestamp() % 60.0 /* seconds of the minute */)
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
    final LEDPattern topDefaultSignal = LEDPattern.solid(new Color(0.0, 0.0, 1.0));
    final LEDPattern mainDefaultSignal = LEDPattern.solid(new Color(0.0, 1.0, 1.0));
    final LEDPattern disabled = LEDPattern.solid(Color.kRed).breathe(Seconds.of(2.0));
    final LEDPattern enabled = LEDPattern.solid(Color.kGreen).breathe(Seconds.of(2.0));
    final LEDPatternSupplier enableDisableDefaultSignal =
        () -> DriverStation.isDisabled() ? disabled : enabled;

    final Command topDefault =
        m_robotSignals
            .m_top
            .setSignal(topDefaultSignal)
            .ignoringDisable(true)
            .withName("TopDefault");
    final Command mainDefault =
        m_robotSignals
            .m_main
            .setSignal(mainDefaultSignal)
            .ignoringDisable(true)
            .withName("MainDefault");
    final Command enableDisableDefault =
        m_robotSignals
            .m_enableDisable
            .setSignal(enableDisableDefaultSignal)
            .ignoringDisable(true)
            .withName("EnableDisableDefault");

    m_robotSignals.m_top.setDefaultCommand(topDefault);
    m_robotSignals.m_main.setDefaultCommand(mainDefault);
    m_robotSignals.m_enableDisable.setDefaultCommand(enableDisableDefault);
  }

  /**
   * Create a command to signal in Autonomous
   *
   * <p>Example of setting two signals by contrived example of composed commands
   *
   * @return
   */
  public Command setAutonomousSignal() {
    LEDPattern autoTopSignal =
        LEDPattern.solid(new Color(0.1, 0.2, 0.2))
            .blend(LEDPattern.solid(new Color(0.7, 0.2, 0.2)).blink(Seconds.of(0.1)));
          
    LEDPattern autoMainSignal = LEDPattern.solid(new Color(0.3, 1.0, 0.3));
    // statements before the return are run early at initialization time
    return
    // statements returned are run later when the command is scheduled
    parallel(
            // interrupting either of the two parallel commands with an external command interrupts
            // the group
            m_robotSignals.m_top.setSignal(autoTopSignal)
                .withTimeout(6.0)/*.asProxy()*/ // timeout ends but the group continues and
            // the default command is not activated here with or without the ".andThen" command.
            // Use ".asProxy()" to disjoint the group and allow the "m_top" default command to run.
            // What happened to the ".andThen"? Beware using Proxy can cause surprising behavior!
                .andThen(m_robotSignals.m_top.setSignal(autoTopSignal)),

            m_robotSignals.m_main.setSignal(autoMainSignal))
        .withName("AutoSignal");
  }

  /**
   * Get disjointed sequence test from its creator for use by Robot - passing the reference up
   * 
   * @return Command to be scheduled to run disjointed sequence test
   */
  public Command getDisjointedSequenceTest() {
    return m_groupDisjointTest.m_disjointedSequenceTest;
  }

  /**
   * Configure Command logging
   */
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
   * There are a variety of methods to run methods periodically and the example implemented below
   * is a very simplistic start of a good possibility.
   * 
   * It demonstrates running before the scheduler loop to get a consistent set of sensor inputs and
   * running after the scheduler loop for all periodic outputs.
   * 
   * There are clever ways to register classes say using a common "SubsystemTeam" class or
   * interface with a "register" method so they are automatically included in a list that can
   * easily be accessed with a loop. But this example is simplistic with no registration and no
   * loop - remember to type them in here and any class that has multiple subsystems such as the 
   * example "GroupDisjointTest".
   * 
   * There are a variety of ways to implement periodic methods depending on how important it is to
   * secure the periodic methods from unauthorized use. The example here is all "public" and any
   * other method with access to a class could run the periodicals of that class.
   * 
   * The next level of security would be to create a class say "SubsystemTeam" in a folder with all
   * other subsystems and have Robot periodic call SubsystemTeam periodicals. Each subsystem could
   * have "protected" periodicals since they are in the same package as the calling class.
   * 
   * Another secure method that works well in all cases but may not be very performant except in
   * limited cases or in conjunction with using "SubsystemTeam" to manage periodicals is to verify
   * the caller of the periodic methods. See "AchieveHueGoal" for an example implementation that
   * would be best applied to only one common periodic manager say the "SubsystemTeam" abstract
   * class or interface.
   * 
   * Another method to run securely periodically is to inject the "Robot.addPeriodic" method into
   * other classes. "addPeriodic" accurately runs on a "clock" within a time step. It could run
   * fairly accurately near the beginning of a time step (0 offset) but would not accurately run
   * "after" the triggers and scheduled commands as it isn't known exactly when they end.
   * "addPeriodic" is limited to running near the beginning (0 offset) or near the end (say 19 ms
   * offset) of the schedule loop but not precisely the beginning or end of the loop.
   * 
   * in Robot:
   *   private final RobotContainer robotContainer = new RobotContainer(this::addPeriodic);
   * in RobotContainer:
   *   public RobotContainer(BiConsumer<Runnable, Double> addPeriodicMethod) {
   *   exampleFlywheel = Flywheel(addPeriodicMethod);
   * in ExampleFlyWheel:
   *   Similarly get the method through the ctor and use it as "addPeriodic" is documented.
   */

  /**
   * Run before commands and triggers from the Robot.periodic()
   *
   * <p>Run periodically before commands are run - read sensors, etc. Include all classes that have
   * periodic inputs or other need to run periodically.
   *
   */
  public void runBeforeCommands() {
    m_intake.runBeforeCommands();
    m_robotSignals.runBeforeCommands();
    m_historyFSM.runBeforeCommands();
    m_achieveHueGoal.runBeforeCommands();
    m_groupDisjointTest.runBeforeCommands();
    m_mooreLikeFSMtop.runBeforeCommands();
    m_mooreLikeFSMbottom.runBeforeCommands();
  }

  /**
   * Run after commands and triggers from the Robot.periodic()
   *
   * <p>Run periodically after commands are run - write logs, dashboards, indicators Include all
   * classes that have periodic outputs
   */
  public void runAfterCommands() {
    m_intake.runAfterCommands();
    m_robotSignals.runAfterCommands();
    m_historyFSM.runAfterCommands();
    m_achieveHueGoal.runAfterCommands();
    m_groupDisjointTest.runAfterCommands();
    m_mooreLikeFSMtop.runAfterCommands();
    m_mooreLikeFSMbottom.runAfterCommands();
  }
}
