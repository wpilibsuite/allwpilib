// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.shuffleboard;

import edu.wpi.first.networktables.BooleanPublisher;
import edu.wpi.first.networktables.BooleanTopic;
import edu.wpi.first.networktables.GenericPublisher;
import edu.wpi.first.networktables.NetworkTable;
import java.util.function.BiConsumer;
import java.util.function.Supplier;

/**
 * A Shuffleboard widget whose value is provided by user code.
 *
 * @param <T> the type of values in the widget
 */
public final class SuppliedValueWidget<T> extends ShuffleboardWidget<SuppliedValueWidget<T>>
    implements AutoCloseable {
  private final String m_typeString;
  private final Supplier<T> m_supplier;
  private final BiConsumer<GenericPublisher, T> m_setter;
  private BooleanPublisher m_controllablePub;
  private GenericPublisher m_entry;

  /**
   * Package-private constructor for use by the Shuffleboard API.
   *
   * @param parent the parent container for the widget
   * @param title the title of the widget
   * @param typeString the NetworkTables string type
   * @param supplier the supplier for values to place in the NetworkTable entry
   * @param setter the function for placing values in the NetworkTable entry
   */
  SuppliedValueWidget(
      ShuffleboardContainer parent,
      String title,
      String typeString,
      Supplier<T> supplier,
      BiConsumer<GenericPublisher, T> setter) {
    super(parent, title);
    m_typeString = typeString;
    m_supplier = supplier;
    m_setter = setter;
  }

  @Override
  public void buildInto(NetworkTable parentTable, NetworkTable metaTable) {
    buildMetadata(metaTable);
    if (m_controllablePub == null) {
      m_controllablePub = new BooleanTopic(metaTable.getTopic("Controllable")).publish();
      m_controllablePub.set(false);
    }

    if (m_entry == null) {
      m_entry = parentTable.getTopic(getTitle()).genericPublish(m_typeString);
    }
    m_setter.accept(m_entry, m_supplier.get());
  }

  @Override
  public void close() {
    if (m_controllablePub != null) {
      m_controllablePub.close();
    }
    if (m_entry != null) {
      m_entry.close();
    }
  }
}
