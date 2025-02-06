package edu.wpi.first.math.trajectory.struct;

import edu.wpi.first.math.trajectory.TrapezoidProfile;
import edu.wpi.first.util.struct.Struct;

import java.nio.ByteBuffer;

public class TrapezoidProfileStateStruct implements Struct<TrapezoidProfile.State> {
	@Override
	public Class<TrapezoidProfile.State> getTypeClass() {
		return TrapezoidProfile.State.class;
	}
	
	@Override
	public String getTypeName() {
		return "TrapezoidProfileState";
	}
	
	@Override
	public int getSize() {
		return kSizeDouble * 2;
	}
	
	@Override
	public String getSchema() {
		return "double position;double velocity";
	}
	
	@Override
	public TrapezoidProfile.State unpack(ByteBuffer bb) {
		return new TrapezoidProfile.State(bb.getDouble(), bb.getDouble());
	}
	
	@Override
	public void pack(ByteBuffer bb, TrapezoidProfile.State value) {
		bb.putDouble(value.position);
		bb.putDouble(value.velocity);
	}
}
