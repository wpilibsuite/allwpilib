package edu.wpi.first.wpilibj.networktables2.server;

import edu.wpi.first.wpilibj.networktables2.*;
import edu.wpi.first.wpilibj.networktables2.stream.*;
import edu.wpi.first.wpilibj.networktables2.thread.*;
import edu.wpi.first.wpilibj.networktables2.type.*;

/**
 * A server node in NetworkTables 2.0
 * 
 * @author Mitchell
 *
 */
public class NetworkTableServer extends NetworkTableNode implements ServerIncomingConnectionListener{
	private final ServerIncomingStreamMonitor incomingStreamMonitor;
	private final WriteManager writeManager;
	private final IOStreamProvider streamProvider;
	private final ServerConnectionList connectionList;

	/**
	 * Create a NetworkTable Server
	 * 
	 * @param streamProvider
	 * @param threadManager
	 * @param transactionPool
	 */
	public NetworkTableServer(final IOStreamProvider streamProvider, final NetworkTableEntryTypeManager typeManager, final NTThreadManager threadManager){
		ServerNetworkTableEntryStore entryStore;
		init(entryStore = new ServerNetworkTableEntryStore(this));
		this.streamProvider = streamProvider;
		
		connectionList = new ServerConnectionList();
		writeManager = new WriteManager(connectionList, threadManager, getEntryStore(), Long.MAX_VALUE);

		incomingStreamMonitor = new ServerIncomingStreamMonitor(streamProvider, entryStore, this, connectionList, typeManager, threadManager);
		
		getEntryStore().setIncomingReceiver(new TransactionDirtier(writeManager));
		getEntryStore().setOutgoingReceiver(new TransactionDirtier(writeManager));
		
		incomingStreamMonitor.start();
		writeManager.start();
	}
	/**
	 * Create a NetworkTable Server
	 * 
	 * @param streamProvider
	 */
	public NetworkTableServer(final IOStreamProvider streamProvider){
		this(streamProvider, new NetworkTableEntryTypeManager(), new DefaultThreadManager());
	}
	
	public void close(){
		try {
			incomingStreamMonitor.stop();
			writeManager.stop();
			connectionList.closeAll();
			Thread.sleep(1000);//To get around bug where an error will occur in select if the socket server is closed before all sockets finish closing
			streamProvider.close();
			Thread.sleep(1000);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public void onNewConnection(ServerConnectionAdapter connectionAdapter) {
		connectionList.add(connectionAdapter);
	}

	
	public boolean isConnected() {
		return true;
	}

	
	public boolean isServer() {
		return true;
	}

}