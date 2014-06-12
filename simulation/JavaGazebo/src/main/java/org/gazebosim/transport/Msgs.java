package org.gazebosim.transport;

import gazebo.msgs.GzBool.Bool;
import gazebo.msgs.GzFloat64;
import gazebo.msgs.GzString;

public class Msgs {
	public static GzString.String String() {
		return GzString.String.getDefaultInstance();
	}
	
	public static GzString.String String(String s) {
		return GzString.String.newBuilder().setData(s).build();
	}
	
	public static GzFloat64.Float64 Float64() {
		return GzFloat64.Float64.getDefaultInstance();
	}
	
	public static GzFloat64.Float64 Float64(double d) {
		return GzFloat64.Float64.newBuilder().setData(d).build();
	}
	
	public static Bool Bool() {
		return Bool.getDefaultInstance();
	}
	
	public static Bool Bool(boolean b) {
		return Bool.newBuilder().setData(b).build();
	}
}
