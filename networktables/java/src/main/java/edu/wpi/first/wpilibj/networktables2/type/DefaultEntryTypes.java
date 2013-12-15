package edu.wpi.first.wpilibj.networktables2.type;

import java.io.*;

public class DefaultEntryTypes {
	private static final byte BOOLEAN_RAW_ID = 0x00;
	private static final byte DOUBLE_RAW_ID = 0x01;
	private static final byte STRING_RAW_ID = 0x02;
	
	/**
	 * a boolean entry type
	 */
	public static final NetworkTableEntryType BOOLEAN = new NetworkTableEntryType(BOOLEAN_RAW_ID, "Boolean"){
		public void sendValue(Object value, DataOutputStream os) throws IOException {
			if(value instanceof Boolean)
				os.writeBoolean(((Boolean) value).booleanValue());
			else
				throw new IOException("Cannot write "+value+" as "+name);
		}
		public Object readValue(DataInputStream is) throws IOException {
			return is.readBoolean()?Boolean.TRUE:Boolean.FALSE;
		}
	};
	/**
	 * a double floating point type
	 */
	public static final NetworkTableEntryType DOUBLE = new NetworkTableEntryType(DOUBLE_RAW_ID, "Double"){
		public void sendValue(Object value, DataOutputStream os) throws IOException {
			if(value instanceof Double)
				os.writeDouble(((Double) value).doubleValue());
			else
				throw new IOException("Cannot write "+value+" as "+name);
		}
		public Object readValue(DataInputStream is) throws IOException {
			return new Double(is.readDouble());
		}
	};
	/**
	 * a string type
	 */
	public static final NetworkTableEntryType STRING = new NetworkTableEntryType(STRING_RAW_ID, "String"){
		public void sendValue(Object value, DataOutputStream os) throws IOException {
			if(value instanceof String)
				os.writeUTF((String)value);
			else
				throw new IOException("Cannot write "+value+" as "+name);
		}
		public Object readValue(DataInputStream is) throws IOException {
			return is.readUTF();
		}
	};
	
	public static void registerTypes(NetworkTableEntryTypeManager manager) {
		manager.registerType(BOOLEAN);
		manager.registerType(DOUBLE);
		manager.registerType(STRING);
		manager.registerType(BooleanArray.TYPE);
		manager.registerType(NumberArray.TYPE);
		manager.registerType(StringArray.TYPE);
	}
}
