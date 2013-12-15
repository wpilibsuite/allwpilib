package edu.wpi.first.wpilibj.networktables2.client;

/**
 * Represents a state that the client is in
 * 
 * @author Mitchell
 *
 */
public class ClientConnectionState {
	/**
	 * indicates that the client is disconnected from the server
	 */
	public static final ClientConnectionState DISCONNECTED_FROM_SERVER = new ClientConnectionState("DISCONNECTED_FROM_SERVER");
	/**
	 * indicates that the client is connected to the server but has not yet begun communication
	 */
	public static final ClientConnectionState CONNECTED_TO_SERVER = new ClientConnectionState("CONNECTED_TO_SERVER");
	/**
	 * represents that the client has sent the hello to the server and is waiting for a response
	 */
	public static final ClientConnectionState SENT_HELLO_TO_SERVER = new ClientConnectionState("SENT_HELLO_TO_SERVER");
	/**
	 * represents that the client is now in sync with the server
	 */
	public static final ClientConnectionState IN_SYNC_WITH_SERVER = new ClientConnectionState("IN_SYNC_WITH_SERVER");
	
	/**
	 * Represents that a client received a message from the server indicating that the client's protocol revision is not supported by the server
	 * @author Mitchell
	 *
	 */
	public static class ProtocolUnsuppotedByServer extends ClientConnectionState{
		private final char serverVersion;
		/**
		 * Create a new protocol unsupported state
		 * @param serverVersion
		 */
		public ProtocolUnsuppotedByServer(final char serverVersion){
			super("PROTOCOL_UNSUPPORTED_BY_SERVER");
			this.serverVersion = serverVersion;
		}
		/**
		 * @return the protocol version that the server reported it supports
		 */
		public char getServerVersion(){
			return serverVersion;
		}
		public String toString(){
			return "PROTOCOL_UNSUPPORTED_BY_SERVER: Server Version: 0x"+Integer.toHexString(serverVersion);
		}
	}
	/**
	 * Represents that the client is in an error state
	 * 
	 * @author Mitchell
	 *
	 */
	public static class Error extends ClientConnectionState{
		private final Exception e;
		/**
		 * Create a new error state
		 * @param e
		 */
		public Error(final Exception e){
			super("CLIENT_ERROR");
			this.e = e;
		}
		/**
		 * @return the exception that caused the client to enter an error state
		 */
		public Exception getException(){
			return e;
		}
		public String toString(){
			return "CLIENT_ERROR: "+e.getClass()+": "+e.getMessage();
		}
	}
	
	private String name;
	protected ClientConnectionState(String name){
		this.name = name;
	}
	public String toString(){
		return name;
	}
}
