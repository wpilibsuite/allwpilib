package test.util;

import edu.wpi.first.wpilibj.networktables2.*;
import edu.wpi.first.wpilibj.networktables2.client.*;

/**
 * @author Mitchell
 *
 */
public class NetworkTableTestNode extends NetworkTableNode{
    
	public NetworkTableTestNode(){
            this(OutgoingEntryReceiver.NULL, OutgoingEntryReceiver.NULL);
        }
	public NetworkTableTestNode(OutgoingEntryReceiver incomingReceiver, OutgoingEntryReceiver outgoingReceiver){
		init(new ClientNetworkTableEntryStore(this));
		
		getEntryStore().setOutgoingReceiver(outgoingReceiver);
		getEntryStore().setIncomingReceiver(incomingReceiver);
	}

    @Override
    public void close() {
    }

    public boolean isConnected() {
        return false;
    }

    public boolean isServer() {
        return false;
    }

}