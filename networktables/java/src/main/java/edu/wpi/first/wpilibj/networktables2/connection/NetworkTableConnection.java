package edu.wpi.first.wpilibj.networktables2.connection;

import java.io.*;

import edu.wpi.first.wpilibj.networktables2.*;
import edu.wpi.first.wpilibj.networktables2.stream.*;
import edu.wpi.first.wpilibj.networktables2.type.*;


/**
 * An abstraction for the NetworkTable protocol
 * 
 * @author mwills
 *
 */
public class NetworkTableConnection {
	public static final char PROTOCOL_REVISION = 0x0200;
        
	private final Object WRITE_LOCK = new Object();

	private final DataInputStream is;
	private final DataOutputStream os;
	/**
	 * the raw stream that is used in this connection
	 */
	public final IOStream stream;
	private final NetworkTableEntryTypeManager typeManager;
	private boolean isValid;

	
	public NetworkTableConnection(IOStream stream, NetworkTableEntryTypeManager typeManager){
		this.stream = stream;
		this.typeManager = typeManager;
		this.is = new DataInputStream(new BufferedInputStream(stream.getInputStream()));
		this.os = new DataOutputStream(new BufferedOutputStream(stream.getOutputStream()));
		isValid = true;
	}

	public void close() {
		if(isValid){
			isValid = false;
			stream.close();
		}
	}


	private void sendMessageHeader(int messageType) throws IOException{
               synchronized(WRITE_LOCK){
		os.writeByte(messageType);
               }
	}
	public void flush() throws IOException{
            synchronized(WRITE_LOCK){
		os.flush();
            }
	}



	public void sendKeepAlive() throws IOException {
            synchronized(WRITE_LOCK){
		sendMessageHeader(NetworkTableMessageType.KEEP_ALIVE);
		flush();
            }
	}

	public void sendClientHello() throws IOException {
            synchronized(WRITE_LOCK){
		sendMessageHeader(NetworkTableMessageType.CLIENT_HELLO);
		os.writeChar(PROTOCOL_REVISION);
		flush();
            }
	}
	
	public void sendServerHelloComplete() throws IOException {
            synchronized(WRITE_LOCK){
		sendMessageHeader(NetworkTableMessageType.SERVER_HELLO_COMPLETE);
		flush();
            }
	}

	public void sendProtocolVersionUnsupported() throws IOException {
            synchronized(WRITE_LOCK){
		sendMessageHeader(NetworkTableMessageType.PROTOCOL_VERSION_UNSUPPORTED);
		os.writeChar(PROTOCOL_REVISION);
		flush();
            }
	}

	
	
	public void sendEntryAssignment(NetworkTableEntry entry) throws IOException {
            synchronized(WRITE_LOCK){
		sendMessageHeader(NetworkTableMessageType.ENTRY_ASSIGNMENT);
		os.writeUTF(entry.name);
		os.writeByte(entry.getType().id);
		os.writeChar(entry.getId());
		os.writeChar(entry.getSequenceNumber());
		entry.sendValue(os);
            }
	}
	public void sendEntryUpdate(NetworkTableEntry entry) throws IOException {
            synchronized(WRITE_LOCK){
		sendMessageHeader(NetworkTableMessageType.FIELD_UPDATE);
		os.writeChar(entry.getId());
		os.writeChar(entry.getSequenceNumber());
		entry.sendValue(os);
            }
	}

	
	
	public void read(ConnectionAdapter adapter) throws IOException {
		try{
			int messageType = is.readByte();
			switch(messageType){
			case NetworkTableMessageType.KEEP_ALIVE:
				adapter.keepAlive();
				return;
			case NetworkTableMessageType.CLIENT_HELLO:
			{
				char protocolRevision = is.readChar();
				adapter.clientHello(protocolRevision);
				return;
			}
			case NetworkTableMessageType.SERVER_HELLO_COMPLETE:
			{
				adapter.serverHelloComplete();
				return;
			}
			case NetworkTableMessageType.PROTOCOL_VERSION_UNSUPPORTED:
			{
				char protocolRevision = is.readChar();
				adapter.protocolVersionUnsupported(protocolRevision);
				return;
			}
			case NetworkTableMessageType.ENTRY_ASSIGNMENT:
			{
				String entryName = is.readUTF();
                                byte typeId = is.readByte();
				NetworkTableEntryType entryType = typeManager.getType(typeId);
				if(entryType==null)
					throw new BadMessageException("Unknown data type: 0x"+Integer.toHexString((int)typeId));
				char entryId = is.readChar();
				char entrySequenceNumber = is.readChar();
				Object value = entryType.readValue(is);
				adapter.offerIncomingAssignment(new NetworkTableEntry(entryId, entryName, entrySequenceNumber, entryType, value));
				return;
			}
			case NetworkTableMessageType.FIELD_UPDATE:
			{
				char entryId = is.readChar();
				char entrySequenceNumber = is.readChar();
				NetworkTableEntry entry = adapter.getEntry(entryId);
				if(entry==null)
					throw new BadMessageException("Received update for unknown entry id: "+(int)entryId);
				Object value = entry.getType().readValue(is);
				
				adapter.offerIncomingUpdate(entry, entrySequenceNumber, value);
				return;
			}
			default:
				throw new BadMessageException("Unknown Network Table Message Type: "+messageType);
			}
		} catch(IOException e){
			throw e;
		}
	}

}
