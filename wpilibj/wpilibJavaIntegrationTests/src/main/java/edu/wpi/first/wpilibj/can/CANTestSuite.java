/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.can;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;
import org.junit.runners.Suite.SuiteClasses;

/**
 * @author jonathanleitschuh
 *
 */
@RunWith(Suite.class)
@SuiteClasses({ CurrentQuadEncoderModeTest.class,
				PercentQuadEncoderModeTest.class,
				PositionQuadEncoderModeTest.class,
				SpeedQuadEncoderModeTest.class,
				VoltageQuadEncoderModeTest.class
			})
public class CANTestSuite {

}
