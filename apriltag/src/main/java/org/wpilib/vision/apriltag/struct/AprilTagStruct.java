// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.vision.apriltag.struct;

import java.nio.ByteBuffer;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.util.struct.Struct;
import org.wpilib.vision.apriltag.AprilTag;

public class AprilTagStruct implements Struct<AprilTag> {
  @Override
  public Class<AprilTag> getTypeClass() {
    return AprilTag.class;
  }

  @Override
  public String getTypeName() {
    return "AprilTag";
  }

  @Override
  public int getSize() {
    return INT32_SIZE + Pose3d.struct.getSize();
  }

  @Override
  public String getSchema() {
    return "int id;Pose3d pose";
  }

  @Override
  public Struct<?>[] getNested() {
    return new Struct<?>[] {Pose3d.struct};
  }

  @Override
  public AprilTag unpack(ByteBuffer bb) {
    int id = bb.getInt();
    Pose3d pose = Pose3d.struct.unpack(bb);
    return new AprilTag(id, pose);
  }

  @Override
  public void pack(ByteBuffer bb, AprilTag value) {
    bb.putInt(value.ID);
    Pose3d.struct.pack(bb, value.pose);
  }
}
