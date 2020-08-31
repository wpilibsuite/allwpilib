/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

/**
 * Abstract superclass for widgets.
 *
 * <p>This class is package-private to minimize API surface area.
 *
 * @param <W> the self type
 */
abstract class ShuffleboardWidget<W extends ShuffleboardWidget<W>>
    extends ShuffleboardComponent<W> {

  ShuffleboardWidget(ShuffleboardContainer parent, String title) {
    super(parent, title);
  }

  /**
   * Sets the type of widget used to display the data. If not set, the default widget type will be
   * used.
   *
   * @param widgetType the type of the widget used to display the data
   * @return this widget object
   * @see BuiltInWidgets
   */
  public final W withWidget(WidgetType widgetType) {
    return withWidget(widgetType.getWidgetName());
  }

  /**
   * Sets the type of widget used to display the data. If not set, the default widget type will be
   * used. This method should only be used to use a widget that does not come built into
   * Shuffleboard (i.e. one that comes with a custom or third-party plugin). To use a widget that
   * is built into Shuffleboard, use {@link #withWidget(WidgetType)} and {@link BuiltInWidgets}.
   *
   * @param widgetType the type of the widget used to display the data
   * @return this widget object
   */
  public final W withWidget(String widgetType) {
    setType(widgetType);
    return (W) this;
  }

}
