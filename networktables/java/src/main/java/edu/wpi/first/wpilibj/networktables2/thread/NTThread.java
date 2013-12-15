package edu.wpi.first.wpilibj.networktables2.thread;

/**
 * Represents a thread in the network tables system
 * @author mwills
 *
 */
public interface NTThread {
	/**
	 * stop the thread
	 */
	void stop();
	/**
	 * @return true if the thread is running
	 */
	boolean isRunning();
}
