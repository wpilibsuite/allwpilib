/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.wpilibj.unittests;

import edu.wpi.first.testing.TestClass;
import edu.wpi.first.wpilibj.command.Scheduler;

/**
 *
 * @author mwills
 */
public class CommandScheduleTest extends TestClass{
    public String getName() {
        return "Command Schedule Test";
    }

    public String[] getTags() {
        return new String[] { RunTests.Tags.Lifecycle.INPRODUCTION,
                              RunTests.Tags.Type.COMMANDBASED };
    }
    
    public void setup() {
    }

    public void teardown() {
    }
    
    {
        new Test("Simple scheduling of a command and making sure the command is run and successfully terminates") {
            public void run() {
                TestCommand command = new TestCommand();
                command.start();
                assertCommandState(command, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command, 1, 1, 1, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command, 1, 2, 2, 0, 0);
                command.setHasFinished(true);
                assertCommandState(command, 1, 2, 2, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command, 1, 3, 3, 1, 0);
                Scheduler.getInstance().run();
                assertCommandState(command, 1, 3, 3, 1, 0);
            }
            
        };
    }
    {
        new Test("Simple scheduling of a command and making sure the command is run and cancels correctly") {
            public void run() {
                TestCommand command = new TestCommand();
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
                command.cancel();
                assertCommandState(command, 1, 3, 3, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command, 1, 3, 3, 0, 1);
                Scheduler.getInstance().run();
                assertCommandState(command, 1, 3, 3, 0, 1);
            }
            
        };
    }
    
    
    public void assertCommandState(TestCommand command, int initialize, int execute, int isFinished, int end, int interrupted){
                assertEquals(initialize, command.getInitializeCount());
                assertEquals(execute, command.getExecuteCount());
                assertEquals(isFinished, command.getIsFinishedCount());
                assertEquals(end, command.getEndCount());
                assertEquals(interrupted, command.getInterruptedCount());
    }
    
}
