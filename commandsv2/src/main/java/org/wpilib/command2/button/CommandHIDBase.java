// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command2.button;

import java.util.HashMap;
import java.util.Map;
import org.wpilib.driverstation.GenericHID;
import org.wpilib.event.EventLoop;
import org.wpilib.math.util.Pair;

/** Minimal base class for generated command controller wrappers. */
public class CommandHIDBase {
  private final Map<EventLoop, Map<Integer, Trigger>> m_buttonCache = new HashMap<>();
  private final Map<EventLoop, Map<Pair<Integer, Double>, Trigger>> m_axisGreaterThanCache =
      new HashMap<>();

  /** Constructs a CommandHIDBase. */
  protected CommandHIDBase() {}

  /**
   * Returns a trigger that is active while the given button is pressed.
   *
   * @param hid the HID to read from
   * @param button the button index
   * @param loop the event loop to poll on
   * @return a trigger for the button
   */
  protected Trigger button(GenericHID hid, int button, EventLoop loop) {
    var cache = m_buttonCache.computeIfAbsent(loop, k -> new HashMap<>());
    return cache.computeIfAbsent(button, k -> new Trigger(loop, () -> hid.getRawButton(k)));
  }

  /**
   * Returns a trigger that is active while the given axis is greater than the threshold.
   *
   * @param hid the HID to read from
   * @param axis the axis index
   * @param threshold the axis threshold
   * @param loop the event loop to poll on
   * @return a trigger for the axis threshold
   */
  protected Trigger axisGreaterThan(GenericHID hid, int axis, double threshold, EventLoop loop) {
    var cache = m_axisGreaterThanCache.computeIfAbsent(loop, k -> new HashMap<>());
    return cache.computeIfAbsent(
        Pair.of(axis, threshold), k -> new Trigger(loop, () -> hid.getRawAxis(axis) > threshold));
  }
}
