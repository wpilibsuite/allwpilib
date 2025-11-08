// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3.proto;

import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.command3.Command;
import org.wpilib.command3.Scheduler;
import org.wpilib.command3.proto.ProtobufCommands.ProtobufScheduler;
import us.hebi.quickbuf.Descriptors;

/**
 * Serializes a {@link Scheduler} to a protobuf message. Deserialization is not supported. A
 * serialized message will include information about commands that are currently running or
 * scheduled (but not yet started), as well as how long the most recent call to {@link
 * Scheduler#run()} took to execute.
 */
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
    msg.clear();

    var commandProto = new CommandProto(scheduler);

    Protobuf.packArray(
        msg.getMutableQueuedCommands(),
        scheduler.getQueuedCommands().toArray(new Command[0]),
        commandProto);

    Protobuf.packArray(
        msg.getMutableRunningCommands(),
        scheduler.getRunningCommands().toArray(new Command[0]),
        commandProto);

    msg.setLastTimeMs(scheduler.lastRuntimeMs());
  }
}
