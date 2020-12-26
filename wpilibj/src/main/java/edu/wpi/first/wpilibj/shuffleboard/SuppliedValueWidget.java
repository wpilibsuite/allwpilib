// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.shuffleboard;

import java.util.function.BiConsumer;
import java.util.function.Supplier;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;

/**
 * A Shuffleboard widget whose value is provided by user code.
 *
 * @param <T> the type of values in the widget
 */
public final class SuppliedValueWidget<T> extends ShuffleboardWidget<SuppliedValueWidget<T>> {
  private final Supplier<T> m_supplier;
  private final BiConsumer<NetworkTableEntry, T> m_setter;

  /**
   * Package-private constructor for use by the Shuffleboard API.
   *
   * @param parent   the parent container for the widget
   * @param title    the title of the widget
   * @param supplier the supplier for values to place in the NetworkTable entry
   * @param setter   the function for placing values in the NetworkTable entry
   */
  SuppliedValueWidget(ShuffleboardContainer parent,
                      String title,
                      Supplier<T> supplier,
                      BiConsumer<NetworkTableEntry, T> setter) {
    super(parent, title);
    this.m_supplier = supplier;
    this.m_setter = setter;
  }

  @Override
  public void buildInto(NetworkTable parentTable, NetworkTable metaTable) {
    buildMetadata(metaTable);
    metaTable.getEntry("Controllable").setBoolean(false);

    NetworkTableEntry entry = parentTable.getEntry(getTitle());
    m_setter.accept(entry, m_supplier.get());
  }
}
