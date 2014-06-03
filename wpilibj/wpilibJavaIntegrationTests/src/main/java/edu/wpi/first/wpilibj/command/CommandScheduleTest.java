/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.command;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import edu.wpi.first.wpilibj.mocks.MockCommand;

/**
 * @author jonathanleitschuh, mwills
 *
 */
public class CommandScheduleTest extends AbstractCommandTest {

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
	 * Simple scheduling of a command and making sure the command is run and successfully terminates
	 */
	@Test
	public void testRunAndTerminate() {
		MockCommand command = new MockCommand();
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
	
	/**
	 * Simple scheduling of a command and making sure the command is run and cancels correctly
	 */
	@Test
	public void testRunAndCancel(){
		 MockCommand command = new MockCommand();
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
}
