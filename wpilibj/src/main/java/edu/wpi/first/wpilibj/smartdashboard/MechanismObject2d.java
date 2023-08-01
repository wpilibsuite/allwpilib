// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.smartdashboard;

import edu.wpi.first.networktables.NetworkTable;
import java.util.HashMap;
import java.util.Map;

/**
 * Common base class for all Mechanism2d node types.
 *
 * <p>To append another node, call {@link #append(MechanismObject2d)}. Objects that aren't appended
 * to a published {@link Mechanism2d} container are nonfunctional.
 *
 * @see Mechanism2d
 */
public abstract class MechanismObject2d implements AutoCloseable {
  /** Relative to parent. */
  private final String m_name;

  private NetworkTable m_table;
  private final Map<String, MechanismObject2d> m_objects = new HashMap<>(1);

  /**
   * Create a new Mechanism node object.
   *
   * @param name the node's name, must be unique.
   */
  protected MechanismObject2d(String name) {
    m_name = name;
  }

  @Override
  public void close() {
    for (MechanismObject2d obj : m_objects.values()) {
      obj.close();
    }
  }

  /**
   * Append a Mechanism object that is based on this one.
   *
   * @param <T> The object type.
   * @param object the object to add.
   * @return the object given as a parameter, useful for variable assignments and call chaining.
   * @throws UnsupportedOperationException if the object's name is already used - object names must
   *     be unique.
   */
  public final synchronized <T extends MechanismObject2d> T append(T object) {
    if (m_objects.containsKey(object.getName())) {
      throw new UnsupportedOperationException("Mechanism object names must be unique!");
    }
    m_objects.put(object.getName(), object);
    if (m_table != null) {
      object.update(m_table.getSubTable(object.getName()));
    }
    return object;
  }

  final synchronized void update(NetworkTable table) {
    m_table = table;
    updateEntries(m_table);
    for (MechanismObject2d obj : m_objects.values()) {
      obj.update(m_table.getSubTable(obj.m_name));
    }
  }

  /**
   * Update all entries with new ones from a new table.
   *
   * @param table the new table.
   */
  protected abstract void updateEntries(NetworkTable table);

  public final String getName() {
    return m_name;
  }
}
