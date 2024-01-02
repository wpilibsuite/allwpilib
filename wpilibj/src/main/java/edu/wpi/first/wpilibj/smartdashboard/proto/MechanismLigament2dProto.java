// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.smartdashboard.proto;

import edu.wpi.first.util.protobuf.Protobuf;
import edu.wpi.first.wpilibj.proto.Mechanism2D.ProtobufMechanismLigament2d;
import edu.wpi.first.wpilibj.smartdashboard.MechanismLigament2d;
import edu.wpi.first.wpilibj.smartdashboard.MechanismObject2d;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class MechanismLigament2dProto
    implements Protobuf<MechanismLigament2d, ProtobufMechanismLigament2d> {
  @Override
  public Class<MechanismLigament2d> getTypeClass() {
    return MechanismLigament2d.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufMechanismLigament2d.getDescriptor();
  }

  @Override
  public ProtobufMechanismLigament2d createMessage() {
    return ProtobufMechanismLigament2d.newInstance();
  }

  @Override
  public MechanismLigament2d unpack(ProtobufMechanismLigament2d msg) {
    var root = new MechanismLigament2d(msg.getName(), msg.getLength(), msg.getAngle());
    for (ProtobufMechanismLigament2d ligament : msg.getLigaments()) {
      root.append(unpack(ligament));
    }
    return root;
  }

  @Override
  public void pack(ProtobufMechanismLigament2d msg, MechanismLigament2d value) {
    msg.setAngle(value.getAngle());
    msg.setColor(value.getColor().toHexString());
    msg.setLength(value.getLength());
    msg.setName(value.getName());
    msg.setWeight(value.getLineWeight());
    for (MechanismObject2d ligament : value.getObjects().values()) {
      var nestedLigament = createMessage();
      pack(nestedLigament, (MechanismLigament2d) ligament);
      msg.addLigaments(nestedLigament);
    }
  }
}
