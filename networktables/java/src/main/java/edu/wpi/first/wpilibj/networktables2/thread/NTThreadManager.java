package edu.wpi.first.wpilibj.networktables2.thread;

/**
 * A thread manager that can be used to obtain new threads
 * 
 * @author Mitchell
 *
 */
public interface NTThreadManager {
	/**
	 * @param r
	 * @param name the name of the thread
	 * @return a thread that will run the provided runnable repeatedly with the assumption that the runnable will block
	 */
	NTThread newBlockingPeriodicThread(PeriodicRunnable r, String name);
}
