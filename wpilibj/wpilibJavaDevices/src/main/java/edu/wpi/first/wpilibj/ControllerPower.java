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
	/**
	* Get the input voltage to the robot controller
	* @return The controller input voltage value in Volts
	*/
	public static double getInputVoltage()
	{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		double retVal = PowerJNI.getVinVoltage(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return retVal;
	}

	/**
	* Get the input current to the robot controller
	* @return The controller input current value in Amps
	*/
	public static double getInputCurrent()
	{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		double retVal = PowerJNI.getVinCurrent(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return retVal;
	}
	
	/**
	* Get the voltage of the 3.3V rail
	* @return The controller 3.3V rail voltage value in Volts
	*/
	public static double getVoltage3V3()
	{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		double retVal = PowerJNI.getUserVoltage3V3(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return retVal;
	}
	
	/**
	* Get the current output of the 3.3V rail
	* @return The controller 3.3V rail output current value in Volts
	*/
	public static double getCurrent3V3()
	{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		double retVal = PowerJNI.getUserCurrent3V3(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return retVal;
	}
	
	/**
	* Get the enabled state of the 3.3V rail. The rail may be disabled due to a controller
	* brownout, a short circuit on the rail, or controller over-voltage
	* @return The controller 3.3V rail enabled value
	*/
	public static boolean getEnabled3V3()
	{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		boolean retVal = PowerJNI.getUserActive3V3(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return retVal;
	}
	
	/**
	* Get the count of the total current faults on the 3.3V rail since the controller has booted
	* @return The number of faults
	*/
	public static int getFaultCount3V3()
	{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		int retVal = PowerJNI.getUserCurrentFaults3V3(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return retVal;
	}
	
	/**
	* Get the voltage of the 5V rail
	* @return The controller 5V rail voltage value in Volts
	*/
	public static double getVoltage5V()
	{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		double retVal = PowerJNI.getUserVoltage5V(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return retVal;
	}
	
	/**
	* Get the current output of the 5V rail
	* @return The controller 5V rail output current value in Amps
	*/
	public static double getCurrent5V()
	{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		double retVal = PowerJNI.getUserCurrent5V(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return retVal;
	}
	
	/**
	* Get the enabled state of the 5V rail. The rail may be disabled due to a controller
	* brownout, a short circuit on the rail, or controller over-voltage
	* @return The controller 5V rail enabled value
	*/
	public static boolean getEnabled5V()
	{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		boolean retVal = PowerJNI.getUserActive5V(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return retVal;
	}	
	
	/**
	* Get the count of the total current faults on the 5V rail since the controller has booted
	* @return The number of faults
	*/
	public static int getFaultCount5V()
	{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		int retVal = PowerJNI.getUserCurrentFaults5V(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return retVal;
	}
	
	/**
	* Get the voltage of the 6V rail
	* @return The controller 6V rail voltage value in Volts
	*/
	public static double getVoltage6V()
	{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		double retVal = PowerJNI.getUserVoltage6V(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return retVal;
	}
	
	/**
	* Get the current output of the 6V rail
	* @return The controller 6V rail output current value in Amps
	*/
	public static double getCurrent6V()
	{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		double retVal = PowerJNI.getUserCurrent6V(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return retVal;
	}
	
	/**
	* Get the enabled state of the 6V rail. The rail may be disabled due to a controller
	* brownout, a short circuit on the rail, or controller over-voltage
	* @return The controller 6V rail enabled value
	*/	
	public static boolean getEnabled6V()
	{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		boolean retVal = PowerJNI.getUserActive6V(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return retVal;
	}

	/**
	* Get the count of the total current faults on the 6V rail since the controller has booted
	* @return The number of faults
	*/
	public static int getFaultCount6V()
	{
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		int retVal = PowerJNI.getUserCurrentFaults6V(status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		return retVal;
	}
}