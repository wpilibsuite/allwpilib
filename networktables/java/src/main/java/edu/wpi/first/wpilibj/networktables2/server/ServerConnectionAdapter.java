package edu.wpi.first.wpilibj.networktables2.server;

import edu.wpi.first.wpilibj.networktables2.connection.BadMessageException;
import java.io.*;

import edu.wpi.first.wpilibj.networktables2.*;
import edu.wpi.first.wpilibj.networktables2.connection.*;
import edu.wpi.first.wpilibj.networktables2.stream.*;
import edu.wpi.first.wpilibj.networktables2.thread.*;
import edu.wpi.first.wpilibj.networktables2.type.*;

/**
 * Object that adapts messages from a client to the server
 * 
 * @author Mitchell
 *
 */
public class ServerConnectionAdapter implements ConnectionAdapter, IncomingEntryReceiver, FlushableOutgoingEntryReceiver{
	
	private final ServerNetworkTableEntryStore entryStore;
	private final IncomingEntryReceiver transactionReceiver;
	private final ServerAdapterManager adapterListener;
	/**
	 * the connection this adapter uses
	 */
	public final NetworkTableConnection connection;
	private final NTThread readThread;
	
	private ServerConnectionState connectionState;
	private void gotoState(ServerConnectionState newState){
		if(connectionState!=newState){
			System.out.println(this+" entered connection state: "+newState);
			connectionState = newState;
		}
	}

	/**
	 * Create a server connection adapter for a given stream
	 * 
	 * @param stream
	 * @param transactionPool
	 * @param entryStore
	 * @param transactionReceiver
	 * @param adapterListener
	 * @param threadManager
	 */
	public ServerConnectionAdapter(final IOStream stream, final ServerNetworkTableEntryStore entryStore, final IncomingEntryReceiver transactionReceiver, final ServerAdapterManager adapterListener, final NetworkTableEntryTypeManager typeManager, final NTThreadManager threadManager) {
		connection = new NetworkTableConnection(stream, typeManager);
		this.entryStore = entryStore;
		this.transactionReceiver = transactionReceiver;
		this.adapterListener = adapterListener;
		
		gotoState(ServerConnectionState.GOT_CONNECTION_FROM_CLIENT);
		readThread = threadManager.newBlockingPeriodicThread(new ConnectionMonitorThread(this, connection), "Server Connection Reader Thread");
	}
	

	public void badMessage(BadMessageException e) {
            gotoState(new ServerConnectionState.Error(e));
            adapterListener.close(this, true);
	}
	public void ioException(IOException e) {
            if(e instanceof EOFException)
                gotoState(ServerConnectionState.CLIENT_DISCONNECTED);
            else
                gotoState(new ServerConnectionState.Error(e));
            adapterListener.close(this, false);
	}
	
	
	/**
	 * stop the read thread and close the stream
	 */
	public void shutdown(boolean closeStream) {
		readThread.stop();
                if(closeStream)
		connection.close();
	}

	public void keepAlive() throws IOException {
		//just let it happen
	}

	public void clientHello(char protocolRevision) throws IOException {
		if(connectionState!=ServerConnectionState.GOT_CONNECTION_FROM_CLIENT)
			throw new BadMessageException("A server should not receive a client hello after it has already connected/entered an error state");
                if(protocolRevision!=NetworkTableConnection.PROTOCOL_REVISION){
                    connection.sendProtocolVersionUnsupported();
                    throw new BadMessageException("Client Connected with bad protocol revision: 0x"+Integer.toHexString(protocolRevision));
                }
                else{
                    entryStore.sendServerHello(connection);
                    gotoState(ServerConnectionState.CONNECTED_TO_CLIENT);
                }
	}

	public void protocolVersionUnsupported(char protocolRevision) throws IOException {
		throw new BadMessageException("A server should not receive a protocol version unsupported message");
	}

	public void serverHelloComplete() throws IOException {
		throw new BadMessageException("A server should not receive a server hello complete message");
	}

	public void offerIncomingAssignment(NetworkTableEntry entry) {
		transactionReceiver.offerIncomingAssignment(entry);
	}

	public void offerIncomingUpdate(NetworkTableEntry entry, char sequenceNumber, Object value) {
		transactionReceiver.offerIncomingUpdate(entry, sequenceNumber, value);
	}

	public NetworkTableEntry getEntry(char id) {
		return entryStore.getEntry(id);
	}

	public void offerOutgoingAssignment(NetworkTableEntry entry) {
		try {
			if(connectionState==ServerConnectionState.CONNECTED_TO_CLIENT)
				connection.sendEntryAssignment(entry);
		} catch (IOException e) {
			ioException(e);
		}
	}
	public void offerOutgoingUpdate(NetworkTableEntry entry) {
		try {
			if(connectionState==ServerConnectionState.CONNECTED_TO_CLIENT)
				connection.sendEntryUpdate(entry);
		} catch (IOException e) {
			ioException(e);
		}
	}


	public void flush() {
		try {
			connection.flush();
		} catch (IOException e) {
			ioException(e);
		}
	}

	/**
	 * @return the state of the connection
	 */
	public ServerConnectionState getConnectionState() {
            return connectionState;
	}

	public void ensureAlive() {
            try {
                    connection.sendKeepAlive();
            } catch (IOException e) {
                    ioException(e);
            }
	}

}
