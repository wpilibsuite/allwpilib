package edu.wpi.first.wpilibj.networktables2.type;

import edu.wpi.first.wpilibj.networktables2.util.*;

public class NetworkTableEntryTypeManager {
	private final ByteArrayMap typeMap = new ByteArrayMap();
	
	public NetworkTableEntryType getType(byte id){
		return (NetworkTableEntryType)typeMap.get(id);
	}
	
	void registerType(NetworkTableEntryType type){
		typeMap.put(type.id, type);
	}
	
	public NetworkTableEntryTypeManager(){
		DefaultEntryTypes.registerTypes(this);
	}
}
