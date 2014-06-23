/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.command;

import java.util.logging.Logger;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Ignore;
import org.junit.Test;

import edu.wpi.first.wpilibj.mocks.MockCommand;

/**
 * Ported from the old CrioTest Classes
 * @author Mitchell
 * @author Jonathan Leitschuh
 */
public class CommandSequentialGroupTest extends AbstractCommandTest {
	private static final Logger logger = Logger.getLogger(CommandSequentialGroupTest.class.getName());
	
	protected Logger getClassLogger(){
		return logger;
	}
	
	/**
	 * @throws java.lang.Exception
	 */
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
	}

	/**
	 * @throws java.lang.Exception
	 */
	@AfterClass
	public static void tearDownAfterClass() throws Exception {
	}

	/**
	 * @throws java.lang.Exception
	 */
	@Before
	public void setUp() throws Exception {
	}

	/**
	 * @throws java.lang.Exception
	 */
	@After
	public void tearDown() throws Exception {
	}

	/**
	 * Simple Command Group With 3 commands that all depend on a subsystem. Some commands have a timeout
	 */
	@Test(timeout = 20000)
	public void testThreeCommandOnSubSystem() {
		logger.fine("Begining Test");
		final ASubsystem subsystem = new ASubsystem();
		
		logger.finest("Creating Mock Command1");
        MockCommand command1 = new MockCommand() {
            {
                requires(subsystem);
            }
       };
       logger.finest("Creating Mock Command2");
       MockCommand command2 = new MockCommand() {
            {
                requires(subsystem);
            }
        };
        logger.finest("Creating Mock Command3");
        MockCommand command3 = new MockCommand() {
            {
                requires(subsystem);
            }
        };
        
        logger.finest("Creating Command Group");
        CommandGroup commandGroup = new CommandGroup();
        commandGroup.addSequential(command1, 1.0);
        commandGroup.addSequential(command2, 2.0);
        commandGroup.addSequential(command3);
        
        
        assertCommandState(command1, 0, 0, 0, 0, 0);
        assertCommandState(command2, 0, 0, 0, 0, 0);
        assertCommandState(command3, 0, 0, 0, 0, 0);
        logger.finest("Starting Command group");
        commandGroup.start();
        assertCommandState(command1, 0, 0, 0, 0, 0);
        assertCommandState(command2, 0, 0, 0, 0, 0);
        assertCommandState(command3, 0, 0, 0, 0, 0);
        Scheduler.getInstance().run();
        assertCommandState(command1, 0, 0, 0, 0, 0);
        assertCommandState(command2, 0, 0, 0, 0, 0);
        assertCommandState(command3, 0, 0, 0, 0, 0);
        Scheduler.getInstance().run();
        assertCommandState(command1, 1, 1, 1, 0, 0);
        assertCommandState(command2, 0, 0, 0, 0, 0);
        assertCommandState(command3, 0, 0, 0, 0, 0);
        sleep(1000);//command 1 timeout
        Scheduler.getInstance().run();
        assertCommandState(command1, 1, 1, 1, 0, 1);
        assertCommandState(command2, 1, 1, 1, 0, 0);
        assertCommandState(command3, 0, 0, 0, 0, 0);
        
        Scheduler.getInstance().run();
        assertCommandState(command1, 1, 1, 1, 0, 1);
        assertCommandState(command2, 1, 2, 2, 0, 0);
        assertCommandState(command3, 0, 0, 0, 0, 0);
        sleep(2000);//command 2 timeout
        Scheduler.getInstance().run();
        assertCommandState(command1, 1, 1, 1, 0, 1);
        assertCommandState(command2, 1, 2, 2, 0, 1);
        assertCommandState(command3, 1, 1 ,1, 0, 0);
        
        Scheduler.getInstance().run();
        assertCommandState(command1, 1, 1, 1, 0, 1);
        assertCommandState(command2, 1, 2, 2, 0, 1);
        assertCommandState(command3, 1, 2, 2, 0, 0);
        command3.setHasFinished(true);
        assertCommandState(command1, 1, 1, 1, 0, 1);
        assertCommandState(command2, 1, 2, 2, 0, 1);
        assertCommandState(command3, 1, 2, 2, 0, 0);
        Scheduler.getInstance().run();
        assertCommandState(command1, 1, 1, 1, 0, 1);
        assertCommandState(command2, 1, 2, 2, 0, 1);
        assertCommandState(command3, 1, 3, 3, 1, 0);
        Scheduler.getInstance().run();
        assertCommandState(command1, 1, 1, 1, 0, 1);
        assertCommandState(command2, 1, 2, 2, 0, 1);
        assertCommandState(command3, 1, 3, 3, 1, 0);
	}

}
