/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.wpilibj.unittests;

import edu.wpi.first.testing.TestClass;
import edu.wpi.first.wpilibj.command.Scheduler;
import edu.wpi.first.wpilibj.command.Subsystem;

/**
 *
 * @author Mitchell
 */
public class CommandSupercedeTest extends TestClass{

    public String getName() {
        return "Command Supercede Test";
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
        protected void initDefaultCommand(){
        }
    }

    {
        new Test("Testing one command superceeding another because of dependancies") {

            public void run() {
                final ASubsystem subsystem = new ASubsystem();


                TestCommand command1 = new TestCommand() {
                    {
                        requires(subsystem);
                    }
                };
                
                TestCommand command2 = new TestCommand() {
                    {
                        requires(subsystem);
                    }
                };
                
                
                assertCommandState(command1, 0, 0, 0, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                command1.start();
                assertCommandState(command1, 0, 0, 0, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command1, 0, 0, 0, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 1, 1, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 2, 2, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 3, 3, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                command2.start();
                assertCommandState(command1, 1, 3, 3, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 4, 4, 0, 1);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 4, 4, 0, 1);
                assertCommandState(command2, 1, 1, 1 , 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 4, 4, 0, 1);
                assertCommandState(command2, 1, 2, 2, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 4, 4, 0, 1);
                assertCommandState(command2, 1, 3, 3, 0, 0);
                
            }
        };
    }
    {
        new Test("Testing one command failing superceeding another because of dependancies because the first command cannot be interrupted") {

            public void run() {
                final ASubsystem subsystem = new ASubsystem();


                TestCommand command1 = new TestCommand() {
                    {
                        requires(subsystem);
                        setInterruptible(false);
                    }
                };
                
                TestCommand command2 = new TestCommand() {
                    {
                        requires(subsystem);
                    }
                };
                
                
                assertCommandState(command1, 0, 0, 0, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                command1.start();
                assertCommandState(command1, 0, 0, 0, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command1, 0, 0, 0, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 1, 1, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 2, 2, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 3, 3, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                command2.start();
                assertCommandState(command1, 1, 3, 3, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 4, 4, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                
            }
        };
    }

    private int testCount = 1;
    public void assertCommandState(TestCommand command, int initialize, int execute, int isFinished, int end, int interrupted) {
        //System.out.println("Test Command Initialize "+(testCount++));
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
