// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3.button;

import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import org.wpilib.command3.Trigger;
import org.wpilib.networktables.BooleanSubscriber;
import org.wpilib.networktables.BooleanTopic;
import org.wpilib.networktables.NetworkTable;
import org.wpilib.networktables.NetworkTableInstance;

/** A {@link Trigger} that uses a {@link NetworkTable} boolean field. */
public class NetworkButton extends Trigger {
  /**
   * Creates a NetworkButton that commands can be bound to.
   *
   * @param topic The boolean topic that contains the value.
   */
  public NetworkButton(BooleanTopic topic) {
    this(topic.subscribe(false));
  }

  /**
   * Creates a NetworkButton that commands can be bound to.
   *
   * @param sub The boolean subscriber that provides the value.
   */
  public NetworkButton(BooleanSubscriber sub) {
    super(() -> sub.getTopic().getInstance().isConnected() && sub.get());
    requireNonNullParam(sub, "sub", "NetworkButton");
  }

  /**
   * Creates a NetworkButton that commands can be bound to.
   *
   * @param table The table where the networktable value is located.
   * @param field The field that is the value.
   */
  public NetworkButton(NetworkTable table, String field) {
    this(table.getBooleanTopic(field));
  }

  /**
   * Creates a NetworkButton that commands can be bound to.
   *
   * @param table The table where the networktable value is located.
   * @param field The field that is the value.
   */
  public NetworkButton(String table, String field) {
    this(NetworkTableInstance.getDefault(), table, field);
  }

  /**
   * Creates a NetworkButton that commands can be bound to.
   *
   * @param inst The NetworkTable instance to use
   * @param table The table where the networktable value is located.
   * @param field The field that is the value.
   */
  public NetworkButton(NetworkTableInstance inst, String table, String field) {
    this(inst.getTable(table), field);
  }
}
