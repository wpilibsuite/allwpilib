package edu.wpi.first.wpilibj.unittests;

import edu.wpi.first.testing.TestClass;
import edu.wpi.first.testing.TestClass.Test;
import edu.wpi.first.wpilibj.command.Command;
import edu.wpi.first.wpilibj.command.Scheduler;
import edu.wpi.first.wpilibj.command.Subsystem;

/**
 *
 * @author mwills
 */
public class DefaultCommandsTest extends TestClass {

    public String getName() {
        return "Default Commands Test";
    }

    public String[] getTags() {
        return new String[] { RunTests.Tags.Lifecycle.INPRODUCTION,
                              RunTests.Tags.Type.COMMANDBASED };
    }

    public void setup() {
    }

    public void teardown() {
    }

    private class ASubsystem extends Subsystem {
        Command command;
        protected void initDefaultCommand(){
            setDefaultCommand(command);
        }
        public void init(Command command) {
            this.command = command;
        }
    }

    {
        new Test("Testing of default commands where the interrupting command is ends itself") {

            public void run() {
                final ASubsystem subsystem = new ASubsystem();


                TestCommand defaultCommand = new TestCommand() {
                    {
                        requires(subsystem);
                    }
                };
                
                TestCommand anotherCommand = new TestCommand() {
                    {
                        requires(subsystem);
                    }
                };
                assertCommandState(defaultCommand, 0, 0, 0, 0, 0);
                subsystem.init(defaultCommand);
                
                assertCommandState(defaultCommand, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(defaultCommand, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(defaultCommand, 1, 1, 1, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(defaultCommand, 1, 2, 2, 0, 0);
                
                anotherCommand.start();
                assertCommandState(defaultCommand, 1, 2, 2, 0, 0);
                assertCommandState(anotherCommand, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
                assertCommandState(anotherCommand, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
                assertCommandState(anotherCommand, 1, 1, 1, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
                assertCommandState(anotherCommand, 1, 2, 2, 0, 0);
                anotherCommand.setHasFinished(true);
                assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
                assertCommandState(anotherCommand, 1, 2, 2, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
                assertCommandState(anotherCommand, 1, 3, 3, 1, 0);
                Scheduler.getInstance().run();
                assertCommandState(defaultCommand, 2, 4, 4, 0, 1);
                assertCommandState(anotherCommand, 1, 3, 3, 1, 0);
                Scheduler.getInstance().run();
                assertCommandState(defaultCommand, 2, 5, 5, 0, 1);
                assertCommandState(anotherCommand, 1, 3, 3, 1, 0);
                
            }
        };
    }

    {
        new Test("Testing of default commands where the interrupting command is canceled") {

            public void run() {
                final ASubsystem subsystem = new ASubsystem();


                TestCommand defaultCommand = new TestCommand() {
                    {
                        requires(subsystem);
                    }
                };
                
                TestCommand anotherCommand = new TestCommand() {
                    {
                        requires(subsystem);
                    }
                };
                assertCommandState(defaultCommand, 0, 0, 0, 0, 0);
                subsystem.init(defaultCommand);
                subsystem.initDefaultCommand();
                assertCommandState(defaultCommand, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(defaultCommand, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(defaultCommand, 1, 1, 1, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(defaultCommand, 1, 2, 2, 0, 0);
                
                anotherCommand.start();
                assertCommandState(defaultCommand, 1, 2, 2, 0, 0);
                assertCommandState(anotherCommand, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
                assertCommandState(anotherCommand, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
                assertCommandState(anotherCommand, 1, 1, 1, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
                assertCommandState(anotherCommand, 1, 2, 2, 0, 0);
                anotherCommand.cancel();
                assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
                assertCommandState(anotherCommand, 1, 2, 2, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(defaultCommand, 1, 3, 3, 0, 1);
                assertCommandState(anotherCommand, 1, 2, 2, 0, 1);
                Scheduler.getInstance().run();
                assertCommandState(defaultCommand, 2, 4, 4, 0, 1);
                assertCommandState(anotherCommand, 1, 2, 2, 0, 1);
                Scheduler.getInstance().run();
                assertCommandState(defaultCommand, 2, 5, 5, 0, 1);
                assertCommandState(anotherCommand, 1, 2, 2, 0, 1);
                
            }
        };
    }

    public void assertCommandState(TestCommand command, int initialize, int execute, int isFinished, int end, int interrupted) {
        //System.out.println("Test Command Initialize");
        assertEquals(initialize, command.getInitializeCount());
        //System.out.println("Test Command Execute");
        assertEquals(execute, command.getExecuteCount());
        //System.out.println("Test Command IsFinished");
        assertEquals(isFinished, command.getIsFinishedCount());
        //System.out.println("Test Command End");
        assertEquals(end, command.getEndCount());
        //System.out.println("Test Command Interrupted");
        assertEquals(interrupted, command.getInterruptedCount());
    }
}
