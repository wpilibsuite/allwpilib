package edu.wpi.first.math.trajectory.struct;

import edu.wpi.first.math.trajectory.ExponentialProfile;
import edu.wpi.first.util.struct.Struct;

import java.nio.ByteBuffer;

public class ExponentialProfileStateStruct implements Struct<ExponentialProfile.State> {
	@Override
	public Class<ExponentialProfile.State> getTypeClass() {
		return ExponentialProfile.State.class;
	}
	
	@Override
	public String getTypeName() {
		return "TrapezoidProfile.State";
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
	public ExponentialProfile.State unpack(ByteBuffer bb) {
		return new ExponentialProfile.State(bb.getDouble(), bb.getDouble());
	}
	
	@Override
	public void pack(ByteBuffer bb, ExponentialProfile.State value) {
		bb.putDouble(value.position);
		bb.putDouble(value.velocity);
	}
}
