package edu.wpi.first.wpilibj.smartdashboard;

import edu.wpi.first.networktables.NetworkTable;
import java.util.HashMap;
import java.util.Map;

public abstract class MechanismObject2d {
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

  /**
   * Append a Mechanism object that is based on this one.
   *
   * @param object the object to add.
   * @return the object given as a parameter, useful for variable assignments and call chaining.
   * @throws UnsupportedOperationException if the object's name is already used - object names must
   *     be unique.
   */
  public final <T extends MechanismObject2d> T append(T object) {
    if (m_objects.containsKey(object.getName()))
      throw new UnsupportedOperationException("Mechanism object names must be unique!");
    m_objects.put(object.getName(), object);
    if (m_table != null) {
      object.update(m_table.getSubTable(object.getName()));
    }
    return object;
  }

  final void update(NetworkTable table) {
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

  @Override
  public boolean equals(Object obj) {
    // we don't really care about types. We want to make sure that names are unique
    return obj instanceof MechanismObject2d && ((MechanismObject2d) obj).m_name.equals(this.m_name);
  }
}
