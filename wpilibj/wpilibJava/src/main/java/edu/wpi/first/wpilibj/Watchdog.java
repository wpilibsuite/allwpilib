/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import java.util.logging.*;
import java.nio.ByteOrder;
import java.nio.IntBuffer;
import java.nio.ByteBuffer;

//import com.sun.jna.Pointer;


import edu.wpi.first.wpilibj.hal.WatchdogJNI;
import edu.wpi.first.wpilibj.hal.HALUtil;
import edu.wpi.first.wpilibj.parsing.IUtility;

/**
 * Watchdog timer class. The watchdog timer is designed to keep the robots safe.
 * The idea is that the robot program must constantly "feed" the watchdog
 * otherwise it will shut down all the motor outputs. That way if a program
 * breaks, rather than having the robot continue to operate at the last known
 * speed, the motors will be shut down.
 * 
 * This is serious business. Don't just disable the watchdog. You can't afford
 * it!
 * 
 * http://thedailywtf.com/Articles/_0x2f__0x2f_TODO_0x3a__Uncomment_Later.aspx
 */
public class Watchdog extends SensorBase implements IUtility {
	private static Logger logger = Logger.getLogger("Watchdog");
	private static ConsoleHandler ch = new ConsoleHandler();
	private static Watchdog m_instance;
	private ByteBuffer m_watchDog;
	
	static
	{
		logger.addHandler(ch);
    	logger.setLevel(Level.ALL);
	}

	/**
	 * The Watchdog is born.
	 */
	protected Watchdog() {
		// allocate direct
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
		m_watchDog = WatchdogJNI.initializeWatchdog(status.asIntBuffer());
		// set the byte order for the return value
		m_watchDog.order(ByteOrder.LITTLE_ENDIAN);
	 	logger.info("Initialize Watchdog Status = " + status.getInt(0) );
	 	logger.info("Watchdog Handle Length = " + m_watchDog.capacity() );
	 	logger.info("Watchdog Handle Value:" + m_watchDog.getInt(0));
	 	HALUtil.checkStatus(status.asIntBuffer());
	}

	/**
	 * Get an instance of the watchdog
	 * 
	 * @return an instance of the watchdog
	 */
	public static synchronized Watchdog getInstance() {
		if (m_instance == null) {
			m_instance = new Watchdog();
		}
		return m_instance;
	}

	/**
	 * Throw the dog a bone.
	 * 
	 * When everything is going well, you feed your dog when you get home. Let's
	 * hope you don't drive your car off a bridge on the way home... Your dog
	 * won't get fed and he will starve to death.
	 * 
	 * By the way, it's not cool to ask the neighbor (some random task) to feed
	 * your dog for you. He's your responsibility!
	 */
	public void feed() {
		IntBuffer status = ByteBuffer.allocateDirect(4).asIntBuffer();
		WatchdogJNI.feedWatchdog(m_watchDog, status);
		HALUtil.checkStatus(status);
	}

	/**
	 * Put the watchdog out of its misery.
	 * 
	 * Don't wait for your dying robot to starve when there is a problem. Kill
	 * it quickly, cleanly, and humanely.
	 */
	public void kill() {
		IntBuffer status = ByteBuffer.allocateDirect(4).asIntBuffer();
		WatchdogJNI.killWatchdog(m_watchDog, status);
		HALUtil.checkStatus(status);
	}

	/**
	 * Read how long it has been since the watchdog was last fed.
	 * 
	 * @return The number of seconds since last meal.
	 */
	public double getTimer() {
		IntBuffer status = ByteBuffer.allocateDirect(4).asIntBuffer();
		double value = WatchdogJNI.getWatchdogLastFed(m_watchDog, status);
		HALUtil.checkStatus(status);
		return value;
	}

	/**
	 * Read what the current expiration is.
	 * 
	 * @return The number of seconds before starvation following a meal
	 *         (watchdog starves if it doesn't eat this often).
	 */
	public double getExpiration() {
		IntBuffer status = ByteBuffer.allocateDirect(4).asIntBuffer();
		double value = WatchdogJNI.getWatchdogExpiration(m_watchDog, status);
		HALUtil.checkStatus(status);
		return value;
	}

	/**
	 * Configure how many seconds your watchdog can be neglected before it
	 * starves to death.
	 * 
	 * @param expiration
	 *            The number of seconds before starvation following a meal
	 *            (watchdog starves if it doesn't eat this often).
	 */
	public void setExpiration(double expiration) {
		// allocate direct
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set to c++ byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
		WatchdogJNI.setWatchdogExpiration(m_watchDog, expiration, status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
	}

	/**
	 * Find out if the watchdog is currently enabled or disabled (mortal or
	 * immortal).
	 * 
	 * @return Enabled or disabled.
	 */
	public boolean getEnabled() {
		IntBuffer status = ByteBuffer.allocateDirect(4).asIntBuffer();
		boolean value = WatchdogJNI.getWatchdogEnabled(m_watchDog, status) != 0;
		HALUtil.checkStatus(status);
		return value;
	}

	/**
	 * Enable or disable the watchdog timer.
	 * 
	 * When enabled, you must keep feeding the watchdog timer to keep the
	 * watchdog active, and hence the dangerous parts (motor outputs, etc.) can
	 * keep functioning. When disabled, the watchdog is immortal and will remain
	 * active even without being fed. It will also ignore any kill commands
	 * while disabled.
	 * 
	 * @param enabled
	 *            Enable or disable the watchdog.
	 */
	public void setEnabled(final boolean enabled) {
		IntBuffer status = ByteBuffer.allocateDirect(4).asIntBuffer();
		WatchdogJNI.setWatchdogEnabled(m_watchDog, (byte) (enabled ? 1 : 0),
				status);
		HALUtil.checkStatus(status);
	}

	/**
	 * Check in on the watchdog and make sure he's still kicking.
	 * 
	 * This indicates that your watchdog is allowing the system to operate. It
	 * is still possible that the network communications is not allowing the
	 * system to run, but you can check this to make sure it's not your fault.
	 * Check isSystemActive() for overall system status.
	 * 
	 * If the watchdog is disabled, then your watchdog is immortal.
	 * 
	 * @return Is the watchdog still alive?
	 */
	public boolean isAlive() {
		IntBuffer status = ByteBuffer.allocateDirect(4).asIntBuffer();
		boolean value = WatchdogJNI.isWatchdogAlive(m_watchDog, status) != 0;
		HALUtil.checkStatus(status);
		return value;
	}

	/**
	 * Check on the overall status of the system.
	 * 
	 * @return Is the system active (i.e. PWM motor outputs, etc. enabled)?
	 */
	public boolean isSystemActive() {
		IntBuffer status = ByteBuffer.allocateDirect(4).asIntBuffer();
		boolean value = WatchdogJNI.isWatchdogSystemActive(m_watchDog, status) != 0;
		HALUtil.checkStatus(status);
		return value;
	}
}
