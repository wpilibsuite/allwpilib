// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot;

import static edu.wpi.first.units.Units.Seconds;
import static edu.wpi.first.wpilibj2.command.Commands.deadline;
import static edu.wpi.first.wpilibj2.command.Commands.parallel;
import static edu.wpi.first.wpilibj2.command.Commands.race;
import static edu.wpi.first.wpilibj2.command.Commands.runOnce;
import static edu.wpi.first.wpilibj2.command.Commands.sequence;
import static edu.wpi.first.wpilibj2.command.Commands.waitSeconds;

import edu.wpi.first.math.filter.Debouncer.DebounceType;
import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.CommandScheduler;
import edu.wpi.first.wpilibj2.command.ParallelCommandGroup;
import edu.wpi.first.wpilibj2.command.ParallelDeadlineGroup;
import edu.wpi.first.wpilibj2.command.ParallelRaceGroup;
import edu.wpi.first.wpilibj2.command.button.CommandXboxController;
import edu.wpi.first.wpilibj2.command.button.Trigger;
import frc.robot.subsystems.AchieveHueGoal;
import frc.robot.subsystems.GroupDisjointTest;
import frc.robot.subsystems.HistoryFSM;
import frc.robot.subsystems.IntakeSubsystem;
import frc.robot.subsystems.RobotSignals;
import frc.robot.subsystems.RobotSignals.LEDPatternSupplier;
import frc.robot.subsystems.TargetVisionSubsystem;

public class RobotContainer {

  // runtime options; too rigid - could be made easier to find and change but this is just a "simple" example program
  private boolean logCommands = false; // switch command logging on/off; a lot of output for the command execute methods
  private boolean useTriggeredJob = true; // select runtime option run tests as Triggered job or run as Commands.sequence

  // define all the subsystems
  private int operatorControllerPort = 0; 
  private final CommandXboxController operatorController = new CommandXboxController(operatorControllerPort);
  private final IntakeSubsystem intake;
  private final TargetVisionSubsystem vision;
  private final HistoryFSM historyFSM;
  private final AchieveHueGoal achieveHueGoal;
  private final GroupDisjointTest[] groupDisjointTest = {new GroupDisjointTest("A"), new GroupDisjointTest("B"), new GroupDisjointTest("C")};
  private final int A = 0; // subsystem id is subscript on the array of GroupDisjointTest subsystems
  private final int B = 1;
  private final int C = 2;
  private final RobotSignals robotSignals; // container for all the LEDView subsystems

  public Command disjointedSequenceTestJob; // Command to be scheduled for running test job

  public RobotContainer() {

    robotSignals = new RobotSignals(1);
    intake = new IntakeSubsystem(robotSignals.Main, operatorController);
    vision = new TargetVisionSubsystem(robotSignals.Top, operatorController);
    historyFSM = new HistoryFSM(robotSignals.HistoryDemo, operatorController);
    achieveHueGoal = new AchieveHueGoal(robotSignals.AchieveHueGoal);
 
    configureBindings();

    configureDefaultCommands();

    configureTestJob(useTriggeredJob);

    if(logCommands) configureLogging();

    { // junk
//FIXME research start of checking requirements if that verification must be added to all the disjoint methods
    var cmd1 = waitSeconds(1.);
    var req1 = cmd1.getRequirements();
    if (!req1.isEmpty()) cmd1 = cmd1.asProxy();

    var cmd2 = groupDisjointTest[0].testDuration(1, 0.);
    var req2 = cmd2.getRequirements();
    if (!req2.isEmpty()) cmd2 = cmd2.asProxy();

    System.out.println(req1 + " " + req2);
    System.out.println(cmd1.getRequirements() + " " + cmd2.getRequirements());
    }

  }
   
  /**
   * configure driver and operator controllers' buttons
   * (if they haven't been defined)
   */
    private void configureBindings() {

    operatorController.x().debounce(0.03, DebounceType.kBoth)
      .onTrue(robotSignals.Top.setSignal(colorWheel()));

    new Trigger(operatorController.rightTrigger(0.05)) // triggers if past a small threshold
      .onTrue(achieveHueGoal.hueGoal.setHueGoal(             // then it's always on
        ()->operatorController.getRightTriggerAxis() * 180.  // supplying the current value; scale joystick's 0 to 1 to computer color wheel hue 0 to 180
        )
      );
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
   * Configure some of the Default Commands
   * 
   * DANGER DANGER DANGER
   * 
   * Default commands are not run in composed commands.
   * 
   * Suggest not using default commands to prevent assuming they run.
   * (Example included on how to disable the setDefaultCommand)
   * 
   * Alternatively use the "disjointSequence()" method that allows commands to
   * release their resources upon completion instead of waiting for completion
   * of the entire command composition then the default command runs upon
   * completion of each individual command. (Example included)
   * 
   * If using the default command, suggest not setting it more than once
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

    groupDisjointTest[A].setDefaultCommand();
    groupDisjointTest[B].setDefaultCommand();
    groupDisjointTest[C].setDefaultCommand();
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

  /* 
  * Sample program to demonstrate loose coupling of Commands in a sequential grouping.
  *
  * A standard sequence locks all of the requirements of all of the subsystems used in
  * the group for the duration of the sequential execution. Default commands do not run
  * until the entire sequence completes.
  * 
  * A loosely connected sequential group demonstrates that each command runs
  * independently and when a command ends its subsystems' default commands run.
  * 
  * Running the same sequence normally with demonstrates that the subsystems'
  * requirements are maintained for the entire group execution duration and
  * the default commands are not activated until the sequence group ends.
  */

  /**
   * Define all tests of the use of Proxy or not to activate or not default commands
   * within command groups.
   * 
   * Create a job that runs all tests.
   * 
   * Option to run all tests as a succession of triggered Commands or as a single
   * Commands.sequence(). With triggering we may only have to fuss with Proxy within
   * each command as there is no interaction between commands. Normally the results are
   * identical except triggering needs one iteration to start the next command and sequence
   * needs two iterations to start the next command.
   * 
   * @param useTriggeredJob true for Triggered jobs and false for use Commands.sequence()
   */
  private void configureTestJob (boolean useTriggeredJob) {

  // Default commands running for A, B, and C.
  // Observe default commands don't run in groups unless disjointed by use of Proxy

  // Can't use decorators .andThen .alongWith on commands of subsystems needing default
  // within the group - reform as parallel() or sequence().

  // Add .asProxy() to commands of subsystems needing the default to run in the group.
  // Could proxyAll() but really only need on subsystems needing default to run in group
  // test with sequences in parallel and other nested complications

    final Command testSequence =
      sequence(
        groupDisjointTest[A].testDuration(1, 0.), waitSeconds(0.1), groupDisjointTest[A].testDuration(2, 0.));

    final Command testDisjointSequence =
      disjointSequence(
        groupDisjointTest[A].testDuration(1, 0.), waitSeconds(0.1), groupDisjointTest[A].testDuration(2, 0.));

    final Command testRepeatingSequence =
      sequence(
        groupDisjointTest[A].testDuration(1, 0.05),
        groupDisjointTest[B].testDuration(1, 0.05),
        groupDisjointTest[C].testDuration(1, 0.05)
      )
      .repeatedly().withTimeout(0.5);

    final Command testDisjointRepeatingSequence =
      disjointSequence(
        groupDisjointTest[A].testDuration(1, 0.05),
        groupDisjointTest[B].testDuration(1, 0.05),
        groupDisjointTest[C].testDuration(1, 0.05)
      )
      .repeatedly().withTimeout(0.5);

    // // Use of Proxy hides the error of having two commands running at once for the same subsystem.
    // // Check for such errors by removing the Proxy and constructing the command.
    // // Unhandled exception: java.lang.IllegalArgumentException:
    // //    Multiple commands in a parallel composition cannot require the same subsystems
    // final Command testParallelBadresults =
    //   parallel( // proxy hides same subsystem used twice at the same time; no error message but erroneous results
    //     groupDisjointTest[A].testDuration(1, 0.1).asProxy(),
    //     groupDisjointTest[A].testDuration(2, 0.1).asProxy()
    //   );

    final Command testParallel =
      parallel(
        sequence(
          groupDisjointTest[B].testDuration(1, 0.74),
          parallel(
            groupDisjointTest[A].testDuration(1, 0.84),
            groupDisjointTest[B].testDuration(2, 1.))),
        groupDisjointTest[C].testDuration(1, 0.6)
      );

    final Command testDisjointParallel =
      disjointParallel(
        disjointSequence(
          groupDisjointTest[B].testDuration(1, 0.74),
          disjointParallel(
            groupDisjointTest[A].testDuration(1, 0.84),
            groupDisjointTest[B].testDuration(1, 1.))),
        groupDisjointTest[C].testDuration(1, 0.6)
      );

    final Command testManualDisjointParallel =
      parallel(
        sequence(
          groupDisjointTest[B].testDuration(1, 0.74).asProxy(),
          parallel(
            groupDisjointTest[A].testDuration(1, 0.84).asProxy(),
            groupDisjointTest[B].testDuration(2, 1.).asProxy())),
        groupDisjointTest[C].testDuration(1, 0.6).asProxy()
      );

    final Command testDeadline =
      deadline(
        sequence(groupDisjointTest[A].testDuration(1, 0.1), waitSeconds(0.2)),
        sequence(groupDisjointTest[B].testDuration(1, 0.12)),
        sequence(groupDisjointTest[C].testDuration(1, 0.4))
      );

    final Command testDisjointDeadline =
      disjointDeadline(
        disjointSequence(groupDisjointTest[A].testDuration(1, 0.1), waitSeconds(0.2)),
        disjointSequence(groupDisjointTest[B].testDuration(1, 0.12)),
        disjointSequence(groupDisjointTest[C].testDuration(1, 0.4))
      );

    final Command testRace =
      race(
        disjointSequence(groupDisjointTest[A].testDuration(1, 0.24)),
        disjointSequence(groupDisjointTest[B].testDuration(1, 0.12), waitSeconds(0.3)),
        disjointSequence(groupDisjointTest[C].testDuration(1, 0.12), waitSeconds(0.3))
      );

    final Command testDisjointRace =
      disjointRace(
        disjointSequence(groupDisjointTest[A].testDuration(1, 0.24)),
        disjointSequence(groupDisjointTest[B].testDuration(1, 0.12), waitSeconds(0.3)),
        disjointSequence(groupDisjointTest[C].testDuration(1, 0.12), waitSeconds(0.3))
      );

    // illegal - Multiple commands in a parallel composition cannot require the same subsystems
    // public final Command testParallel =
    //   parallel(
    //     groupedDisjointTest.setTest(1).repeatedly(),
    //     waitSeconds(0.1).andThen(groupedDisjointTest.setTest(2)));

    Command[] allTests =
    {
      // need a "Runnable" as a supplier for the dynamic "teleopIterationCounter"
      // The print Command is static so its "teleopIterationCounter" would be the value when the command was made - not run.
      // "teleopIterationCounter" isn't much value in these examples - a curiosity to see the timing.

      runOnce(()->System.out.println("\nSTART testSequence " + Robot.teleopIterationCounter)),
       testSequence,
        runOnce(()->System.out.println("\nEND testSequence " + Robot.teleopIterationCounter)),
      runOnce(()->System.out.println("\nSTART testDisjointSequence " + Robot.teleopIterationCounter)),
       testDisjointSequence,
        runOnce(()->System.out.println("\nEND testDisjointSequence " + Robot.teleopIterationCounter)),
      runOnce(()->System.out.println("\nSTART testRepeatingSequence " + Robot.teleopIterationCounter)),
       testRepeatingSequence,
        runOnce(()->System.out.println("\nEND testRepeatingSequence " + Robot.teleopIterationCounter)),
      runOnce(()->System.out.println("\nSTART testDisjointRepeatingSequence " + Robot.teleopIterationCounter)),
       testDisjointRepeatingSequence,
        runOnce(()->System.out.println("\nEND testDisjointRepeatingSequence " + Robot.teleopIterationCounter)),
      runOnce(()->System.out.println("\nSTART testParallel " + Robot.teleopIterationCounter)),
       testParallel,
        runOnce(()->System.out.println("\nEND testParallel " + Robot.teleopIterationCounter)),
      runOnce(()->System.out.println("\nSTART testDisjointParallel " + Robot.teleopIterationCounter)),
       testDisjointParallel,
        runOnce(()->System.out.println("\nEND testDisjointParallel " + Robot.teleopIterationCounter)),
      runOnce(()->System.out.println("\nSTART testManualDisjointParallel " + Robot.teleopIterationCounter)),
       testManualDisjointParallel,
        runOnce(()->System.out.println("\nEND testManualDisjointParallel " + Robot.teleopIterationCounter)),
      runOnce(()->System.out.println("\nSTART testDeadlineParallel " + Robot.teleopIterationCounter)),
       testDeadline,
        runOnce(()->System.out.println("\nEND testDeadlineParallel " + Robot.teleopIterationCounter)),
      runOnce(()->System.out.println("\nSTART testDisjointDeadlineParallel " + Robot.teleopIterationCounter)),
       testDisjointDeadline,
        runOnce(()->System.out.println("\nEND testDisjointDeadlineParallel " + Robot.teleopIterationCounter)),
      runOnce(()->System.out.println("\nSTART testRaceParallel " + Robot.teleopIterationCounter)),
       testRace,
        runOnce(()->System.out.println("\nEND testRaceParallel " + Robot.teleopIterationCounter)),
      runOnce(()->System.out.println("\nSTART testDisjointRaceParallel " + Robot.teleopIterationCounter)),
       testDisjointRace,
        runOnce(()->System.out.println("\nEND testDisjointRaceParallel " + Robot.teleopIterationCounter)),
      runOnce(()->
              {
                // stop default commands to stop the output
                CommandScheduler.getInstance().cancel(groupDisjointTest[A].getDefaultCommand());
                CommandScheduler.getInstance().cancel(groupDisjointTest[B].getDefaultCommand());
                CommandScheduler.getInstance().cancel(groupDisjointTest[C].getDefaultCommand());
                groupDisjointTest[A].removeDefaultCommand();
                groupDisjointTest[B].removeDefaultCommand();
                groupDisjointTest[C].removeDefaultCommand();
              })
    };

    if (useTriggeredJob) {
      disjointedSequenceTestJob = TriggeredDisjointSequence.sequence(allTests);
    }
    else {
      disjointedSequenceTestJob = disjointSequence (allTests);
    }
  }

  /**
   * Run periodically before commands are run - read sensors, etc.
   * Include all classes that have periodic inputs or other need to run periodically.
   *
   * There are clever ways to register classes so they are automatically
   * included in a list but this example is simplistic - remember to type them in.
   */
  public void beforeCommands() {

    intake.beforeCommands();
    vision.beforeCommands();
    robotSignals.beforeCommands();
    historyFSM.beforeCommands();
    achieveHueGoal.beforeCommands();
    groupDisjointTest[0].afterCommands();
    groupDisjointTest[1].afterCommands();
    groupDisjointTest[2].afterCommands();
  }

  /**
   * Run periodically after commands are run - write logs, dashboards, indicators
   * Include all classes that have periodic outputs
   * 
   * There are clever ways to register classes so they are automatically
   * included in a list but this example is simplistic - remember to type them in.
   */
  public void afterCommands() {

    intake.afterCommands();
    vision.afterCommands();
    robotSignals.afterCommands();
    historyFSM.afterCommands();
    achieveHueGoal.afterCommands();
    groupDisjointTest[0].afterCommands();
    groupDisjointTest[1].afterCommands();
    groupDisjointTest[2].afterCommands();
  }

  /*********************************************************************************************************/
  /*********************************************************************************************************/
  /*********************************************************************************************************/
  // anticipated to be included in an upcoming WPILib release
  /*********************************************************************************************************/
  /*********************************************************************************************************/
  /*********************************************************************************************************/

  /**
   * Runs individual commands in a series without grouped behavior.
   *
   * <p>Each command is run independently by proxy. The requirements of
   * each command are reserved only for the duration of that command and
   * are not reserved for an entire group process as they are in a
   * grouped sequence.
   * 
   * <p>disjoint...() does not propagate to interior groups. Use additional disjoint...() as needed.
   *
   * @param commands the commands to include in the series
   * @return the command to run the series of commands
   * @see #sequence(Command...) use sequence() to invoke group sequence behavior
   */
  public static Command disjointSequence(Command... commands) {
    return sequence(proxyAll(commands));
  }

  /**
   * Runs individual commands in a series without grouped behavior; once the last command ends, the series is restarted.
   *
   * <p>Each command is run independently by proxy. The requirements of
   * each command are reserved only for the duration of that command and
   * are not reserved for an entire group process as they are in a
   * grouped sequence.
   * 
   * <p>disjoint...() does not propagate to interior groups. Use additional disjoint...() as needed.
   *
   * @param commands the commands to include in the series
   * @return the command to run the series of commands repeatedly
   * @see #sequence(Command...) use sequenceRepeatedly() to invoke repeated group sequence behavior
   * @see #disjointSequence(Command...)
   * @see Command#repeatedly() 
   */
  public static Command repeatingDisjointSequence(Command... commands) {
    return disjointSequence(commands).repeatedly();
  }

  /**
   * Runs individual commands at the same time without grouped behavior; when the deadline command ends the otherCommands are cancelled.
   *
   * <p>Each otherCommand is run independently by proxy. The requirements of
   * each command are reserved only for the duration of that command and are
   * not reserved for an entire group process as they are in a grouped deadline.
   *
   * <p>disjoint...() does not propagate to interior groups. Use additional disjoint...() as needed.
   *
   * @param deadline the deadline command
   * @param otherCommands the other commands to include and will be cancelled when the deadline ends
   * @return the command to run the deadline command and otherCommands
   * @see #deadline(Command, Command...) use deadline() to invoke group parallel deadline behavior
   * @throws IllegalArgumentException if the deadline command is also in the otherCommands argument
   */
  public static Command disjointDeadline(Command deadline, Command... otherCommands) {
    new ParallelDeadlineGroup(deadline, otherCommands); // check parallel deadline constraints
    CommandScheduler.getInstance().removeComposedCommand(deadline);
    for (Command cmd : otherCommands) CommandScheduler.getInstance().removeComposedCommand(cmd);
    return deadline(deadline, proxyAll(otherCommands));
  }

  /**
   * Runs a group of commands at the same time. Ends once any command in the group finishes, and
   * cancels the others.
   *
   * @param commands the commands to include
   * @return the command group
   * @see ParallelRaceGroup
   */
  public static Command disjointRace(Command... commands) {
    new ParallelRaceGroup(commands); // check parallel constraints
    for (Command cmd : commands) CommandScheduler.getInstance().removeComposedCommand(cmd);
    return race(proxyAll(commands));
  }

  /**
   * Runs individual commands at the same time without grouped behavior and ends once all commands finish.
   *
   * <p>Each command is run independently by proxy. The requirements of
   * each command are reserved only for the duration of that command and
   * are not reserved for an entire group process as they are in a
   * grouped parallel.
   * 
   * <p>disjoint...() does not propagate to interior groups. Use additional disjoint...() as needed.
   *
   * @param commands the commands to run in parallel
   * @return the command to run the commands in parallel
   * @see #parallel(Command...) use parallel() to invoke group parallel behavior
   */
  public static Command disjointParallel(Command... commands) {
    new ParallelCommandGroup(commands); // check parallel constraints
    for (Command cmd : commands) CommandScheduler.getInstance().removeComposedCommand(cmd);
    return parallel(proxyAll(commands));
  }

  // to be included in an upcoming WPILib release
  /**
   * Maps an array of commands by proxying every element using {@link Command#asProxy()}.
   *
   * <p>This is useful to ensure that default commands of subsystems within a command group are
   * still triggered despite command groups requiring the union of their members' requirements
   *
   * <p>Example usage for creating an auto for a robot that has a drivetrain and arm:
   *
   * <pre>
   * {@code var auto = sequence(proxyAll(drive.move(), arm.score()));}
   * </pre>
   *
   * @param commands an array of commands
   * @return an array of proxied commands
   */
  public static Command[] proxyAll(Command... commands) {
    Command[] out = new Command[commands.length];
    for (int i = 0; i < commands.length; i++) {
      out[i] = commands[i].asProxy();
    }
    return out;
  }
}

/*
Triggered sequence - no default commands running

START testSequence 0
A1A2
END testSequence 10

START testDisjointSequence 11
A1A2
END testDisjointSequence 23

START testRepeatingSequence 24
A1A1A1A1B1B1B1C1C1C1A1A1A1A1B1B1B1C1C1C1A1A1A1A1B1B1
END testRepeatingSequence 51

START testDisjointRepeatingSequence 52
A1A1A1A1B1B1B1C1C1C1A1B1A1B1A1B1C1C1C1A1B1A1B1A1B1
END testDisjointRepeatingSequence 79

START testParallel 80
B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1B1B1B1B1B1B1B1A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2B2B2B2B2B2B2B2B2
END testParallel 171

START testDisjointParallel 172
B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1B1B1B1B1B1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1A1B1B1B1B1B1B1B1B1B1
END testDisjointParallel 265

START testManualDisjointParallel 266
B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1B1B1B1B1B1B1B1A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2B2B2B2B2B2B2B2B2B2
END testManualDisjointParallel 359

START testDeadlineParallel 360
A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1
END testDeadlineParallel 373

START testDisjointDeadlineParallel 374
A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1B1C1B1C1
END testDisjointDeadlineParallel 388

START testRaceParallel 389
A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1B1C1
END testRaceParallel 403

START testDisjointRaceParallel 404
A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1B1C1B1C1
END testDisjointRaceParallel 419
*/


/*
triggered sequence - default commands running

START testSequence 0
A1BdCdBdCdBdCdBdCdBdCdBdCdBdCdA2BdCd
END testSequence 10

AdBdCd

START testDisjointSequence 11
AdBdCdA1BdCdAdBdCdAdBdCdAdBdCdAdBdCdAdBdCdAdBdCdAdBdCdA2BdCdAdBdCd
END testDisjointSequence 22

AdBdCd

START testRepeatingSequence 23
AdBdCdA1A1A1A1B1B1B1C1C1C1A1A1A1A1B1B1B1C1C1C1A1A1A1A1B1B1
END testRepeatingSequence 50

AdBdCd

START testDisjointRepeatingSequence 51
AdBdCdA1BdCdA1BdCdA1BdCdA1BdCdAdBdCdAdB1CdAdB1CdAdB1CdAdBdCdAdBdC1AdBdC1AdBdC1AdBdCdAdBdCdA1B1CdA1B1CdA1B1CdAdBdCdAdBdC1AdBdC1AdBdC1AdBdCdAdBdCdA1B1CdA1B1CdA1B1CdAdBdCd
END testDisjointRepeatingSequence 79

AdBdCd

START testParallel 80
AdBdCdB1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1B1B1B1B1B1B1A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2B2B2B2B2B2B2B2B2B2
END testParallel 171

AdBdCd

START testDisjointParallel 172
AdBdCdAdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdBdCdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1
CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdAdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdBdCdAdBdCdAdBdCd
END testDisjointParallel 267

AdBdCd

START testManualDisjointParallel 268
AdBdCdAdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdBdCdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdAdB2CdAdB2CdAdB2CdAdB2CdAdB2CdAdB2CdAdB2CdAdB2CdAdBdCd
END testManualDisjointParallel 359

AdBdCd

START testDeadlineParallel 360
AdBdCdA1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1C1A1C1A1C1A1C1A1C1A1C1A1C1
END testDeadlineParallel 375

AdBdCd

START testDisjointDeadlineParallel 376
AdBdCdA1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1BdC1A1BdC1A1BdC1A1BdC1A1BdC1AdBdC1AdBdC1
END testDisjointDeadlineParallel 392

AdBdCd

START testRaceParallel 393
AdBdCdA1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1BdCdA1BdCdA1BdCdA1BdCdA1BdCdA1BdCdAdBdCd
END testRaceParallel 408

AdBdCd

START testDisjointRaceParallel 409
AdBdCdA1B1C1A1B1C1A1B1C1A1B1C1A1B1
C1A1B1C1A1BdCdA1BdCdA1BdCdA1BdCdA1BdCdA1BdCdA1BdCdA1BdCdAdBdCdAdBdCd
END testDisjointRaceParallel 426

AdBdCd

*/

/*
disjointSequence - default commands running

START testSequence 0
AdBdCdA1BdCdBdCdBdCdBdCdBdCdBdCdBdCdA2BdCd
END testSequence 11

AdBdCdAdBdCd

START testDisjointSequence 13
AdBdCdAdBdCdAdBdCdA1BdCdAdBdCdAdBdCdAdBdCdAdBdCdAdBdCdAdBdCdA2BdCdAdBdCd
END testDisjointSequence 25

AdBdCdAdBdCd

START testRepeatingSequence 27
AdBdCdAdBdCdAdBdCdA1A1A1B1B1B1C1C1C1A1A1A1A1B1B1B1C1C1C1A1A1A1A1B1B1B1
END testRepeatingSequence 56

AdBdCdAdBdCd

START testDisjointRepeatingSequence 58
AdBdCdAdBdCdAdBdCdA1BdCdA1BdCdA1BdCdAdBdCdAdB1CdAdB1CdAdB1CdAdBdCdAdBdC1AdBdC1AdBdC1AdBdCdAdBdCdA1B1CdA1B1CdA1B1CdAdBdCdAdBdC1AdBdC1AdBdC1AdBdCdAdBdCdA1B1CdA1
B1Cd
END testDisjointRepeatingSequence 85

AdBdCdAdBdCd

START testParallel 87
AdBdCdAdBdCdAdBdCdB1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1B1B1B1B1B1A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2B2B2B2B2B2B2B2B2
END testParallel 178

AdBdCdAdBdCd

START testDisjointParallel 180
AdBdCdAdBdCdAdBdCdAdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdBdCdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdWarning at edu.wpi.first.wpilibj.IterativeRobotBase.printLoopOverrunMessage(IterativeRobotBase.java:412): Loop time of 0.02s overrun
A1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdAdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdBdCdAdBdCdAdBdCd
END testDisjointParallel 276

AdBdCdAdBdCd

START testManualDisjointParallel 278
AdBdCdAdBdCdAdBdCdAdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdBdCdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdAdB2CdAdB2CdAdB2CdAdB2CdAdB2CdAdB2CdAdB2CdAdBdCd
END testManualDisjointParallel 371

AdBdCdAdBdCd

START testDeadlineParallel 373
AdBdCdAdBdCdAdBdCdA1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1C1A1C1A1C1A1C1A1C1A1C1
END testDeadlineParallel 389

AdBdCdAdBdCd

START testDisjointDeadlineParallel 391
AdBdCdAdBdCdAdBdCdA1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1BdC1A1BdC1A1BdC1A1BdC1A1BdC1AdBdC1AdBdC1
END testDisjointDeadlineParallel 408

AdBdCdAdBdCd

START testRaceParallel 410
AdBdCdAdBdCdAdBdCdA1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1BdCdA1BdCdA1BdCdA1BdCdA1BdCdAdBdCd
END testRaceParallel 426

AdBdCdAdBdCd

START testDisjointRaceParallel 428
AdBdCdAdBdCdAdBdCdA1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1BdCdA1BdCdA1BdCdA1BdCdA1BdCdA1BdCdA1BdCdAdBdCdAdBdCd
END testDisjointRaceParallel 445

AdBdCdAdBdCdAdBdCd
*/
