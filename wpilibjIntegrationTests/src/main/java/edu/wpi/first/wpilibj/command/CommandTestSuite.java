/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;
import org.junit.runners.Suite.SuiteClasses;

import edu.wpi.first.wpilibj.test.AbstractTestSuite;

/**
 * Contains a listing of all of the {@link Command} tests.
 * @author jonathanleitschuh
 */
@RunWith(Suite.class)
@SuiteClasses({ButtonTest.class, CommandParallelGroupTest.class, CommandScheduleTest.class,
    CommandSequentialGroupTest.class, CommandSupersedeTest.class, CommandTimeoutTest.class,
    DefaultCommandTest.class})
public class CommandTestSuite extends AbstractTestSuite {

}
