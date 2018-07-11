/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

import edu.wpi.first.networktables.NetworkTable;

/**
 * Abstract superclass for widgets.
 *
 * @param <W> the self type
 */
public abstract class ShuffleboardWidget<W extends ShuffleboardWidget<W>>
    extends ShuffleboardComponent<W> {

  ShuffleboardWidget(ShuffleboardContainer parent, String title) {
    super(parent, title);
  }

  protected final void buildMetadata(NetworkTable metaTable) {
    metaTable.getEntry("Title").forceSetString(getTitle());
    if (getType() == null) {
      metaTable.getEntry("Type").delete();
    } else {
      metaTable.getEntry("Type").forceSetString(getType());
    }
    if (getProperties() != null) {
      NetworkTable propTable = metaTable.getSubTable("Properties");
      getProperties().forEach((name, value) -> propTable.getEntry(name).setValue(value));
    }
  }

  /**
   * Sets the type of widget used to display the data. If not set, the default widget type will be
   * used.
   *
   * @param widgetType the type of the widget used to display the data
   * @return this widget object
   */
  public final W withWidget(String widgetType) {
    setType(widgetType);
    return (W) this;
  }

}
