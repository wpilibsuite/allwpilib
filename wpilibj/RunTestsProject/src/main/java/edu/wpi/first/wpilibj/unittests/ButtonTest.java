/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.wpilibj.unittests;

import edu.wpi.first.testing.TestClass;
import edu.wpi.first.wpilibj.buttons.InternalButton;
import edu.wpi.first.wpilibj.command.Scheduler;

/**
 *
 * @author Mitchell
 */
public class ButtonTest extends TestClass{

    public String getName() {
        return "Button Test";
    }

    public String[] getTags() {
        return new String[] { RunTests.Tags.Lifecycle.INPRODUCTION,
                              RunTests.Tags.Type.COMMANDBASED };
    }
    
    private InternalButton button1;
    private InternalButton button2;
    public void setup() {
        button1 = new InternalButton();
        button2 = new InternalButton();
    }

    public void teardown() {
    }
    
    
    {
        new Test("Simple Button Test") {

            public void run() {
                TestCommand command1 = new TestCommand();
                TestCommand command2 = new TestCommand();
                TestCommand command3 = new TestCommand();
                TestCommand command4 = new TestCommand();
                
                button1.whenPressed(command1);
                button1.whenReleased(command2);
                button1.whileHeld(command3);
                button2.whileHeld(command4);
                
                assertCommandState(command1, 0, 0, 0, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                assertCommandState(command3, 0, 0, 0, 0, 0);
                assertCommandState(command4, 0, 0, 0, 0, 0);
                button1.setPressed(true);
                assertCommandState(command1, 0, 0, 0, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                assertCommandState(command3, 0, 0, 0, 0, 0);
                assertCommandState(command4, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command1, 0, 0, 0, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                assertCommandState(command3, 0, 0, 0, 0, 0);
                assertCommandState(command4, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 1, 1, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                assertCommandState(command3, 1, 1, 1, 0, 0);
                assertCommandState(command4, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 2, 2, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                assertCommandState(command3, 1, 2, 2, 0, 0);
                assertCommandState(command4, 0, 0, 0, 0, 0);
                button2.setPressed(true);
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 3, 3, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                assertCommandState(command3, 1, 3, 3, 0, 0);
                assertCommandState(command4, 0, 0, 0, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 4, 4, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                assertCommandState(command3, 1, 4, 4, 0, 0);
                assertCommandState(command4, 1, 1, 1, 0, 0);
                button1.setPressed(false);
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 5, 5, 0, 0);
                assertCommandState(command2, 0, 0, 0, 0, 0);
                assertCommandState(command3, 1, 4, 4, 0, 1);
                assertCommandState(command4, 1, 2, 2, 0, 0);
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 6, 6, 0, 0);
                assertCommandState(command2, 1, 1, 1, 0, 0);
                assertCommandState(command3, 1, 4, 4, 0, 1);
                assertCommandState(command4, 1, 3, 3, 0, 0);
                button2.setPressed(false);
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 7, 7, 0, 0);
                assertCommandState(command2, 1, 2, 2, 0, 0);
                assertCommandState(command3, 1, 4, 4, 0, 1);
                assertCommandState(command4, 1, 3, 3, 0, 1);
                command1.cancel();
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 7, 7, 0, 1);
                assertCommandState(command2, 1, 3, 3, 0, 0);
                assertCommandState(command3, 1, 4, 4, 0, 1);
                assertCommandState(command4, 1, 3, 3, 0, 1);
                command2.setHasFinished(true);
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 7, 7, 0, 1);
                assertCommandState(command2, 1, 4, 4, 1, 0);
                assertCommandState(command3, 1, 4, 4, 0, 1);
                assertCommandState(command4, 1, 3, 3, 0, 1);
                Scheduler.getInstance().run();
                assertCommandState(command1, 1, 7, 7, 0, 1);
                assertCommandState(command2, 1, 4, 4, 1, 0);
                assertCommandState(command3, 1, 4, 4, 0, 1);
                assertCommandState(command4, 1, 3, 3, 0, 1);
                
                
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
