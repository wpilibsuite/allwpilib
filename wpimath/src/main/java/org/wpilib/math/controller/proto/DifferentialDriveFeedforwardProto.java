// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.controller.proto;

import org.wpilib.math.controller.DifferentialDriveFeedforward;
import org.wpilib.math.controller.proto.detail.ProtobufDifferentialDriveFeedforward;
import org.wpilib.util.protobuf.Protobuf;
import us.hebi.quickbuf.Descriptors.Descriptor;

public final class DifferentialDriveFeedforwardProto
    implements Protobuf<DifferentialDriveFeedforward, ProtobufDifferentialDriveFeedforward> {
  @Override
  public Class<DifferentialDriveFeedforward> getTypeClass() {
    return DifferentialDriveFeedforward.class;
  }

  @Override
  public Descriptor getDescriptor() {
    return ProtobufDifferentialDriveFeedforward.getDescriptor();
  }

  @Override
  public ProtobufDifferentialDriveFeedforward createMessage() {
    return ProtobufDifferentialDriveFeedforward.newInstance();
  }

  @Override
  public DifferentialDriveFeedforward unpack(ProtobufDifferentialDriveFeedforward msg) {
    return new DifferentialDriveFeedforward(
        msg.getKvLinear(), msg.getKaLinear(), msg.getKvAngular(), msg.getKaAngular());
  }

  @Override
  public void pack(ProtobufDifferentialDriveFeedforward msg, DifferentialDriveFeedforward value) {
    msg.setKvLinear(value.V_LINEAR);
    msg.setKaLinear(value.A_LINEAR);
    msg.setKvAngular(value.V_ANGULAR);
    msg.setKaAngular(value.A_ANGULAR);
  }
}
