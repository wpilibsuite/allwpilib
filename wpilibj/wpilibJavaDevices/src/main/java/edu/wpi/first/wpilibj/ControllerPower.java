/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.IntBuffer;

import edu.wpi.first.wpilibj.hal.HALUtil;
import edu.wpi.first.wpilibj.hal.PowerJNI;

public class ControllerPower
{
	public static double getInputVoltage()
	{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		double retVal = PowerJNI.getVinVoltage(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return retVal;
	}
	
	public static double getInputCurrent()
	{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		double retVal = PowerJNI.getVinCurrent(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return retVal;
	}
	
	public static double getVoltage3V3()
	{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		double retVal = PowerJNI.getUserVoltage3V3(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return retVal;
	}
	
	public static double getCurrent3V3()
	{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		double retVal = PowerJNI.getUserCurrent3V3(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return retVal;
	}
	
	public static boolean getEnabled3V3()
	{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		boolean retVal = PowerJNI.getUserActive3V3(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return retVal;
	}
	
	public static int getFaultCount3V3()
	{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		int retVal = PowerJNI.getUserCurrentFaults3V3(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return retVal;
	}
	
	public static double getVoltage5V()
	{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		double retVal = PowerJNI.getUserVoltage5V(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return retVal;
	}
	
	public static double getCurrent5V()
	{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		double retVal = PowerJNI.getUserCurrent5V(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return retVal;
	}
	
	public static boolean getEnabled5V()
	{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		boolean retVal = PowerJNI.getUserActive5V(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return retVal;
	}	
	
	public static int getFaultCount5V()
	{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		int retVal = PowerJNI.getUserCurrentFaults5V(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return retVal;
	}
	
	public static double getVoltage6V()
	{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		double retVal = PowerJNI.getUserVoltage6V(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return retVal;
	}
	
	public static double getCurrent6V()
	{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		double retVal = PowerJNI.getUserCurrent6V(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return retVal;
	}
	
	public static boolean getEnabled6V()
	{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		boolean retVal = PowerJNI.getUserActive6V(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return retVal;
	}
	
	public static int getFaultCount6V()
	{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		int retVal = PowerJNI.getUserCurrentFaults6V(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return retVal;
	}
}