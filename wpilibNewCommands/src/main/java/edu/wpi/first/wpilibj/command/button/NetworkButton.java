// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.command.button;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;

/**
 * A {@link Button} that uses a {@link NetworkTable} boolean field.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public class NetworkButton extends Button {
  /**
   * Creates a NetworkButton that commands can be bound to.
   *
   * @param entry The entry that is the value.
   */
  public NetworkButton(NetworkTableEntry entry) {
    super(() -> entry.getInstance().isConnected() && entry.getBoolean(false));
    requireNonNullParam(entry, "entry", "NetworkButton");
  }

  /**
   * Creates a NetworkButton that commands can be bound to.
   *
   * @param table The table where the networktable value is located.
   * @param field The field that is the value.
   */
  public NetworkButton(NetworkTable table, String field) {
    this(table.getEntry(field));
  }

  /**
   * Creates a NetworkButton that commands can be bound to.
   *
   * @param table The table where the networktable value is located.
   * @param field The field that is the value.
   */
  public NetworkButton(String table, String field) {
    this(NetworkTableInstance.getDefault().getTable(table), field);
  }
}
