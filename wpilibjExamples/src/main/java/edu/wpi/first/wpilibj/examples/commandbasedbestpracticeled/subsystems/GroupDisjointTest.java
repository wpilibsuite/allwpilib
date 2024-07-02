// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems;

/*
 * Sample program to demonstrate loose coupling of Commands in a sequential grouping. (Each leg of
 * any parallel grouping is also considered a sequence and that is especially obvious if a default
 * command runs after the primary command ends.)
 *
 * A standard sequence locks all of the requirements of all of the subsystems used in
 * the group for the duration of the sequential execution. Default commands do not run
 * until the entire sequence completes.
 *
 * A loosely connected sequential group demonstrates that each command runs independently and when
 * a command ends its subsystems' default commands run.
 *
 * Running the same sequence normally demonstrates that the subsystems' requirements are maintained
 * for the entire group execution duration and the default commands are not activated until the
 * sequence group ends.
 * 
 * Note that this class didn't have to be a subsystem for correct results under expected
 * circumstances. In the spirit of "things can go wrong that shouldn't" and "use a good practice
 * instead of a bad practice" this class was made a subsystem to be safe and demonstrate some
 * pitfalls of disjoint sequences.
 * 
 * An expectation of these tests is only one test job runs at a time. The scheduler does prevent
 * the same command from running more than once at the same time. Subsystem usage is not required
 * for that protection feature.
 * 
 * If a command was duplicated, the two copies could run at the same time since they appear as
 * different commands. In that case the use of subsystem requirements is needed to prevent
 * simultaneous runs.
 * 
 * The three subsystems being used for tests have been disjointed as that is the point of these
 * tests. Their requirements would not prevent copies of the entire test job from running at the
 * same time. To treat the entire test job as one transaction a requirement must be made of the
 * complete command. Using this subsystem as a requirement on the runOnce commands locks in the
 * entire job as a single entity when it is run as proxy disjoint sequence. If this class isn't a
 * subsystem, then the runOnce doesn't have the automatically provided requirement. The default is
 * no requirement and a requirement must be manually provided. That is prone to errors of omission
 * at the least.
 * 
 * The use of a subsystem requirement as described above only works if the sequence of jobs is
 * disjointed by proxy and not by triggering. Triggered sequence is highly disjointed with
 * essentially no communication between the individual commands and no group behavior validation.
 * Putting a requirement on the sequence actually is applied to only the first command and all the
 * rest of the commands are independent.
 * 
 * Using a subsystem or not and using triggered sequencing instead of proxies all give essentially
 * identical results within one (more or less) loop iteration. The major differences are in the
 * security provided by subsystems and command group membership.
 * 
 * (run(), runOnce(), startEnd(), runEnd(), and defer() within a subsystem automatically include
 * the subsystem as a requirements so you won't forget it. Usage outside of the subsystem defaults
 * to no requirements and any needed requirements must be manually entered.)
 */

import static edu.wpi.first.units.Units.Seconds;
import static edu.wpi.first.wpilibj2.command.Commands.deadline;
import static edu.wpi.first.wpilibj2.command.Commands.parallel;
import static edu.wpi.first.wpilibj2.command.Commands.print;
import static edu.wpi.first.wpilibj2.command.Commands.race;
import static edu.wpi.first.wpilibj2.command.Commands.sequence;
import static edu.wpi.first.wpilibj2.command.Commands.waitSeconds;

import edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.TriggeredDisjointSequence;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.CommandScheduler;
import edu.wpi.first.wpilibj2.command.Commands;
import edu.wpi.first.wpilibj2.command.ParallelCommandGroup;
import edu.wpi.first.wpilibj2.command.ParallelDeadlineGroup;
import edu.wpi.first.wpilibj2.command.ParallelRaceGroup;
import edu.wpi.first.wpilibj2.command.SubsystemBase;

public class GroupDisjointTest extends SubsystemBase {
  // runtime option; too rigid - could be made easier to find and change but this is just a
  // "simple" example program

  // runtime option to run tests as Triggered job (true) or run as Commands.sequence (false)
  private final boolean m_useTriggeredJob = false;

  private final GroupDisjoint[]
      m_groupDisjoint = // subsystems that provide the requirements for testing
      {new GroupDisjoint("A"), new GroupDisjoint("B"), new GroupDisjoint("C")};
  private static final int m_a = 0; // subsystem id is subscript on the array of groupDisjoint subsystems
  private static final int m_b = 1;
  private static final int m_c = 2;

  public final Command m_disjointedSequenceTest; // created at ctor so available immediately on demand

  /**
   * Create disjointed sequence test upon ctor.
   * 
   * <p>Could create and return test on demand but this way can save some time when demanded.
   */
  public GroupDisjointTest() {
    m_disjointedSequenceTest = configureTestJob(m_useTriggeredJob);
  }

  /**
   * Define several tests of the use of Proxy, or not, to activate, or not, the default commands within command groups.
   *
   * The several tests run in a sequence optionally continuing to use the Proxy technique to
   * disjoint the tests or use successively triggered commands method. That is all the disjoint
   * tests use Proxies at a low level of inner commands and then those commands are additionally
   * demonstrated   * to be run in sequence by outer level proxies or triggered commands. Triggered
   * commands could have been used to disjoint all commands even the inner most ones.
   *
   * <p>Create a Command that runs the test.
   *
   * <p>Option to run the Proxy tests as a succession of triggered Commands or as a single
   * Commands.sequence(). With triggering we may only have to fuss with Proxy within each command as
   * there is no interaction between commands. Normally the results are identical within one loop
   * iteration.
   *
   * @param useTriggeredJob true for use Triggered commands; false for use Commands.sequence()
   */
  private Command configureTestJob(boolean useTriggeredJob) {
    // Default commands running for subsystems A, B, and C.
    // Observe default commands don't run within groups unless disjointed by use of Proxy or
    // triggered commands.

    // Can't use decorators .andThen or .alongWith on commands of subsystems needing default
    // command within the group - reform as parallel() or sequence().

    // Bug in WPILib "RepeatCommand" causes incorrect results for repeats.

    // Add .asProxy() to commands of subsystems needing the default command to run in the group.
    // Better to use proxyAll() to help add safely .asProxy() only to commands that have
    // requirements of subsystems needing default command to run in the group.

    final Command testSequence =
        sequence(
            m_groupDisjoint[m_a].testDuration(1, Seconds.of(0.)),
            waitSeconds(0.1),
            m_groupDisjoint[m_a].testDuration(2, Seconds.of(0.)));

    final Command testDisjointSequence =
        disjointSequence(
            m_groupDisjoint[m_a].testDuration(1, Seconds.of(0.)),
            waitSeconds(0.1),
            m_groupDisjoint[m_a].testDuration(2, Seconds.of(0.)));

    final Command testRepeatingSequence =
        sequence(
                m_groupDisjoint[m_a].testDuration(1, Seconds.of(0.05)),
                m_groupDisjoint[m_b].testDuration(1, Seconds.of(0.05)),
                m_groupDisjoint[m_c].testDuration(1, Seconds.of(0.05)))
            .repeatedly()
            .withTimeout(0.5);

    final Command testDisjointRepeatingSequence =
        disjointSequence(
                m_groupDisjoint[m_a].testDuration(1, Seconds.of(0.05)),
                m_groupDisjoint[m_b].testDuration(1, Seconds.of(0.05)),
                m_groupDisjoint[m_c].testDuration(1, Seconds.of(0.05)))
            .repeatedly()
            .withTimeout(0.5);

    // final Command testDisjointRepeatingSequenceBlocked =
    //   repeatingDisjointSequence(
    //     m_groupDisjoint[m_a].testDuration(1, Seconds.of(0.05)),
    //     m_groupDisjoint[m_b].testDuration(1, Seconds.of(0.05)),
    //     m_groupDisjoint[m_c].testDuration(1, Seconds.of(0.05)))
    //   .withTimeout(0.5);

    // Illegal - Multiple commands in a parallel composition cannot require the same subsystems.
    // Throws an error message.
    // final Command testParallel =
    //   parallel(
    //     m_groupDisjoint[m_a].testDuration(1, Seconds.of(10.)).repeatedly(),
    //     waitSeconds(0.1).andThen(m_groupDisjoint[m_a].testDuration(2, Seconds.of(10.))));

    // Use of Proxy hides the error of having two commands running at once for the same subsystem.
    // No error message but erroneous results.
    // final Command testParallel =
    //   parallel(
    //     m_groupDisjoint[m_a].testDuration(1, Seconds.of(0.2)).asProxy(),
    //     m_groupDisjoint[m_a].testDuration(2, Seconds.of(0.2)).asProxy());

    final Command testParallel =
        parallel(
            sequence(
                m_groupDisjoint[m_b].testDuration(1, Seconds.of(0.74)),
                parallel(
                    m_groupDisjoint[m_a].testDuration(1, Seconds.of(0.84)),
                    m_groupDisjoint[m_b].testDuration(2, Seconds.of(1.)))),
            m_groupDisjoint[m_c].testDuration(1, Seconds.of(0.6)));

    final Command testDisjointParallel =
        disjointParallel(
            disjointSequence(
                m_groupDisjoint[m_b].testDuration(1, Seconds.of(0.74)),
                disjointParallel(
                    m_groupDisjoint[m_a].testDuration(1, Seconds.of(0.84)),
                    m_groupDisjoint[m_b].testDuration(2, Seconds.of(1.)))),
            m_groupDisjoint[m_c].testDuration(1, Seconds.of(0.6)));

    final Command testManualDisjointParallel =
        parallel(
            sequence(
                m_groupDisjoint[m_b].testDuration(1, Seconds.of(0.74)).asProxy(),
                parallel(
                    m_groupDisjoint[m_a].testDuration(1, Seconds.of(0.84)).asProxy(),
                    m_groupDisjoint[m_b].testDuration(2, Seconds.of(1.)).asProxy())),
            m_groupDisjoint[m_c].testDuration(1, Seconds.of(0.6)).asProxy());

    final Command testDeadline =
        deadline(
            sequence(m_groupDisjoint[m_a].testDuration(1, Seconds.of(0.1)), waitSeconds(0.2)),
            sequence(m_groupDisjoint[m_b].testDuration(1, Seconds.of(0.12))),
            sequence(m_groupDisjoint[m_c].testDuration(1, Seconds.of(0.4))));

    final Command testDisjointDeadline =
        disjointDeadline(
            disjointSequence(
                m_groupDisjoint[m_a].testDuration(1, Seconds.of(0.1)), waitSeconds(0.2)),
            disjointSequence(m_groupDisjoint[m_b].testDuration(1, Seconds.of(0.12))),
            disjointSequence(m_groupDisjoint[m_c].testDuration(1, Seconds.of(0.4))));

    final Command testRace =
        race(
            sequence(m_groupDisjoint[m_a].testDuration(1, Seconds.of(0.24))),
            sequence(m_groupDisjoint[m_b].testDuration(1, Seconds.of(0.12)), waitSeconds(0.3)),
            sequence(m_groupDisjoint[m_c].testDuration(1, Seconds.of(0.12)), waitSeconds(0.3)));

    final Command testDisjointRace =
        disjointRace(
            disjointSequence(m_groupDisjoint[m_a].testDuration(1, Seconds.of(0.24))),
            disjointSequence(
                m_groupDisjoint[m_b].testDuration(1, Seconds.of(0.12)), waitSeconds(0.3)),
            disjointSequence(
                m_groupDisjoint[m_c].testDuration(1, Seconds.of(0.12)), waitSeconds(0.3)));

    Command[] allTests = {
      /* runOnce within a subsystem automatically provides the requirement of the subsystem.
       * If this class isn't a subsystem, then it runs the same but without automatic requirements.
       * 
       * There is a conflict of requirements with a command being interrupted by a new command with
       * the same requirements. If requirements are desired on the entire group and included by
       * wrapping a group with such as .beforeStarting then there must not be the same requirements
       * within the group or cancellation occurs.
       * 
       * The Commands version of runOnce allows the requirements to be specified and none must be
       * to avoid the conflict with the group requirements from .beforeStarting.
       * 
       * The effect and duration of a requirement depends on if the group is disjointed or not and
       * how it is disjointed. Disjointed by triggered commands is highly disjointed and the
       * requirements would only be on the first command of the sequence. Disjointed by proxy
       * (disjointSequence method for example) is mildly disjointed and the group still exists with
       * the requirement of this subsystem persisting.
       */
       Commands.runOnce( // This is the first command to be run in sequence and it is disjointed
              // the same as all the rest. Any requirements are within the runOnce only and not
              // applied to the entire sequence. Don't use requirements if the group has
              // requirements or the command will be interrupted.
          () -> {
            // Cleanup default commands from a previous aborted run.
            if (m_groupDisjoint[m_a].getDefaultCommand() != null) {
              CommandScheduler.getInstance().cancel(m_groupDisjoint[m_a].getDefaultCommand());
              m_groupDisjoint[m_a].removeDefaultCommand();         
            }
            if (m_groupDisjoint[m_b].getDefaultCommand() != null) {
              CommandScheduler.getInstance().cancel(m_groupDisjoint[m_b].getDefaultCommand());
              m_groupDisjoint[m_b].removeDefaultCommand();     
            }
            if (m_groupDisjoint[m_c].getDefaultCommand() != null) {
            CommandScheduler.getInstance().cancel(m_groupDisjoint[m_c].getDefaultCommand());
            m_groupDisjoint[m_c].removeDefaultCommand();
            }

            // set default commands
            m_groupDisjoint[m_a].setDefaultCommand();
            m_groupDisjoint[m_b].setDefaultCommand();
            m_groupDisjoint[m_c].setDefaultCommand();
          }
      ),
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
      Commands.runOnce( // This is the last command to be run in sequence and it is disjointed the
              // same as all the rest. Any requirements are within the runOnce only and not applied
              // to the entire sequence. As the last command any requirements might be irrelevant
              // and either version of runOnce may be essentially equivalent. The command is
              // interrupted but after it apparently has completed. Best not to end by interruption.
          () -> {
            // stop default commands to stop the output
            CommandScheduler.getInstance().cancel(m_groupDisjoint[m_a].getDefaultCommand());
            CommandScheduler.getInstance().cancel(m_groupDisjoint[m_b].getDefaultCommand());
            CommandScheduler.getInstance().cancel(m_groupDisjoint[m_c].getDefaultCommand());
            m_groupDisjoint[m_a].removeDefaultCommand();
            m_groupDisjoint[m_b].removeDefaultCommand();
            m_groupDisjoint[m_c].removeDefaultCommand();
          }
      )
    };

    // Triggered disjoint sequence is highly disjointed thus the beforeStarting and finallyDo apply
    // only to the first command in the sequence which likely is unexpected behavior. (Read the
    // javadoc - it says the first command is returned.)
    
    // Proxy disjoint sequence still maintains a group and the beforeStarting and finallyDo apply
    // to the group as a whole which is likely the expected behavior.
    // That means that requirements may be added to the proxy group disjointSequence and is done so
    // below with the beforeStarting runOnce command which is in a wrapper of the entire group.

    // Requirements on the triggered sequence are largely irrelevant since they would only apply to
    // the first command and not the entire sequence. No requirements created in this example.
    if (useTriggeredJob) {
      return TriggeredDisjointSequence.sequence(allTests) // the first command only
                                                        // the rest of the commands triggered later
            .beforeStarting(print("** starting triggered disjoint sequence tests"))
            .finallyDo(interrupted->System.out.println(
                "** the end of triggered disjoint sequence tests interrupted flag = "
                + interrupted)
            );
    } else {
    // Requirements added to the proxy group disjoint sequence are applied to the entire wrapped
    // sequence. Requirements come from the beforeStarting runOnce within this subsystem.
      return disjointSequence(allTests) // group wrapper command represents all the commands
            .beforeStarting(runOnce(()->System.out.println("** starting disjoint sequence tests")))
            .finallyDo(interrupted->System.out.println(
                "** the end of disjoint sequence tests interrupted flag = "
                + interrupted)
            );
    }
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
  public void runBeforeCommands() {
    m_groupDisjoint[m_a].runBeforeCommands();
    m_groupDisjoint[m_b].runBeforeCommands();
    m_groupDisjoint[m_c].runBeforeCommands();
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
  public void runAfterCommands() {
    m_groupDisjoint[m_a].runAfterCommands();
    m_groupDisjoint[m_b].runAfterCommands();
    m_groupDisjoint[m_c].runAfterCommands();
  }

  // The following useful methods might be included in an upcoming WPILib release
  // Use "proxy" with caution!

  /**
   * Runs individual commands in a series without grouped behavior.
   *
   * <p>Each command is run independently by proxy. The requirements of each command are reserved
   * only for the duration of that command and are not reserved for an entire group process as they
   * are in a grouped sequence.
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
   * Runs individual commands in a series without grouped behavior; once the last command ends, the
   * series is restarted.
   *
   * <p>Each command is run independently by proxy. The requirements of each command are reserved
   * only for the duration of that command and are not reserved for an entire group process as they
   * are in a grouped sequence.
   *
   * <p>disjoint...() does not propagate to interior groups. Use additional disjoint...() as needed.
   *
   * @param commands the commands to include in the series
   * @return the command to run the series of commands repeatedly
   * @see #repeatingSequence(Command...) use sequenceRepeatedly() to invoke repeated group sequence
   *     behavior
   * @see #disjointSequence(Command...) use disjointSequence() for no repeating behavior
   */
  public static Command repeatingDisjointSequence(Command... commands) {
    throw new IllegalArgumentException(
        "Not Supported - RepeatCommand bug prevents correct use of Proxy");
    // return disjointSequence(commands).repeatedly();
  }

  /**
   * Runs individual commands at the same time without grouped behavior and ends once all commands
   * finish.
   *
   * <p>Each command is run independently by proxy. The requirements of each command are reserved
   * only for the duration of that command and are not reserved for an entire group process as they
   * are in a grouped parallel.
   *
   * <p>disjoint...() does not propagate to interior groups. Use additional disjoint...() as needed.
   *
   * @param commands the commands to run in parallel
   * @return the command to run the commands in parallel
   * @see #parallel(Command...) use parallel() to invoke group parallel behavior
   */
  public static Command disjointParallel(Command... commands) {
    new ParallelCommandGroup(commands); // check parallel constraints
    for (Command cmd : commands) {
      CommandScheduler.getInstance().removeComposedCommand(cmd);
    }
    return parallel(proxyAll(commands));
  }

  /**
   * Runs a group of commands at the same time. Ends once any command in the group finishes, and
   * cancels the others.
   *
   * <p>disjoint...() does not propagate to interior groups. Use additional disjoint...() as needed.
   *
   * @param commands the commands to include
   * @return the command group
   * @see ParallelRaceGroup
   */
  public static Command disjointRace(Command... commands) {
    new ParallelRaceGroup(commands); // check parallel constraints
    for (Command cmd : commands) {
      CommandScheduler.getInstance().removeComposedCommand(cmd);
    }
    return race(proxyAll(commands));
  }

  /**
   * Runs individual commands at the same time without grouped behavior; when the deadline command
   * ends the otherCommands are cancelled.
   *
   * <p>Each otherCommand is run independently by proxy. The requirements of each command are
   * reserved only for the duration of that command and are not reserved for an entire group process
   * as they are in a grouped deadline.
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
    for (Command cmd : otherCommands) {
      CommandScheduler.getInstance().removeComposedCommand(cmd);
    }
    if (!deadline.getRequirements().isEmpty()) {
      deadline = deadline.asProxy();
    }
    return deadline(deadline, proxyAll(otherCommands));
  }

  /**
   * Maps an array of commands by adding proxy to every element that has requirements using {@link
   * Command#asProxy()}.
   *
   * <p>This is useful to ensure that default commands of subsystems within a command group are
   * still triggered despite command groups requiring the union of their members' requirements
   *
   * @param commands an array of commands
   * @return an array of commands to run by proxy if a command has requirements
   */
  public static Command[] proxyAll(Command... commands) {
    Command[] out = new Command[commands.length];
    for (int i = 0; i < commands.length; i++) {
      if (commands[i].getRequirements().isEmpty()) {
        out[i] = commands[i];
      } else {
        out[i] = commands[i].asProxy();
      }
    }
    return out;
  }
}

/*
[triggered disjoint sequence - default commands running; scrubbed to make pretty:
First command only has the beforeStarting() and finallyDo() decorations. That makes the ending
message a fib. Other test results are essentially identical to those below within one iteration.]
** starting triggered disjoint sequence tests
** the end of triggered disjoint sequence tests interrupted flag = false


[proxy disjoint sequence - default commands running; scrubbed to make pretty:]

** starting disjoint sequence tests

START testSequence
AdBdCdAdBdCd
BdCdA1
BdCdBdCdBdCdBdCd
BdCdA2
END testSequence

BdCdAd

START testDisjointSequence
BdCdAdBdCdAdBd
CdA1BdCdAdBdCdAdBdCdAdBdCdAdBdCdAdBdCdAdBdCdAd
BdCdA2
END testDisjointSequence

BdCdAd

START testRepeatingSequence
BdCdAdBdCdAd
A1A1A1B1B1B1C1C1C1
A1A1A1A1B1B1B1C1C1C1
A1A1A1A1B1B1B1
END testRepeatingSequence

AdBdCd

START testDisjointRepeatingSequence - incorrect results - library bug
AdBdCdAdBdCdBdCd
A1BdCdA1BdCdA1BdCd
AdCd
AdB1CdAdB1CdAdB1Cd
AdBdAdBdC1AdBdC1AdBdC1AdBdCdAdBdCdCdA1B1CdA1B1CdA1B1Cd
AdBdAdBdC1AdBdC1AdBdC1AdBdCdAdBdCdCdA1B1CdA1B1
END testDisjointRepeatingSequence - incorrect results - library bug

CdA1B1

START testParallel
CdAdBdCdAdBd
C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1C1B1
C1B1C1B1C1B1C1B1C1B1C1B1C1B1
B1B1B1B1B1B1B1
A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2
A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2A1B2
B2B2B2B2B2B2B2
END testParallel

AdBdCd

START testDisjointParallel
AdBdCdAdBdCd
AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1
AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1
AdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdB1Cd
AdCdBdCd
A1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2Cd
A1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2Cd
A1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2CdA1B2Cd
B2AdCdB2AdCdB2AdCdB2AdCdB2AdCdB2AdCdB2AdCdB2Ad
END testDisjointParallel

CdAdBd

START testManualDisjointParallel
CdAdBdCdAdBd
AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1
AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1AdC1B1
AdB1CdAdB1CdAdB1CdAdB1CdAdB1CdAdB1Cd
AdCdBd
CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1
CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1
CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1CdB2A1
CdB2AdCdB2AdCdB2AdCdB2AdCdB2AdCdB2AdCdB2AdCdB2Ad
END testManualDisjointParallel

CdAdBd

START testDeadlineParallel
CdAdBdCdAdBd
A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1
B1C1C1C1C1C1C1C1C1C1C1C1
END testDeadlineParallel

AdBdCd

START testDisjointDeadlineParallel
AdBdCdAdBdCd
B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1
B1C1AdB1C1Ad
C1AdBdC1AdBdC1AdBdC1AdBdC1AdBdC1AdBdC1AdBdC1AdBd
END testDisjointDeadlineParallel

C1AdBd

START testRaceParallel
AdBdCdAdBdCd
C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1
C1A1A1A1A1A1A1A1
END testRaceParallel

AdBdCd

START testDisjointRaceParallel
AdBdCdAdBdCd
C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1C1A1B1
A1BdCdA1BdCdA1BdCdA1BdCdA1BdCdA1BdCd
END testDisjointRaceParallel

BdCdAdBdCdAd

** the end of disjoint sequence tests interrupted flag = false
 */