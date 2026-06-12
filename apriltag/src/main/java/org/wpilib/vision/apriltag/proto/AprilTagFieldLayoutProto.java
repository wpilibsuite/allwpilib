// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.vision.apriltag.proto;

import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.vision.apriltag.AprilTag;
import org.wpilib.vision.apriltag.AprilTagFieldLayout;
import org.wpilib.vision.apriltag.proto.Apriltag.ProtobufAprilTagFieldLayout;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class AprilTagFieldLayoutProto
    implements Protobuf<AprilTagFieldLayout, ProtobufAprilTagFieldLayout> {
  @Override
  public Class<AprilTagFieldLayout> getTypeClass() {
    return AprilTagFieldLayout.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufAprilTagFieldLayout.getDescriptor();
  }

  @Override
  public ProtobufAprilTagFieldLayout createMessage() {
    return ProtobufAprilTagFieldLayout.newInstance();
  }

  @Override
  public AprilTagFieldLayout unpack(ProtobufAprilTagFieldLayout msg) {
    return new AprilTagFieldLayout(
        Protobuf.unpackList(msg.getTags(), AprilTag.proto),
        msg.getField().getLength(),
        msg.getField().getWidth());
  }

  @Override
  public void pack(ProtobufAprilTagFieldLayout msg, AprilTagFieldLayout value) {
    Protobuf.packList(msg.getMutableTags(), value.getTags(), AprilTag.proto);
    var field = msg.getMutableField();
    field.setLength(value.getFieldLength());
    field.setWidth(value.getFieldWidth());
  }
}
