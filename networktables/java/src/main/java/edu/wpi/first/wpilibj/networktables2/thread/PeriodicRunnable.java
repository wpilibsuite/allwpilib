package edu.wpi.first.wpilibj.networktables2.thread;

/**
 * A runnable where the run method will be called periodically 
 * 
 * @author Mitchell
 *
 */
public interface PeriodicRunnable {
	/**
	 * the method that will be called periodically on a thread
	 * 
	 * @throws InterruptedException thrown when the thread is supposed to be interrupted and stop (implementers should always let this exception fall through)
	 */
	public void run() throws InterruptedException;
}
