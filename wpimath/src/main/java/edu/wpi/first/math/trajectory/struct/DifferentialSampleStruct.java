package edu.wpi.first.math.trajectory.struct;

import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.kinematics.ChassisAccelerations;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.trajectory.DifferentialSample;
import edu.wpi.first.math.trajectory.TrajectorySample;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public class DifferentialSampleStruct implements Struct<DifferentialSample> {
  @Override
  public Class<DifferentialSample> getTypeClass() {
    return DifferentialSample.class;
  }

  @Override
  public String getTypeName() {
    return "DifferentialSample";
  }

  @Override
  public int getSize() {
    return TrajectorySample.struct.getSize() + kSizeDouble + kSizeDouble;
  }

  @Override
  public String getSchema() {
    return TrajectorySample.struct.getSchema() + ";double leftSpeed;double rightSpeed";
  }

  @Override
  public DifferentialSample unpack(ByteBuffer bb) {
    TrajectorySample.Base base = TrajectorySample.struct.unpack(bb);
    double leftSpeed = bb.getDouble();
    double rightSpeed = bb.getDouble();
    return new DifferentialSample(base, leftSpeed, rightSpeed);
  }

  @Override
  public void pack(ByteBuffer bb, DifferentialSample value) {
    bb.putDouble(value.timestamp.in(Seconds));
    Pose2d.struct.pack(bb, value.pose);
    ChassisSpeeds.struct.pack(bb, value.vel);
    ChassisAccelerations.struct.pack(bb, value.accel);
    bb.putDouble(value.leftSpeed);
    bb.putDouble(value.rightSpeed);
  }
}
