package test.util;

import java.util.concurrent.*;

public class TestExecutor implements Executor, ThreadFactory{
	private ScheduledExecutorService executor;
	private long threadTimeout;
	private LinkedBlockingQueue<Throwable> exceptions = new LinkedBlockingQueue<Throwable>();

	public TestExecutor(int numThreads, long threadTimeout) {
		this.threadTimeout = threadTimeout;
		executor = Executors.newScheduledThreadPool(numThreads, this);
	}

	@Override
	public void execute(Runnable command) {
		executor.schedule(command, threadTimeout, TimeUnit.MILLISECONDS);
	}

	@Override
	public Thread newThread(final Runnable r) {
		return new Thread(new Runnable(){
			@Override
			public void run() {
				try{
					r.run();
				} catch(Throwable t){
					exceptions.offer(t);
				}
			}
		});
	}
	public void await(long timeout, TimeUnit unit) throws Throwable{
		executor.shutdown();
		executor.awaitTermination(timeout, unit);
		Throwable firstError = exceptions.poll();
		if(firstError!=null)
			throw firstError;
	}
}
