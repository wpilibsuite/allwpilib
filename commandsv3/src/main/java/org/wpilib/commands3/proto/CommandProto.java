// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3.proto;

import edu.wpi.first.util.protobuf.Protobuf;
import org.wpilib.commands3.Command;
import org.wpilib.commands3.Scheduler;
import us.hebi.quickbuf.Descriptors;

public class CommandProto implements Protobuf<Command, ProtobufCommand> {
  private final Scheduler scheduler;

  public CommandProto(Scheduler scheduler) {
    this.scheduler = scheduler;
  }

  @Override
  public Class<Command> getTypeClass() {
    return Command.class;
  }

  @Override
  public Descriptors.Descriptor getDescriptor() {
    return ProtobufCommand.getDescriptor();
  }

  @Override
  public ProtobufCommand createMessage() {
    return ProtobufCommand.newInstance();
  }

  @Override
  public Command unpack(ProtobufCommand msg) {
    // Not possible. The command behavior is what really matters, and it cannot be serialized
    throw new UnsupportedOperationException("Deserialization not supported");
  }

  @Override
  public void pack(ProtobufCommand msg, Command value) {
    // Use identityHashCode for ID fields. Object hashCode can be overridden and collide
    msg.setId(System.identityHashCode(value));
    if (scheduler.getParentOf(value) instanceof Command parent) {
      msg.setParentId(System.identityHashCode(parent));
    }
    msg.setName(value.name());
    msg.setPriority(value.priority());

    for (var requirement : value.requirements()) {
      var rrp = new RequireableResourceProto();
      ProtobufRequireableResource requirementMessage = rrp.createMessage();
      rrp.pack(requirementMessage, requirement);
      msg.addRequirements(requirementMessage);
    }
  }
}
