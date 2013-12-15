package edu.wpi.first.wpilibj.networktables2.server;

import java.io.*;

import edu.wpi.first.wpilibj.networktables2.stream.*;
import edu.wpi.first.wpilibj.networktables2.thread.*;
import edu.wpi.first.wpilibj.networktables2.type.*;

/**
 * Thread that monitors for incoming connections
 * 
 * @author Mitchell
 *
 */
public class ServerIncomingStreamMonitor implements PeriodicRunnable{

	private final IOStreamProvider streamProvider;
	private final ServerIncomingConnectionListener incomingListener;
	private final ServerNetworkTableEntryStore entryStore;
	private final ServerAdapterManager adapterListener;

	private NTThread monitorThread;
	private NTThreadManager threadManager;
	private final NetworkTableEntryTypeManager typeManager;
	
	/**
	 * Create a new incoming stream monitor
	 * @param streamProvider the stream provider to retrieve streams from
	 * @param entryStore the entry store for the server
	 * @param transactionPool transaction pool for the server
	 * @param incomingListener the listener that is notified of new connections
	 * @param adapterListener the listener that will listen to adapter events
	 * @param threadManager the thread manager used to create the incoming thread and provided to the Connection Adapters
	 */
	public ServerIncomingStreamMonitor(final IOStreamProvider streamProvider, final ServerNetworkTableEntryStore entryStore,
			final ServerIncomingConnectionListener incomingListener,
			final ServerAdapterManager adapterListener,
			final NetworkTableEntryTypeManager typeManager, final NTThreadManager threadManager) {
		this.streamProvider = streamProvider;
		this.entryStore = entryStore;
		this.incomingListener = incomingListener;
		this.adapterListener = adapterListener;
		this.typeManager = typeManager;
		this.threadManager = threadManager;
	}
	
	/**
	 * Start the monitor thread
	 */
	public void start(){
		if(monitorThread!=null)
			stop();
		monitorThread = threadManager.newBlockingPeriodicThread(this, "Server Incoming Stream Monitor Thread");
	}
	/**
	 * Stop the monitor thread
	 */
	public void stop(){
		if(monitorThread!=null)
			monitorThread.stop();
	}
	
	public void run(){
		IOStream newStream = null;
		try {
			newStream = streamProvider.accept();
			if(newStream!=null){
				ServerConnectionAdapter connectionAdapter = new ServerConnectionAdapter(newStream, entryStore, entryStore, adapterListener, typeManager, threadManager);
				incomingListener.onNewConnection(connectionAdapter);
			}
		} catch (IOException e) {
			//could not get a new stream for some reason. ignore and continue
		}
	}
	
	
	
}
