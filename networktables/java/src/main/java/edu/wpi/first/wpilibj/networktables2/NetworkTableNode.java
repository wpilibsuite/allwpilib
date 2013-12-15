package edu.wpi.first.wpilibj.networktables2;

import edu.wpi.first.wpilibj.networktables2.AbstractNetworkTableEntryStore.TableListenerManager;
import edu.wpi.first.wpilibj.networktables2.client.*;
import edu.wpi.first.wpilibj.networktables2.type.*;
import edu.wpi.first.wpilibj.networktables2.util.*;
import edu.wpi.first.wpilibj.tables.*;

/**
 * represents a node (either a client or a server) in a network tables 2.0
 * <br>
 * implementers of the class must ensure that they call {@link #init(NetworkTableTransactionPool, AbstractNetworkTableEntryStore)} before calling any other methods on this class
 * 
 * @author Mitchell
 *
 */
public abstract class NetworkTableNode implements TableListenerManager, ClientConnectionListenerManager, IRemote{

	protected AbstractNetworkTableEntryStore entryStore;
	

	protected final void init(AbstractNetworkTableEntryStore entryStore) {
		this.entryStore = entryStore;
	}
	
	/**
	 * @return the entry store used by this node
	 */
	public AbstractNetworkTableEntryStore getEntryStore(){
		return entryStore;
	}

	
	
	public void putBoolean(String name, boolean value){
		putValue(name, DefaultEntryTypes.BOOLEAN, value?Boolean.TRUE:Boolean.FALSE);
	}
	public boolean getBoolean(String name) throws TableKeyNotDefinedException{
		NetworkTableEntry entry = entryStore.getEntry(name);
		if(entry==null)
			throw new TableKeyNotDefinedException(name);
		return ((Boolean)entry.getValue()).booleanValue();
	}

	public void putDouble(String name, double value){
		putValue(name, DefaultEntryTypes.DOUBLE, new Double(value));//TODO don't make a new double every time
	}
	public double getDouble(String name) throws TableKeyNotDefinedException{
		NetworkTableEntry entry = entryStore.getEntry(name);
		if(entry==null)
			throw new TableKeyNotDefinedException(name);
		return ((Double)entry.getValue()).doubleValue();
	}

	public void putString(String name, String value){
		putValue(name, DefaultEntryTypes.STRING, value);
	}
	public String getString(String name) throws TableKeyNotDefinedException{
		NetworkTableEntry entry = entryStore.getEntry(name);
		if(entry==null)
			throw new TableKeyNotDefinedException(name);
		return ((String)entry.getValue());
	}
        
        public void putComplex(String name, ComplexData value){
            putValue(name, value.getType(), value);
        }
        
	public void retrieveValue(String name, Object externalData) throws TableKeyNotDefinedException{
            synchronized(entryStore){
                NetworkTableEntry entry = entryStore.getEntry(name);
                if(entry==null)
                    throw new TableKeyNotDefinedException(name);
                NetworkTableEntryType entryType = entry.getType();
                if(!(entryType instanceof ComplexEntryType))
                    throw new TableKeyExistsWithDifferentTypeException(name, entryType, "Is not a complex data type");
                ComplexEntryType complexType = (ComplexEntryType)entryType;
                complexType.exportValue(name, entry.getValue(), externalData);
            }
        }
	
	
	
	
	public void putValue(String name, Object value) throws IllegalArgumentException{
            if(value instanceof Double){
                    putValue(name, DefaultEntryTypes.DOUBLE, value);
            } else if (value instanceof String){
                    putValue(name, DefaultEntryTypes.STRING, value);
            } else if(value instanceof Boolean){
                    putValue(name, DefaultEntryTypes.BOOLEAN, value);
            } else if(value instanceof ComplexData){
                    putValue(name, ((ComplexData)value).getType(), value);
            } else if(value==null) {
                throw new NullPointerException("Cannot put a null value into networktables");
            } else {
                throw new IllegalArgumentException("Invalid Type");
            }
	}
	
	/**
	 * Put a value with a specific network table type
	 * @param name the name of the entry to associate with the given value
	 * @param type the type of the entry
	 * @param value the actual value of the entry
	 */
	public void putValue(String name, NetworkTableEntryType type, Object value){
            if(type instanceof ComplexEntryType){
                synchronized(entryStore){//must sync because use get
                    ComplexEntryType entryType = (ComplexEntryType)type;
                    NetworkTableEntry entry = entryStore.getEntry(name);
                    if(entry!=null)
                        entryStore.putOutgoing(entry, entryType.internalizeValue(entry.name, value, entry.getValue()));
                    else
                    	entryStore.putOutgoing(name, type, entryType.internalizeValue(name, value, null));
                }
            }
            else
                entryStore.putOutgoing(name, type, value);
	}
	
	public void putValue(NetworkTableEntry entry, Object value){
        if(entry.getType() instanceof ComplexEntryType){
            synchronized(entryStore){//must sync because use get
                ComplexEntryType entryType = (ComplexEntryType)entry.getType();
                entryStore.putOutgoing(entry, entryType.internalizeValue(entry.name, value, entry.getValue()));
            }
        }
        else
        	entryStore.putOutgoing(entry, value);
	}
	
	public Object getValue(String name) throws TableKeyNotDefinedException{//TODO don't allow get of complex types
            synchronized(entryStore){
                NetworkTableEntry entry = entryStore.getEntry(name);
                if(entry == null)
                        throw new TableKeyNotDefinedException(name);
                return entry.getValue();
            }
	}
	
	
	/**
	 * @param key the key to check for existence
	 * @return true if the table has the given key
	 */
	public boolean containsKey(final String key){
		return entryStore.getEntry(key)!=null;
	}

	/**
	 * close all networking activity related to this node
	 */
	public abstract void close();
	
	private final List remoteListeners = new List();
	public void addConnectionListener(IRemoteConnectionListener listener, boolean immediateNotify) {
		remoteListeners.add(listener);
		if(isConnected())
			listener.connected(this);
		else
			listener.disconnected(this);
	}
	public void removeConnectionListener(IRemoteConnectionListener listener) {
		remoteListeners.remove(listener);
	}
	public void fireConnectedEvent(){
		for(int i = 0; i<remoteListeners.size(); ++i)
			((IRemoteConnectionListener)remoteListeners.get(i)).connected(this);
	}
	public void fireDisconnectedEvent(){
		for(int i = 0; i<remoteListeners.size(); ++i)
			((IRemoteConnectionListener)remoteListeners.get(i)).disconnected(this);
	}
	

	private final List tableListeners = new List();
	public void addTableListener(ITableListener listener, boolean immediateNotify) {
		tableListeners.add(listener);
		if(immediateNotify)
			entryStore.notifyEntries(null, listener);
	}
	public void removeTableListener(ITableListener listener) {
		tableListeners.remove(listener);
	}
	public void fireTableListeners(String key, Object value, boolean isNew){
		for(int i = 0; i<tableListeners.size(); ++i)
			((ITableListener)tableListeners.get(i)).valueChanged(null, key, value, isNew);
	}
}
