// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.smartdashboard.proto;

import edu.wpi.first.util.protobuf.Protobuf;
import edu.wpi.first.wpilibj.proto.Mechanism2D.ProtobufMechanismLigament2d;
import edu.wpi.first.wpilibj.proto.Mechanism2D.ProtobufMechanismRoot2d;
import edu.wpi.first.wpilibj.smartdashboard.MechanismLigament2d;
import edu.wpi.first.wpilibj.smartdashboard.MechanismObject2d;
import edu.wpi.first.wpilibj.smartdashboard.MechanismRoot2d;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class MechanismRoot2dProto implements Protobuf<MechanismRoot2d, ProtobufMechanismRoot2d> {
  @Override
  public Class<MechanismRoot2d> getTypeClass() {
    return MechanismRoot2d.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufMechanismRoot2d.getDescriptor();
  }

  @Override
  public ProtobufMechanismRoot2d createMessage() {
    return ProtobufMechanismRoot2d.newInstance();
  }

  @Override
  public MechanismRoot2d unpack(ProtobufMechanismRoot2d msg) {
    var root = new MechanismRoot2d(msg.getName(), msg.getX(), msg.getY());
    for (ProtobufMechanismLigament2d ligament : msg.getLigaments()) {
      root.append(MechanismLigament2d.proto.unpack(ligament));
    }
    return root;
  }

  @Override
  public void pack(ProtobufMechanismRoot2d msg, MechanismRoot2d value) {
    msg.setX(value.getX());
    msg.setY(value.getY());
    for (MechanismObject2d ligament : value.getObjects().values()) {
      var nestedLigament = MechanismLigament2d.proto.createMessage();
      MechanismLigament2d.proto.pack(nestedLigament, (MechanismLigament2d) ligament);
      msg.addLigaments(nestedLigament);
    }
  }
}
