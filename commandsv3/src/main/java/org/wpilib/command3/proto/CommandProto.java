// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3.proto;

import org.wpilib.command3.Command;
import org.wpilib.command3.Mechanism;
import org.wpilib.command3.Scheduler;
import org.wpilib.command3.proto.ProtobufCommands.ProtobufCommand;
import org.wpilib.util.protobuf.Protobuf;
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
    msg.clear();

    msg.setId(m_scheduler.runId(command));
    Command parent = m_scheduler.getParentOf(command);
    if (parent != null) {
      msg.setParentId(m_scheduler.runId(parent));
    }
    msg.setName(command.name());
    msg.setPriority(command.priority());

    Protobuf.packArray(
        msg.getMutableRequirements(),
        command.requirements().toArray(new Mechanism[0]),
        new MechanismProto());

    if (m_scheduler.isRunning(command)) {
      msg.setLastTimeMs(m_scheduler.lastCommandRuntimeMs(command));
      msg.setTotalTimeMs(m_scheduler.totalRuntimeMs(command));
    }
  }
}
