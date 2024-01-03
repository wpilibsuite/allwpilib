// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.smartdashboard.proto;

import edu.wpi.first.util.protobuf.Protobuf;
import edu.wpi.first.wpilibj.proto.Mechanism2D.ProtobufMechanism2d;
import edu.wpi.first.wpilibj.proto.Mechanism2D.ProtobufMechanismLigament2d;
import edu.wpi.first.wpilibj.proto.Mechanism2D.ProtobufMechanismRoot2d;
import edu.wpi.first.wpilibj.smartdashboard.Mechanism2d;
import edu.wpi.first.wpilibj.smartdashboard.MechanismLigament2d;
import edu.wpi.first.wpilibj.smartdashboard.MechanismRoot2d;
import edu.wpi.first.wpilibj.util.Color8Bit;
import us.hebi.quickbuf.Descriptors.Descriptor;

public class Mechanism2dProto implements Protobuf<Mechanism2d, ProtobufMechanism2d> {
  @Override
  public Class<Mechanism2d> getTypeClass() {
    return Mechanism2d.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufMechanism2d.getDescriptor();
  }

  @Override
  public ProtobufMechanism2d createMessage() {
    return ProtobufMechanism2d.newInstance();
  }

  @Override
  public Mechanism2d unpack(ProtobufMechanism2d msg) {
    var mechanism =
        new Mechanism2d(msg.getWidth(), msg.getHeight(), new Color8Bit(msg.getBackgroundColor()));
    // Unpack each root into the mechanism
    for (ProtobufMechanismRoot2d root : msg.getRoots()) {
      var mechanismRoot = mechanism.getRoot(root.getName(), root.getX(), root.getY());
      // Unpack the ligaments for each root
      for (ProtobufMechanismLigament2d ligament : root.getLigaments()) {
        mechanismRoot.append(MechanismLigament2d.proto.unpack(ligament));
      }
    }
    return mechanism;
  }

  @Override
  public void pack(ProtobufMechanism2d msg, Mechanism2d value) {
    msg.setHeight(value.getHeight());
    msg.setWidth(value.getWidth());
    msg.setBackgroundColor(value.getBackgroundColor());
    for (MechanismRoot2d root : value.getAllRoots().values()) {
      var nestedRoot = MechanismRoot2d.proto.createMessage();
      MechanismRoot2d.proto.pack(nestedRoot, root);
      msg.addRoots(nestedRoot);
    }
  }
}
