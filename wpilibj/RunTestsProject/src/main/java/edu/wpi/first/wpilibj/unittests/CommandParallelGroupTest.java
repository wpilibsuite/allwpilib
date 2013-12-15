/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.wpilibj.unittests;

import edu.wpi.first.testing.TestClass;
import edu.wpi.first.wpilibj.command.Command;
import edu.wpi.first.wpilibj.command.CommandGroup;
import edu.wpi.first.wpilibj.command.Scheduler;
import edu.wpi.first.wpilibj.command.Subsystem;

/**
 *
 * @author Mitchell
 */
public class CommandParallelGroupTest extends TestClass{

    public String getName() {
        return "Command Parallel Group Test";
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
        new Test("Simple Parallel Command Group With 2 commands one command terminates first") {

            public void run() {
                TestCommand command1 = new TestCommand();
                TestCommand command2 = new TestCommand();
                
                CommandGroup commandGroup = new CommandGroup();
                commandGroup.addParallel(command1);
                commandGroup.addParallel(command2);
                
                
                assertCommandState(command1, 0, 0, 0, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                commandGroup.start();
                assertCommandState(command1, 0, 0, 0, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command1, 0, 0, 0, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 1, 1, 0, 0);
                assertCommandState(command2, 1, 1, 1, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 2, 2, 0, 0);
                assertCommandState(command2, 1, 2, 2, 0, 0);
                command1.setHasFinished(true);
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 3, 3, 1, 0);
                assertCommandState(command2, 1, 3, 3, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 3, 3, 1, 0);
                assertCommandState(command2, 1, 4, 4, 0, 0);
                command2.setHasFinished(true);
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 3, 3, 1, 0);
                assertCommandState(command2, 1, 5, 5, 1, 0);
                
            }
        };
    }
    
    {
        new Test("Simple Parallel Command Group With 2 commands. The group is canceled") {

            public void run() {
                TestCommand command1 = new TestCommand();
                TestCommand command2 = new TestCommand();
                
                CommandGroup commandGroup = new CommandGroup();
                commandGroup.addParallel(command1);
                commandGroup.addParallel(command2);
                
                
                assertCommandState(command1, 0, 0, 0, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                commandGroup.start();
                assertCommandState(command1, 0, 0, 0, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command1, 0, 0, 0, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 1, 1, 0, 0);
                assertCommandState(command2, 1, 1, 1, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 2, 2, 0, 0);
                assertCommandState(command2, 1, 2, 2, 0, 0);
                commandGroup.cancel();
                assertCommandState(command1, 1, 2, 2, 0, 0);
                assertCommandState(command2, 1, 2, 2, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 2, 2, 0, 1);
                assertCommandState(command2, 1, 2, 2, 0, 1);
                
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
    
    public void sleep(long time){
        try {
            Thread.sleep(time);
        } catch (InterruptedException ex) {
            assertFail("Sleep Interrupted!?!?!?!?");
        }
    }
    
}
