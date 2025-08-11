package edu.wpi.first.math.trajectory.struct;

import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.kinematics.ChassisAccelerations;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.trajectory.TrajectorySample;
import edu.wpi.first.units.Units;
import edu.wpi.first.util.struct.Struct;
import java.nio.ByteBuffer;

public class TrajectorySampleStruct implements Struct<TrajectorySample> {
  @Override
  public Class<TrajectorySample> getTypeClass() {
    return TrajectorySample.class;
  }

  @Override
  public String getTypeName() {
    return "TrajectorySample";
  }

  @Override
  public int getSize() {
    return kSizeDouble
        + Pose2d.struct.getSize()
        + ChassisSpeeds.struct.getSize()
        + ChassisAccelerations.struct.getSize();
  }

  @Override
  public String getSchema() {
    return "double timestamp;Pose2d pose;ChassisSpeeds vel;ChassisAccelerations accel";
  }

  @Override
  public TrajectorySample unpack(ByteBuffer bb) {
    double dt = bb.getDouble();
    Pose2d pose = Pose2d.struct.unpack(bb);
    ChassisSpeeds vel = ChassisSpeeds.struct.unpack(bb);
    ChassisAccelerations accel = ChassisAccelerations.struct.unpack(bb);
    return new TrajectorySample(Units.Seconds.of(dt), pose, vel, accel);
  }

  @Override
  public void pack(ByteBuffer bb, TrajectorySample value) {
    bb.putDouble(value.timestamp.in(Seconds));
    Pose2d.struct.pack(bb, value.pose);
    ChassisSpeeds.struct.pack(bb, value.vel);
    ChassisAccelerations.struct.pack(bb, value.accel);
  }
}
