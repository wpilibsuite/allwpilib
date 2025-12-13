// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3.epilogue;

import org.wpilib.command3.Scheduler;
import org.wpilib.epilogue.Logged;

// This class will fail to compile if m_scheduler is not considered loggable
// Note that this assumes epilogue is set up correctly.
@Logged
public class SchedulerIsLoggableWithEpilogue {
  @Logged(name = "Scheduler (logged via protobuf)")
  private final Scheduler m_scheduler = Scheduler.createIndependentScheduler();
}
