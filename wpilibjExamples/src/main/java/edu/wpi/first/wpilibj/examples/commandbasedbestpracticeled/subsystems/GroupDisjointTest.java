// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot.subsystems;

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

import static edu.wpi.first.wpilibj2.command.Commands.deadline;
import static edu.wpi.first.wpilibj2.command.Commands.parallel;
import static edu.wpi.first.wpilibj2.command.Commands.print;
import static edu.wpi.first.wpilibj2.command.Commands.race;
import static edu.wpi.first.wpilibj2.command.Commands.runOnce;
import static edu.wpi.first.wpilibj2.command.Commands.sequence;
import static edu.wpi.first.wpilibj2.command.Commands.waitSeconds;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.CommandScheduler;
import edu.wpi.first.wpilibj2.command.ParallelCommandGroup;
import edu.wpi.first.wpilibj2.command.ParallelDeadlineGroup;
import edu.wpi.first.wpilibj2.command.ParallelRaceGroup;
import frc.robot.TriggeredDisjointSequence;

public class GroupDisjointTest {

    // runtime options; too rigid - could be made easier to find and change but this is just a "simple" example program
    private boolean useTriggeredJob = true; // select runtime option run tests as Triggered job or run as Commands.sequence
        
    private final GroupDisjoint[] groupDisjoint = // subsystems that provide the requirements for testing
            {
                new GroupDisjoint("A"),
                new GroupDisjoint("B"),
                new GroupDisjoint("C")
            };
    private final int A = 0; // subsystem id is subscript on the array of groupDisjoint subsystems
    private final int B = 1;
    private final int C = 2;

    // This is "static" so it can be used simply in Robot.java.
    // That means only one instance of this class should be made since all instances would
    // use the same variable for the tests. Make this class a singleton to make sure.
    private static Command disjointedSequenceTestJob;
 
    /**
     * 
     * @return Command to be scheduled for running test job
     */
    public static Command disjointedSequenceTestJob() {
        return disjointedSequenceTestJob;
    }

    // singleton class so access to disjointedSequenceTestJob can be static safely
    private static GroupDisjointTest single_instance = null;
    private GroupDisjointTest() {
        
        configureTestJob(useTriggeredJob);
    }

    public static  GroupDisjointTest getInstance()
    {

        if (single_instance == null)
            single_instance = new GroupDisjointTest();
 
        return single_instance;
    }

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
   * @param useTriggeredJob true for Triggered jobs; false for use Commands.sequence()
   */
  private void configureTestJob (boolean useTriggeredJob) {

  // Default commands running for subsystems A, B, and C.
  // Observe default commands don't run in groups unless disjointed by use of Proxy

  // Can't use decorators .andThen .alongWith on commands of subsystems needing default
  // within the group - reform as parallel() or sequence().

  // Bug in WPILib "RepeatCommand" causes incorrect results for repeats.

  // Add .asProxy() to commands of subsystems needing the default to run in the group.
  // Could proxyAll() everything but really only need Proxy on commands with required
  // subsystems needing default command to run in the group.

    final Command testSequence =
      sequence(
        groupDisjoint[A].testDuration(1, 0.), waitSeconds(0.1), groupDisjoint[A].testDuration(2, 0.));

    final Command testDisjointSequence =
      disjointSequence(
        groupDisjoint[A].testDuration(1, 0.), waitSeconds(0.1), groupDisjoint[A].testDuration(2, 0.));

    final Command testRepeatingSequence =
      sequence(
        groupDisjoint[A].testDuration(1, 0.05),
        groupDisjoint[B].testDuration(1, 0.05),
        groupDisjoint[C].testDuration(1, 0.05)
      )
      .repeatedly().withTimeout(0.5);

    final Command testDisjointRepeatingSequence =
      disjointSequence(
        groupDisjoint[A].testDuration(1, 0.05),
        groupDisjoint[B].testDuration(1, 0.05),
        groupDisjoint[C].testDuration(1, 0.05)
      )
      .repeatedly().withTimeout(0.5);

    // final Command testDisjointRepeatingSequenceBlocked =
    //   repeatingDisjointSequence(
    //     groupDisjoint[A].testDuration(1, 0.05),
    //     groupDisjoint[B].testDuration(1, 0.05),
    //     groupDisjoint[C].testDuration(1, 0.05)
    //   )
    //   .withTimeout(0.5);

    // Illegal - Multiple commands in a parallel composition cannot require the same subsystems.
    // Throws an error message.
    // public final Command testParallel =
    //   parallel(
    //     groupedDisjointTest[A].setTest(1).repeatedly(),
    //     waitSeconds(0.1).andThen(groupedDisjointTest[A].setTest(2)));

    // Use of Proxy hides the error of having two commands running at once for the same subsystem.
    // No error message but erroneous results.
    // final Command testParallel =
    //   parallel(
    //     groupDisjoint[A].testDuration(1, 0.1).asProxy(),
    //     groupDisjoint[A].testDuration(2, 0.1).asProxy()
    //   );

    final Command testParallel =
      parallel(
        sequence(
          groupDisjoint[B].testDuration(1, 0.74),
          parallel(
            groupDisjoint[A].testDuration(1, 0.84),
            groupDisjoint[B].testDuration(2, 1.))),
        groupDisjoint[C].testDuration(1, 0.6)
      );

    final Command testDisjointParallel =
      disjointParallel(
        disjointSequence(
          groupDisjoint[B].testDuration(1, 0.74),
          disjointParallel(
            groupDisjoint[A].testDuration(1, 0.84),
            groupDisjoint[B].testDuration(1, 1.))),
        groupDisjoint[C].testDuration(1, 0.6)
      );

    final Command testManualDisjointParallel =
      parallel(
        sequence(
          groupDisjoint[B].testDuration(1, 0.74).asProxy(),
          parallel(
            groupDisjoint[A].testDuration(1, 0.84).asProxy(),
            groupDisjoint[B].testDuration(2, 1.).asProxy())),
        groupDisjoint[C].testDuration(1, 0.6).asProxy()
      );

    final Command testDeadline =
      deadline(
        sequence(groupDisjoint[A].testDuration(1, 0.1), waitSeconds(0.2)),
        sequence(groupDisjoint[B].testDuration(1, 0.12)),
        sequence(groupDisjoint[C].testDuration(1, 0.4))
      );

    final Command testDisjointDeadline =
      disjointDeadline(
        disjointSequence(groupDisjoint[A].testDuration(1, 0.1), waitSeconds(0.2)),
        disjointSequence(groupDisjoint[B].testDuration(1, 0.12)),
        disjointSequence(groupDisjoint[C].testDuration(1, 0.4))
      );

    final Command testRace =
      race(
        sequence(groupDisjoint[A].testDuration(1, 0.24)),
        sequence(groupDisjoint[B].testDuration(1, 0.12), waitSeconds(0.3)),
        sequence(groupDisjoint[C].testDuration(1, 0.12), waitSeconds(0.3))
      );

    final Command testDisjointRace =
      disjointRace(
        disjointSequence(groupDisjoint[A].testDuration(1, 0.24)),
        disjointSequence(groupDisjoint[B].testDuration(1, 0.12), waitSeconds(0.3)),
        disjointSequence(groupDisjoint[C].testDuration(1, 0.12), waitSeconds(0.3))
      );

    Command[] allTests =
    {
      // Printing a static message so use the Commands.print. If message contained variables, then
      // need a "Runnable" as a supplier for the dynamic values such as
      // ()->System.out.println(supplier variable or field variable). Otherwise the variable would
      // print its value from when the command was made - not when it's run.

      runOnce(()->
              {
                groupDisjoint[A].setDefaultCommand();
                groupDisjoint[B].setDefaultCommand();
                groupDisjoint[C].setDefaultCommand();
              }),
      print("\nSTART testSequence"),
       testSequence,
        print("\nEND testSequence"),
      print("\nSTART testDisjointSequence"),
       testDisjointSequence,
        print("\nEND testDisjointSequence"),
      print("\nSTART testRepeatingSequence"),
       testRepeatingSequence,
        print("\nEND testRepeatingSequence"),
      print("\nSTART testDisjointRepeatingSequence - incorrect results - library bug"),
       testDisjointRepeatingSequence,
        print("\nEND testDisjointRepeatingSequence - incorrect results - library bug"),
    //  print("\nSTART testDisjointRepeatingSequence - blocked - not supported"),
    //    testDisjointRepeatingSequenceBlocked,
    //     print("\nEND testDisjointRepeatingSequence - blocked - not supported"),
      print("\nSTART testParallel"),
       testParallel,
        print("\nEND testParallel"),
      print("\nSTART testDisjointParallel"),
       testDisjointParallel,
        print("\nEND testDisjointParallel"),
      print("\nSTART testManualDisjointParallel"),
       testManualDisjointParallel,
        print("\nEND testManualDisjointParallel"),
      print("\nSTART testDeadlineParallel"),
       testDeadline,
        print("\nEND testDeadlineParallel"),
      print("\nSTART testDisjointDeadlineParallel"),
       testDisjointDeadline,
        print("\nEND testDisjointDeadlineParallel"),
      print("\nSTART testRaceParallel"),
       testRace,
        print("\nEND testRaceParallel"),
      print("\nSTART testDisjointRaceParallel"),
       testDisjointRace,
        print("\nEND testDisjointRaceParallel"),
      runOnce(()->
              {
                // stop default commands to stop the output
                CommandScheduler.getInstance().cancel(groupDisjoint[A].getDefaultCommand());
                CommandScheduler.getInstance().cancel(groupDisjoint[B].getDefaultCommand());
                CommandScheduler.getInstance().cancel(groupDisjoint[C].getDefaultCommand());
                groupDisjoint[A].removeDefaultCommand();
                groupDisjoint[B].removeDefaultCommand();
                groupDisjoint[C].removeDefaultCommand();
              })
    };

    if (useTriggeredJob) {
      disjointedSequenceTestJob = TriggeredDisjointSequence.sequence(allTests);
    }
    else {
      disjointedSequenceTestJob = disjointSequence(allTests);
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

    groupDisjoint[0].afterCommands();
    groupDisjoint[1].afterCommands();
    groupDisjoint[2].afterCommands();
  }

  /**
   * Run periodically after commands are run - write logs, dashboards, indicators
   * Include all classes that have periodic outputs
   * 
   * There are clever ways to register classes so they are automatically
   * included in a list but this example isn't it; simplistic - remember to type them in.
   */
  public void afterCommands() {

    groupDisjoint[0].afterCommands();
    groupDisjoint[1].afterCommands();
    groupDisjoint[2].afterCommands();
  }

  /*********************************************************************************************************/
  /*********************************************************************************************************/
  /*********************************************************************************************************/
  // The following methods might be included in an upcoming WPILib release
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

//   // bug in WPILib prevents this from working correctly
//   // If this needs to be implemented with the bug present then do this:
//   //    throw new IllegalArgumentException("RepeatCommand bug prevents correct operation of asProxy/ProxyCommand");
//   /**
//    * Runs individual commands in a series without grouped behavior; once the last command ends, the series is restarted.
//    *
//    * <p>Each command is run independently by proxy. The requirements of
//    * each command are reserved only for the duration of that command and
//    * are not reserved for an entire group process as they are in a
//    * grouped sequence.
//    * 
//    * <p>disjoint...() does not propagate to interior groups. Use additional disjoint...() as needed.
//    *
//    * @param commands the commands to include in the series
//    * @return the command to run the series of commands repeatedly
//    * @see #sequence(Command...) use sequenceRepeatedly() to invoke repeated group sequence behavior
//    * @see #disjointSequence(Command...)
//    * @see Command#repeatedly() 
//    */
//   public static Command repeatingDisjointSequence(Command... commands) {

//     return disjointSequence(commands).repeatedly();
//   }

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
triggered sequence - default commands running; scrubbed to make pretty

START testSequence
AdBdCd
A1BdCdBdCdBdCdBdCdBdCdBdCdBdCd
A2BdCd
END testSequence

AdBdCd

START testDisjointSequence
AdBdCd
A1BdCdAdBdCdAdBdCdAdBdCdAdBdCdAdBdCdAdBdCdAdBdCd
A2BdCdAdBdCd
END testDisjointSequence

AdBdCd

START testRepeatingSequence
AdBdCdA1A1A1A1B1B1B1C1C1C1A1A1A1A1B1B1B1C1C1C1A1A1A1A1B1B1
END testRepeatingSequence

AdBdCd

START testDisjointRepeatingSequence - incorrect results - library bug
AdBdCdA1BdCdA1BdCdA1BdCdA1BdCdAdBdCdAdB1CdAdB1CdAdB1CdAdBdCdAdBdC1AdBdC1AdBdC1AdBdCdAdBdCd
A1B1CdA1B1CdA1B1CdAdBdCdAdBdC1AdBdC1AdBdC1AdBdCdAdBdCdA1B1CdA1B1CdA1B1CdAdBdCd
END testDisjointRepeatingSequence - incorrect results - library bug

AdBdCd

START testParallel
AdBdCd
B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1
B1C1B1C1B1C1B1C1B1C1B1C1B1
B1B1B1B1B1B1
A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2
A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2B2B2B2B2B2B2B2B2
END testParallel

AdBdCd

START testDisjointParallel
AdBdCdAdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1
AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1
AdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdB1Cd
AdBdCd
A1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1Cd
A1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1Cd
A1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1CdA1B1Cd
AdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdB1Cd
AdBdCdAdBdCdAdBdCd
END testDisjointParallel

AdBdCd

START testManualDisjointParallel
AdBdCd
AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1
AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1AdB1C1
AdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdB1Cd
AdBdCd
A1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2Cd
A1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2Cd
A1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2Cd
AdB2CdAdB2CdAdB2CdAdB2CdAdB2CdAdB2CdAdB2Cd
AdBdCd
END testManualDisjointParallel

AdBdCd

START testDeadlineParallel
AdBdCdA1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1B1C1C1C1C1C1C1C1C1C1C1C1
END testDeadlineParallel

AdBdCd

START testDisjointDeadlineParallel
AdBdCd
A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1AdB1C1
AdBdC1AdBdC1AdBdC1AdBdC1AdBdC1AdBdC1AdBdC1AdBdC1AdBdC1AdBdC1AdBdC1    
END testDisjointDeadlineParallel

AdBdCd

START testRaceParallel
AdBdCdA1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1A1A1A1A1A1
END testRaceParallel

AdBdCd

START testDisjointRaceParallel
AdBdCd
A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1
A1BdCdA1BdCdA1BdCdA1BdCdA1BdCdA1BdCd
AdBdCdAdBdCd
END testDisjointRaceParallel

AdBdCd
*/
