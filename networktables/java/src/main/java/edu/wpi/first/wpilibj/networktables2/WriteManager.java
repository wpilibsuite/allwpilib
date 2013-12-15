
package edu.wpi.first.wpilibj.networktables2;

import edu.wpi.first.wpilibj.networktables2.thread.*;
import edu.wpi.first.wpilibj.networktables2.util.*;

/**
 * A write manager is a {@link IncomingEntryReceiver} that buffers transactions and then and then dispatches them to a flushable transaction receiver that is periodically offered all queued transaction and then flushed
 * 
 * @author Mitchell
 *
 */
public class WriteManager implements OutgoingEntryReceiver, PeriodicRunnable{
	private final int SLEEP_TIME = 100;
        
	private final int queueSize = 500;
	
	private Object transactionsLock = new Object();
	private NTThread thread;
	private NTThreadManager threadManager;
	private final AbstractNetworkTableEntryStore entryStore;
	
	private volatile HalfQueue incomingAssignmentQueue;
	private volatile HalfQueue incomingUpdateQueue;
	private volatile HalfQueue outgoingAssignmentQueue;
	private volatile HalfQueue outgoingUpdateQueue;
	
	private FlushableOutgoingEntryReceiver receiver;
	private long lastWrite;

	private final long keepAliveDelay;

	/**
	 * Create a new Write manager
	 * @param receiver
	 * @param threadManager
	 * @param transactionPool
	 * @param entryStore
	 */
	public WriteManager(final FlushableOutgoingEntryReceiver receiver, final NTThreadManager threadManager, final AbstractNetworkTableEntryStore entryStore, long keepAliveDelay) {
		this.receiver = receiver;
		this.threadManager = threadManager;
		this.entryStore = entryStore;
		
		incomingAssignmentQueue = new HalfQueue(queueSize);
		incomingUpdateQueue = new HalfQueue(queueSize);
		outgoingAssignmentQueue = new HalfQueue(queueSize);
		outgoingUpdateQueue = new HalfQueue(queueSize);
		
		this.keepAliveDelay = keepAliveDelay;
	}

	/**
	 * start the write thread
	 */
	public void start(){
		if(thread!=null)
			stop();
		lastWrite = System.currentTimeMillis();
		thread = threadManager.newBlockingPeriodicThread(this, "Write Manager Thread");
	}
	/**
	 * stop the write thread
	 */
	public void stop(){
		if(thread!=null)
			thread.stop();
	}


	public void offerOutgoingAssignment(NetworkTableEntry entry) {
		synchronized(transactionsLock){
			incomingAssignmentQueue.queue(entry);
			if(incomingAssignmentQueue.isFull()){
				try {
					run();
				} catch (InterruptedException e) {}
				System.err.println("assignment queue overflowed. decrease the rate at which you create new entries or increase the write buffer size");
			}
		}
	}


	public void offerOutgoingUpdate(NetworkTableEntry entry) {
		synchronized(transactionsLock){
			incomingUpdateQueue.queue(entry);
			if(incomingUpdateQueue.isFull()){
				try {
					run();
				} catch (InterruptedException e) {}
				System.err.println("update queue overflowed. decrease the rate at which you update entries or increase the write buffer size");
			}
		}
	}

	
	/**
	 * the periodic method that sends all buffered transactions
	 */
	public void run() throws InterruptedException {
		synchronized(transactionsLock){
			//swap the assignment and update queue
			HalfQueue tmp = incomingAssignmentQueue;
			incomingAssignmentQueue = outgoingAssignmentQueue;
			outgoingAssignmentQueue = tmp;
			
			tmp = incomingUpdateQueue;
			incomingUpdateQueue = outgoingUpdateQueue;
			outgoingUpdateQueue = tmp;
		}
		
		boolean wrote = false;
		NetworkTableEntry entry;
		int i;
		int size = outgoingAssignmentQueue.size();
		Object[] array = outgoingAssignmentQueue.array;
		for(i = 0; i<size; ++i){
			entry = (NetworkTableEntry)array[i];
			synchronized(entryStore){
				entry.makeClean();
			}
			wrote = true;
			receiver.offerOutgoingAssignment(entry);
		}
		outgoingAssignmentQueue.clear();
		
		
		size = outgoingUpdateQueue.size();
		array = outgoingUpdateQueue.array;
		for(i = 0; i<size; ++i){
			entry = (NetworkTableEntry)array[i];
			synchronized(entryStore){
				entry.makeClean();
			}
			wrote = true;
			receiver.offerOutgoingUpdate(entry);
		}
		outgoingUpdateQueue.clear();
		
		
		if(wrote){
			receiver.flush();
			lastWrite = System.currentTimeMillis();
                }
		else if(System.currentTimeMillis()-lastWrite>keepAliveDelay)
			receiver.ensureAlive();
		
		Thread.sleep(SLEEP_TIME);
	}

}