/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.command;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.fail;

import org.junit.Before;

import edu.wpi.first.wpilibj.mocks.MockCommand;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;

/**
 * @author jonathanleitschuh
 *
 */
public abstract class AbstractCommandTest extends AbstractComsSetup {
	
	@Before
	public void commandSetup(){
		Scheduler.getInstance().removeAll();
		Scheduler.getInstance().enable();
	}
	
	public class ASubsystem extends Subsystem {
		Command command;
        protected void initDefaultCommand(){
        	if(command != null){
        		setDefaultCommand(command);
        	}
        }
        
        public void init(Command command) {
            this.command = command;
        }
    }
	
	public void assertCommandState(MockCommand command, int initialize, int execute, int isFinished, int end, int interrupted){
        assertEquals(initialize, command.getInitializeCount());
        assertEquals(execute, command.getExecuteCount());
        assertEquals(isFinished, command.getIsFinishedCount());
        assertEquals(end, command.getEndCount());
        assertEquals(interrupted, command.getInterruptedCount());
	 }
	
	public void sleep(int time){
		try {
            Thread.sleep(time);
        } catch (InterruptedException ex) {
            fail("Sleep Interrupted!?!?!?!?");
        }
	}
}
