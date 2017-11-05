/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * IterativeBase provides methods that are to be used when code is designed to be aware of
 * robot state. These are provided as 'init' functions and 'periodic' functions.
 *
 * <p>init() functions -- each of the following functions is called once when the
 * appropriate mode is entered:
 *   - disabledInit()   -- called only when first disabled
 *   - autonomousInit() -- called each and every time autonomous is entered from
 *                         another mode
 *   - teleopInit()     -- called each and every time teleop is entered from
 *                         another mode
 *   - testInit()       -- called each and every time test is entered from
 *                         another mode
 *
 * <p>periodic() functions -- each of these functions is called on an interval:
 *   - robotPeriodic()
 *   - disabledPeriodic()
 *   - autonomousPeriodic()
 *   - teleopPeriodic()
 *   - testPeriodic()
 */
public interface IterativeBase {

    /**
     * Initialization code for disabled mode should go here.
     */
    public void disabledInit();

    /**
     * Initialization code for autonomous mode should go here.
     */
    public void autonomousInit();

    /**
     * Initialization code for teleop mode should go here.
     */
    public void teleopInit();

    /**
     * Initialization code for test mode should go here.
     */
    public void testInit();

    /**
     * Periodic code for all robot modes should go here.
     */
    public void robotPeriodic();

    /**
     * Periodic code for disabled mode should go here.
     */
    public void disabledPeriodic();

    /**
     * Periodic code for autonomous mode should go here.
     */
    public void autonomousPeriodic();

    /**
     * Periodic code for teleop mode should go here.
     */
    public void teleopPeriodic();

    /**
     * Periodic code for test mode should go here.
     */
    public void testPeriodic();
}
