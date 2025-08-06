// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
