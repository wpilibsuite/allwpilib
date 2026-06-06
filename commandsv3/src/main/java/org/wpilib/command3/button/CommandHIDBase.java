// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3.button;

import java.util.HashMap;
import java.util.Map;
import org.wpilib.command3.Scheduler;
import org.wpilib.command3.Trigger;
import org.wpilib.driverstation.GenericHID;
import org.wpilib.event.EventLoop;
import org.wpilib.math.util.Pair;

/** Minimal base class for generated command controller wrappers. */
public class CommandHIDBase {
  private final Scheduler m_scheduler;
  private final Map<EventLoop, Map<Integer, Trigger>> m_buttonCache = new HashMap<>();
  private final Map<EventLoop, Map<Pair<Integer, Double>, Trigger>> m_axisGreaterThanCache =
      new HashMap<>();

  protected CommandHIDBase(Scheduler scheduler) {
    m_scheduler = scheduler;
  }

  protected EventLoop getDefaultEventLoop() {
    return m_scheduler.getDefaultEventLoop();
  }

  protected Trigger button(GenericHID hid, int button, EventLoop loop) {
    var cache = m_buttonCache.computeIfAbsent(loop, k -> new HashMap<>());
    return cache.computeIfAbsent(
        button, k -> new Trigger(m_scheduler, loop, () -> hid.getRawButton(k)));
  }

  protected Trigger axisGreaterThan(GenericHID hid, int axis, double threshold, EventLoop loop) {
    var cache = m_axisGreaterThanCache.computeIfAbsent(loop, k -> new HashMap<>());
    return cache.computeIfAbsent(
        Pair.of(axis, threshold),
        k -> new Trigger(m_scheduler, loop, () -> hid.getRawAxis(axis) > threshold));
  }
}
