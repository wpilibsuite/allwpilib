// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.vision.apriltag.proto;

import org.wpilib.math.geometry.Pose3d;
import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.vision.apriltag.AprilTag;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class AprilTagProto implements Protobuf<AprilTag, ProtobufAprilTag> {
  @Override
  public Class<AprilTag> getTypeClass() {
    return AprilTag.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufAprilTag.getDescriptor();
  }

  @Override
  public ProtobufAprilTag createMessage() {
    return ProtobufAprilTag.newInstance();
  }

  @Override
  public AprilTag unpack(ProtobufAprilTag msg) {
    return new AprilTag(msg.getId(), Pose3d.proto.unpack(msg.getPose()));
  }

  @Override
  public void pack(ProtobufAprilTag msg, AprilTag value) {
    msg.setId(value.ID);
    Pose3d.proto.pack(msg.getMutablePose(), value.pose);
  }
}
