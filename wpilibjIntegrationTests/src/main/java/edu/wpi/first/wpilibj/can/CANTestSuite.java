/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.can;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;
import org.junit.runners.Suite.SuiteClasses;

import edu.wpi.first.wpilibj.test.AbstractTestSuite;

/**
 * All of the tests to cover the CAN Motor Controllers.
 *
 * @author jonathanleitschuh
 */
@RunWith(Suite.class)
@SuiteClasses({CANCurrentQuadEncoderModeTest.class, CANDefaultTest.class,
    CANJaguarInversionTest.class, CANPercentQuadEncoderModeTest.class,
    CANPositionPotentiometerModeTest.class, CANPositionQuadEncoderModeTest.class,
    CANSpeedQuadEncoderModeTest.class, CANVoltageQuadEncoderModeTest.class})
public class CANTestSuite extends AbstractTestSuite {

}
