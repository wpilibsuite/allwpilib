// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3.proto;

import edu.wpi.first.util.protobuf.Protobuf;
import org.wpilib.commands3.Scheduler;
import us.hebi.quickbuf.Descriptors;

public class SchedulerProto implements Protobuf<Scheduler, ProtobufScheduler> {
  @Override
  public Class<Scheduler> getTypeClass() {
    return Scheduler.class;
  }

  @Override
  public Descriptors.Descriptor getDescriptor() {
    return ProtobufScheduler.getDescriptor();
  }

  @Override
  public ProtobufScheduler createMessage() {
    return ProtobufScheduler.newInstance();
  }

  @Override
  public Scheduler unpack(ProtobufScheduler msg) {
    throw new UnsupportedOperationException("Deserialization not supported");
  }

  @Override
  public void pack(ProtobufScheduler msg, Scheduler scheduler) {
    var cp = new CommandProto(scheduler);

    for (var command : scheduler.getScheduledCommands()) {
      ProtobufCommand commandMessage = cp.createMessage();
      cp.pack(commandMessage, command);
      msg.addQueuedCommands(commandMessage);
    }

    for (var command : scheduler.getRunningCommands()) {
      ProtobufCommand commandMessage = cp.createMessage();
      cp.pack(commandMessage, command);
      msg.addRunningCommands(commandMessage);
    }

    msg.setLastTimeMs(scheduler.lastRuntimeMs());
  }
}
