// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.apriltag;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import edu.wpi.first.apriltag.proto.Apriltag.ProtobufAprilTag;
import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.util.protobuf.Protobuf;
import java.util.Objects;
import us.hebi.quickbuf.Descriptors.Descriptor;

@SuppressWarnings("MemberName")
public class AprilTag {
  @JsonProperty(value = "ID")
  public int ID;

  @JsonProperty(value = "pose")
  public Pose3d pose;

  @SuppressWarnings("ParameterName")
  @JsonCreator
  public AprilTag(
      @JsonProperty(required = true, value = "ID") int ID,
      @JsonProperty(required = true, value = "pose") Pose3d pose) {
    this.ID = ID;
    this.pose = pose;
  }

  @Override
  public boolean equals(Object obj) {
    if (obj instanceof AprilTag) {
      var other = (AprilTag) obj;
      return ID == other.ID && pose.equals(other.pose);
    }
    return false;
  }

  @Override
  public int hashCode() {
    return Objects.hash(ID, pose);
  }

  @Override
  public String toString() {
    return "AprilTag(ID: " + ID + ", pose: " + pose + ")";
  }

  public static final class AProto implements Protobuf<AprilTag, ProtobufAprilTag> {
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
      return new AprilTag(msg.getID(), Pose3d.proto.unpack(msg.getPose()));
    }

    @Override
    public void pack(ProtobufAprilTag msg, AprilTag value) {
      msg.setID(value.ID);
      Pose3d.proto.pack(msg.getMutablePose(), value.pose);
    }
  }

  public static final AProto proto = new AProto();
}
