/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.wpilibj.unittests;

import edu.wpi.first.testing.TestClass;
import edu.wpi.first.testing.TestClass.Test;
import edu.wpi.first.wpilibj.command.Command;
import edu.wpi.first.wpilibj.command.CommandGroup;
import edu.wpi.first.wpilibj.command.Scheduler;
import edu.wpi.first.wpilibj.command.Subsystem;

/**
 *
 * @author Mitchell
 */
public class CommandTimeoutTest extends TestClass{

    public String getName() {
        return "Command Timeout Test";
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
        new Test("Command 2 second Timeout Test") {

            public void run() {
                final ASubsystem subsystem = new ASubsystem();


                TestCommand command = new TestCommand() {
                    {
                        requires(subsystem);
                        setTimeout(2);
                    }
                    public boolean isFinished(){
                        return super.isFinished() || isTimedOut();
                    }
                };
                
                command.start();
                assertCommandState(command, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command, 1, 1, 1, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command, 1, 2, 2, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command, 1, 3, 3, 0, 0);
                sleep(2000);
                Scheduler.getInstance().run();
                assertCommandState(command, 1, 4, 4, 1, 0);
                Scheduler.getInstance().run();
                assertCommandState(command, 1, 4, 4, 1, 0);
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
