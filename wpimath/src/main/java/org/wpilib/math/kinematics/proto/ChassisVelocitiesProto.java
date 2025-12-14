// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.proto;

import org.wpilib.math.kinematics.ChassisVelocities;
import org.wpilib.math.proto.Kinematics.ProtobufChassisVelocities;
import org.wpilib.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class ChassisVelocitiesProto
    implements Protobuf<ChassisVelocities, ProtobufChassisVelocities> {
  @Override
  public Class<ChassisVelocities> getTypeClass() {
    return ChassisVelocities.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufChassisVelocities.getDescriptor();
  }

  @Override
  public ProtobufChassisVelocities createMessage() {
    return ProtobufChassisVelocities.newInstance();
  }

  @Override
  public ChassisVelocities unpack(ProtobufChassisVelocities msg) {
    return new ChassisVelocities(msg.getVx(), msg.getVy(), msg.getOmega());
  }

  @Override
  public void pack(ProtobufChassisVelocities msg, ChassisVelocities value) {
    msg.setVx(value.vx);
    msg.setVy(value.vy);
    msg.setOmega(value.omega);
  }
}
