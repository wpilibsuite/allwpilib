/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

import java.util.Map;
import java.util.Objects;

import edu.wpi.first.networktables.NetworkTable;

/**
 * A generic component in Shuffleboard.
 *
 * @param <C> the self type
 */
public abstract class ShuffleboardComponent<C extends ShuffleboardComponent<C>>
    implements ShuffleboardValue {
  private final ShuffleboardContainer m_parent;
  private final String m_title;
  private String m_type;
  private Map<String, Object> m_properties;
  private boolean m_metadataDirty = false; // NOPMD redundant field initializer

  protected ShuffleboardComponent(ShuffleboardContainer parent, String title, String type) {
    m_parent = Objects.requireNonNull(parent, "Parent cannot be null");
    m_title = Objects.requireNonNull(title, "Title cannot be null");
    m_type = type;
  }

  protected ShuffleboardComponent(ShuffleboardContainer parent, String title) {
    this(parent, title, null);
  }

  public final ShuffleboardContainer getParent() {
    return m_parent;
  }

  protected final void setType(String type) {
    m_type = type;
    m_metadataDirty = true;
  }

  public final String getType() {
    return m_type;
  }

  @Override
  public final String getTitle() {
    return m_title;
  }

  /**
   * Gets the custom properties for this component. May be null.
   */
  final Map<String, Object> getProperties() {
    return m_properties;
  }

  /**
   * Sets custom properties for this component. Property names are case- and whitespace-insensitive
   * (capitalization and spaces do not matter).
   *
   * @param properties the properties for this component
   * @return this component
   */
  public final C withProperties(Map<String, Object> properties) {
    m_properties = properties;
    m_metadataDirty = true;
    return (C) this;
  }

  protected final void buildMetadata(NetworkTable metaTable) {
    if (!m_metadataDirty) {
      return;
    }
    if (getType() == null) {
      metaTable.getEntry("PreferredComponent").delete();
    } else {
      metaTable.getEntry("PreferredComponent").forceSetString(getType());
    }
    if (getProperties() != null) {
      NetworkTable propTable = metaTable.getSubTable("Properties");
      getProperties().forEach((name, value) -> propTable.getEntry(name).setValue(value));
    }
    m_metadataDirty = false;
  }

}
