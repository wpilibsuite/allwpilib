package edu.wpi.first.math.kinematics.struct;

import java.nio.ByteBuffer;

import edu.wpi.first.math.kinematics.DifferentialDriveWheelPositions;
import edu.wpi.first.util.struct.Struct;

public class DifferentialDriveWheelPositionsStruct implements Struct<DifferentialDriveWheelPositions> {

    @Override
    public Class<DifferentialDriveWheelPositions> getTypeClass() {
        return DifferentialDriveWheelPositions.class;
    }

    @Override
    public String getTypeString() {
       return "struct:DifferentialDriveWheelPositions";
    }

    @Override
    public int getSize() {
        return kSizeDouble * 2;
    }

    @Override
    public String getSchema() {
        return "double left;double right";
    }

    @Override
    public DifferentialDriveWheelPositions unpack(ByteBuffer bb) {
        double left = bb.getDouble();
        double right = bb.getDouble();
        return new DifferentialDriveWheelPositions(left, right);
    }

    @Override
    public void pack(ByteBuffer bb, DifferentialDriveWheelPositions value) {
        bb.putDouble(value.leftMeters);
        bb.putDouble(value.rightMeters);
    }
    
}
