package edu.wpi.first.wpilibj.networktables2.thread;

/**
 * A simple thread manager that will run periodic threads in their own thread
 * 
 * @author Mitchell
 *
 */
public class DefaultThreadManager implements NTThreadManager {
	private static class PeriodicNTThread implements NTThread {
		private final Thread thread;
		private boolean run = true;
		public PeriodicNTThread(final PeriodicRunnable r, String name){
			thread = new Thread(new Runnable() {
				public void run() {
					try {
						while(run){
							r.run();
						}
					} catch (InterruptedException e) {
						//Terminate thread when interrupted
					}
				}
			}, name);
			thread.start();
		}
		public void stop() {
			run = false;
			thread.interrupt();
		}
		public boolean isRunning() {
			return thread.isAlive();
		}
	}

	public NTThread newBlockingPeriodicThread(final PeriodicRunnable r, String name) {
		return new PeriodicNTThread(r, name);
	}

}
