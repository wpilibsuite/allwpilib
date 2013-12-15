package test.util;

import org.hamcrest.*;

import edu.wpi.first.wpilibj.networktables2.*;
import edu.wpi.first.wpilibj.networktables2.type.*;

public class NetworkTableEntryUtil {
	public static NetworkTableEntry newBooleanEntry(final String name, final boolean value){
		return new NetworkTableEntry(name, DefaultEntryTypes.BOOLEAN, new Boolean(value));
	}
	public static NetworkTableEntry newBooleanEntry(char id, String name, char sequenceNumber, boolean value){
		return new NetworkTableEntry(id, name, sequenceNumber, DefaultEntryTypes.BOOLEAN, new Boolean(value));
	}

	public static NetworkTableEntry newDoubleEntry(String name, double value){
		return new NetworkTableEntry(name, DefaultEntryTypes.DOUBLE, new Double(value));
	}
	public static NetworkTableEntry newDoubleEntry(char id, String name, char sequenceNumber, double value){
		return new NetworkTableEntry(id, name, sequenceNumber, DefaultEntryTypes.DOUBLE, new Double(value));
	}

	public static NetworkTableEntry newStringEntry(String name, String value){
		return new NetworkTableEntry(name, DefaultEntryTypes.STRING, value);
	}
	public static NetworkTableEntry newStringEntry(char id, String name, char sequenceNumber, String value){
		return new NetworkTableEntry(id, name, sequenceNumber, DefaultEntryTypes.STRING, value);
	}
	
	public static Matcher<NetworkTableEntry> aBooleanEntry(final int id, final String name, final int sequenceNumber, final boolean value){
		return aTableEntry((char)id, name, (char)sequenceNumber, DefaultEntryTypes.BOOLEAN, value);
	}
	public static Matcher<NetworkTableEntry> aDoubleEntry(final int id, final String name, final int sequenceNumber, final double value){
		return aTableEntry((char)id, name, (char)sequenceNumber, DefaultEntryTypes.DOUBLE, value);
	}
	public static Matcher<NetworkTableEntry> aStringEntry(final int id, final String name, final int sequenceNumber, final String value){
		return aTableEntry((char)id, name, (char)sequenceNumber, DefaultEntryTypes.STRING, value);
	}
	
	public static Matcher<NetworkTableEntry> aTableEntry(final char id, final String name, final char sequenceNumber, final NetworkTableEntryType type, final Object value){
		return new TypeSafeMatcher<NetworkTableEntry>() {
			@Override
			public void describeTo(Description description) {
				description.appendText(" a NetworkTableEntry with an id ").appendValue((int)id)
				.appendText(" and name ").appendValue(name)
				.appendText(" and sequence number ").appendValue((int)sequenceNumber)
				.appendText(" and type ").appendValue(type)
				.appendText(" and value ").appendValue(value);
			}

			@Override
			protected boolean matchesSafely(NetworkTableEntry item) {
				if(item.getId()!=id)
					return false;
				if(!item.name.equals(name))
					return false;
				if(item.getSequenceNumber()!=sequenceNumber)
					return false;
				if(!item.getType().equals(type))
					return false;
				if(!item.getValue().equals(value))
					return false;
				return true;
			}
		};
	}
}
