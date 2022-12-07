// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command.button;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.networktables.BooleanSubscriber;
import edu.wpi.first.networktables.BooleanTopic;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableInstance;

/**
 * A {@link Button} that uses a {@link NetworkTable} boolean field.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
@SuppressWarnings("deprecation")
public class NetworkButton extends Button {
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
