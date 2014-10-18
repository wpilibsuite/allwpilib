package test.util;

import java.util.*;
import java.util.concurrent.*;

import edu.wpi.first.wpilibj.networktables2.thread.*;

public class TestThreadManager implements NTThreadManager {
	private final List<TestPeriodicThread> periodicThreads = new CopyOnWriteArrayList<TestPeriodicThread>();


	public class TestPeriodicThread implements NTThread{

		private final PeriodicRunnable r;
		private final String name;
		public TestPeriodicThread(PeriodicRunnable r, String name){
			this.r = r;
			this.name = name;
		}
		public String toString(){
			return "Test Thread: "+name;
		}
		public String getName(){
			return r.getClass().getName();
		}
		public void run(){
			try {
				r.run();
			} catch (InterruptedException e) {}
		}
		@Override
		public void stop() {
			periodicThreads.remove(this);
		}

		@Override
		public boolean isRunning() {
			return periodicThreads.contains(this);
		}
	}

	@Override
	public NTThread newBlockingPeriodicThread(PeriodicRunnable r, String name) {
		TestPeriodicThread thread = new TestPeriodicThread(r, name);
		periodicThreads.add(thread);
		return thread;
	}
	
	public TestPeriodicThread getPeriodicThread(Class<?> type){
		TestPeriodicThread foundThread = null;
		for(TestPeriodicThread thread:periodicThreads){
			if(thread.getName().equals(type.getName())){
				if(foundThread!=null)
					throw new RuntimeException("There are more than one periodic threads of type: "+type);
				foundThread = thread;
			}
		}
		if(foundThread==null)
			throw new RuntimeException("There are no periodic threads of type: "+type);
		return foundThread;
	}
	public List<TestPeriodicThread> getAllPeriodicThread(Class<?> type){
		List<TestPeriodicThread> threads = new ArrayList<TestPeriodicThread>();
		for(TestPeriodicThread thread:periodicThreads){
			if(thread.getName().equals(type.getName()))
				threads.add(thread);
		}
		return threads;
	}
	public void runAllPeriodicThread(Class<?> type){
		for(TestPeriodicThread thread:periodicThreads){
			if(thread.getName().equals(type.getName()))
				thread.run();
		}
	}
}
