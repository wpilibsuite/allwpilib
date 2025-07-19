// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3.proto;

import edu.wpi.first.util.protobuf.Protobuf;
import org.wpilib.commands3.Command;
import org.wpilib.commands3.Scheduler;
import us.hebi.quickbuf.Descriptors;

/** Protobuf serde for running commands. */
public class CommandProto implements Protobuf<Command, ProtobufCommand> {
  private final Scheduler m_scheduler;

  public CommandProto(Scheduler scheduler) {
    m_scheduler = scheduler;
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
  public void pack(ProtobufCommand msg, Command command) {
    msg.setId(m_scheduler.runId(command));
    if (m_scheduler.getParentOf(command) instanceof Command parent) {
      msg.setParentId(m_scheduler.runId(parent));
    }
    msg.setName(command.name());
    msg.setPriority(command.priority());

    for (var requirement : command.requirements()) {
      var rrp = new RequireableResourceProto();
      ProtobufRequireableResource requirementMessage = rrp.createMessage();
      rrp.pack(requirementMessage, requirement);
      msg.addRequirements(requirementMessage);
    }

    if (m_scheduler.isRunning(command)) {
      msg.setLastTimeMs(m_scheduler.lastCommandRuntimeMs(command));
      msg.setTotalTimeMs(m_scheduler.totalRuntimeMs(command));
    }
  }
}
