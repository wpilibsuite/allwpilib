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
import edu.wpi.first.wpilibj.test.AbstractComsSetup;

/**
 * Ported from the old CrioTest Classes
 * @author Mitchell
 * @author Jonathan Leitschuh
 */
public class CommandParallelGroupTest extends AbstractCommandTest {

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

	@Test
	public void test() {
		MockCommand command1 = new MockCommand();
		MockCommand command2 = new MockCommand();

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

	public void sleep(long time) {
		try {
			Thread.sleep(time);
		} catch (InterruptedException ex) {
			fail("Sleep Interrupted!?!?!?!?");
		}
	}
}
