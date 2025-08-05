package edu.wpi.first.math.trajectory.struct;

import edu.wpi.first.math.trajectory.ExponentialProfile;
import edu.wpi.first.util.struct.Struct;

import java.nio.ByteBuffer;

public class ExponentialProfileTimingStruct implements Struct<ExponentialProfile.ProfileTiming> {
    @Override
    public Class<ExponentialProfile.ProfileTiming> getTypeClass() {
        return ExponentialProfile.ProfileTiming.class;
    }

    @Override
    public String getTypeName() {
        return "ProfileTiming";
    }

    @Override
    public int getSize() {
        return kSizeDouble * 2;
    }

    @Override
    public String getSchema() {
        return "double inflectionTime;double totalTime;";
    }

    @Override
    public ExponentialProfile.ProfileTiming unpack(ByteBuffer bb) {
        return new ExponentialProfile.ProfileTiming(
            bb.getDouble(),
            bb.getDouble()
        );
    }

    @Override
    public void pack(ByteBuffer bb, ExponentialProfile.ProfileTiming value) {
        bb.putDouble(value.inflectionTime);
        bb.putDouble(value.totalTime);
    }
}
