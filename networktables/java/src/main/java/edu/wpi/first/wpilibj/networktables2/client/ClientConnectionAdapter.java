package edu.wpi.first.wpilibj.networktables2.client;

import edu.wpi.first.wpilibj.networktables2.connection.BadMessageException;
import java.io.*;

import edu.wpi.first.wpilibj.networktables2.*;
import edu.wpi.first.wpilibj.networktables2.connection.*;
import edu.wpi.first.wpilibj.networktables2.stream.*;
import edu.wpi.first.wpilibj.networktables2.thread.*;
import edu.wpi.first.wpilibj.networktables2.type.*;

/**
 * Object that adapts messages from a server
 * 
 * @author Mitchell
 *
 */
public class ClientConnectionAdapter implements ConnectionAdapter, IncomingEntryReceiver, FlushableOutgoingEntryReceiver{
	
	private final ClientNetworkTableEntryStore entryStore;
	private final IOStreamFactory streamFactory;
	private final NTThreadManager threadManager;
	
	private NetworkTableConnection connection;
	private NTThread readThread;
	private ClientConnectionState connectionState = ClientConnectionState.DISCONNECTED_FROM_SERVER;
	private final ClientConnectionListenerManager connectionListenerManager;
	private final Object connectionLock = new Object();
	private final NetworkTableEntryTypeManager typeManager;

	private void gotoState(ClientConnectionState newState){
		synchronized(connectionLock){
			if(connectionState!=newState){
				System.out.println(this+" entered connection state: "+newState);
				if(newState==ClientConnectionState.IN_SYNC_WITH_SERVER)
					connectionListenerManager.fireConnectedEvent();
				if(connectionState==ClientConnectionState.IN_SYNC_WITH_SERVER)
					connectionListenerManager.fireDisconnectedEvent();
				connectionState = newState;
			}
		}
	}
	/**
	 * @return the state of the connection
	 */
	public ClientConnectionState getConnectionState(){
		return connectionState;
	}
	/**
	 * @return if the client is connected to the server
	 */
	public boolean isConnected() {
		return getConnectionState()==ClientConnectionState.IN_SYNC_WITH_SERVER;
	}

	/**
	 * Create a new ClientConnectionAdapter
	 * @param entryStore
	 * @param threadManager
	 * @param streamFactory
	 * @param transactionPool
	 * @param connectionListenerManager
	 */
	public ClientConnectionAdapter(final ClientNetworkTableEntryStore entryStore, final NTThreadManager threadManager, final IOStreamFactory streamFactory, final ClientConnectionListenerManager connectionListenerManager, final NetworkTableEntryTypeManager typeManager) {
		this.entryStore = entryStore;
		this.streamFactory = streamFactory;
		this.threadManager = threadManager;
		this.connectionListenerManager = connectionListenerManager;
		this.typeManager = typeManager;
	}
	
	
	
	/*
	 * Connection management
	 */
	/**
	 * Reconnect the client to the server (even if the client is not currently connected)
	 */
	public void reconnect() {
		synchronized(connectionLock){
			close();//close the existing stream and monitor thread if needed
			try{
				IOStream stream = streamFactory.createStream();
				if(stream==null)
					return;
				connection = new NetworkTableConnection(stream, typeManager);
				readThread = threadManager.newBlockingPeriodicThread(new ConnectionMonitorThread(this, connection), "Client Connection Reader Thread");
				connection.sendClientHello();
				gotoState(ClientConnectionState.CONNECTED_TO_SERVER);
			} catch(Exception e){
				close();//make sure to clean everything up if we fail to connect
			}
		}
	}
	
	/**
	 * Close the client connection
	 */
	public void close() {
		close(ClientConnectionState.DISCONNECTED_FROM_SERVER);
	}
	/**
	 * Close the connection to the server and enter the given state
	 * @param newState
	 */
	public void close(final ClientConnectionState newState) {
		synchronized(connectionLock){
			gotoState(newState);
			if(readThread!=null){
				readThread.stop();
				readThread = null;
			}
			if(connection!=null){
				connection.close();
				connection = null;
			}
			entryStore.clearIds();
		}
	}
	
	

	public void badMessage(BadMessageException e) {
		close(new ClientConnectionState.Error(e));
	}

	public void ioException(IOException e) {
		if(connectionState!=ClientConnectionState.DISCONNECTED_FROM_SERVER)//will get io exception when on read thread connection is closed
			reconnect();
		//gotoState(new ClientConnectionState.Error(e));
	}
	
	public NetworkTableEntry getEntry(char id) {
		return entryStore.getEntry(id);
	}
	
	
	public void keepAlive() throws IOException {
	}

	public void clientHello(char protocolRevision) throws IOException {
		throw new BadMessageException("A client should not receive a client hello message");
	}

	public void protocolVersionUnsupported(char protocolRevision) {
		close();
		gotoState(new ClientConnectionState.ProtocolUnsuppotedByServer(protocolRevision));
	}

	public void serverHelloComplete() throws IOException {
		if (connectionState==ClientConnectionState.CONNECTED_TO_SERVER) {
			try {
				gotoState(ClientConnectionState.IN_SYNC_WITH_SERVER);
				entryStore.sendUnknownEntries(connection);
			} catch (IOException e) {
				ioException(e);
			}
		}
		else
			throw new BadMessageException("A client should only receive a server hello complete once and only after it has connected to the server");
	}


	public void offerIncomingAssignment(NetworkTableEntry entry) {
		entryStore.offerIncomingAssignment(entry);
	}
	public void offerIncomingUpdate(NetworkTableEntry entry, char sequenceNumber, Object value) {
		entryStore.offerIncomingUpdate(entry, sequenceNumber, value);
	}

	public void offerOutgoingAssignment(NetworkTableEntry entry) {
		try {
			synchronized(connectionLock){
				if(connection!=null && connectionState==ClientConnectionState.IN_SYNC_WITH_SERVER)
					connection.sendEntryAssignment(entry);
			}
		} catch(IOException e){
			ioException(e);
		}
	}

	public void offerOutgoingUpdate(NetworkTableEntry entry) {
		try {
			synchronized(connectionLock){
				if(connection!=null && connectionState==ClientConnectionState.IN_SYNC_WITH_SERVER)
					connection.sendEntryUpdate(entry);
			}
		} catch(IOException e){
			ioException(e);
		}
	}
	public void flush() {
		synchronized(connectionLock){
			if(connection!=null) {
				try {
					connection.flush();
				} catch (IOException e) {
					ioException(e);
				}
			}
		}
	}
	public void ensureAlive() {
		synchronized(connectionLock){
			if(connection!=null) {
				try {
					connection.sendKeepAlive();
				} catch (IOException e) {
					ioException(e);
				}
			}
			else
				reconnect();//try to reconnect if not connected
		}
	}

}
