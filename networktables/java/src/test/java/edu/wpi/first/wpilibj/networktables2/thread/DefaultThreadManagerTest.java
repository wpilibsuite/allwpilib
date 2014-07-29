package edu.wpi.first.wpilibj.networktables2.thread;

import static org.junit.Assert.*;

import java.io.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.*;

import org.junit.*;

public class DefaultThreadManagerTest {
	volatile Throwable exception = null;
	@Test(timeout=400)
	public void testConcurrentThreads() throws Throwable {
		final int numCounts = 10;
		final int numThreads = 10;
		final CyclicBarrier barrier = new CyclicBarrier(numThreads);
		final CountDownLatch ranNotification = new CountDownLatch(numThreads*numCounts);
		DefaultThreadManager threadManager = new DefaultThreadManager();

		NTThread[] threads = new NTThread[numThreads];
		for(int i = 0; i<numThreads; ++i)
			threads[i] = threadManager.newBlockingPeriodicThread(new PeriodicRunnable(){
				@Override
				public void run() throws InterruptedException {
					try {
						barrier.await();
					} catch (BrokenBarrierException e) {
						throw new InterruptedException();
					}
					ranNotification.countDown();
				}
			}, "Test thread "+i);
		
		try {
			ranNotification.await();
			for(int i = 0; i<numThreads; ++i)
				threads[i].stop();
			Thread.sleep(100);//Wait for threads to terminate
			for(int i = 0; i<numThreads; ++i)
				assertFalse(threads[i].isRunning());
		} catch (InterruptedException e) {
			fail("interrupted while waiting");
		}
		if(exception!=null)
			throw exception;
	}


	volatile boolean threadInterrupted = false;
	@Test(timeout=300)
	public void testStopThread() throws IOException {
		DefaultThreadManager threadManager = new DefaultThreadManager();
		NTThread thread = threadManager.newBlockingPeriodicThread(new PeriodicRunnable(){
			@Override
			public void run() throws InterruptedException {
				Thread.sleep(1000);
			}
		}, "A Test Thread");
		try {
			Thread.sleep(100);
			thread.stop();
			Thread.sleep(100);//wait for thread to stop
			assertFalse(thread.isRunning());
		} catch (InterruptedException e) {
			fail("interrupted while waiting");
		}
	}
	
	@Test(timeout=2000)
	public void testPeriodicThread() throws IOException {
		final CountDownLatch countDown = new CountDownLatch(10);
		final AtomicInteger afterFinishCount = new AtomicInteger(0);
		DefaultThreadManager threadManager = new DefaultThreadManager();
		NTThread thread = threadManager.newBlockingPeriodicThread(new PeriodicRunnable(){
			@Override
			public void run() throws InterruptedException {
				Thread.sleep(50);
				if(countDown.getCount()==0)
					afterFinishCount.incrementAndGet();
				else
					countDown.countDown();
			}
		}, "A test thread");
		try {
			countDown.await();
			thread.stop();
			Thread.sleep(100);//wait for thread to stop
			assertFalse(thread.isRunning());
			assertTrue(afterFinishCount.get()<=1);//make sure the periodic calls stopped
		} catch (InterruptedException e) {
			fail("interrupted while waiting");
		}
	}

}
