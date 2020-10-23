/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

import java.util.List;
import java.util.NoSuchElementException;
import java.util.function.BooleanSupplier;
import java.util.function.DoubleSupplier;
import java.util.function.Supplier;

import edu.wpi.cscore.VideoSource;
import edu.wpi.first.wpilibj.Sendable;

/**
 * Common interface for objects that can contain shuffleboard components.
 */
public interface ShuffleboardContainer extends ShuffleboardValue {

  /**
   * Gets the components that are direct children of this container.
   */
  List<ShuffleboardComponent<?>> getComponents();

  /**
   * Gets the layout with the given type and title, creating it if it does not already exist at the
   * time this method is called. Note: this method should only be used to use a layout type that
   * is not already built into Shuffleboard. To use a layout built into Shuffleboard, use
   * {@link #getLayout(String, LayoutType)} and the layouts in {@link BuiltInLayouts}.
   *
   * @param title the title of the layout
   * @param type  the type of the layout, eg "List Layout" or "Grid Layout"
   * @return the layout
   * @see #getLayout(String, LayoutType)
   */
  ShuffleboardLayout getLayout(String title, String type);

  /**
   * Gets the layout with the given type and title, creating it if it does not already exist at the
   * time this method is called.
   *
   * @param title      the title of the layout
   * @param layoutType the type of the layout, eg "List" or "Grid"
   * @return the layout
   */
  default ShuffleboardLayout getLayout(String title, LayoutType layoutType) {
    return getLayout(title, layoutType.getLayoutName());
  }

  /**
   * Gets the already-defined layout in this container with the given title.
   *
   * <pre>{@code
   * Shuffleboard.getTab("Example Tab")
   *   .getLayout("My Layout", BuiltInLayouts.kList);
   *
   * // Later...
   * Shuffleboard.getTab("Example Tab")
   *   .getLayout("My Layout");
   * }</pre>
   *
   * @param title the title of the layout to get
   * @return the layout with the given title
   * @throws NoSuchElementException if no layout has yet been defined with the given title
   */
  ShuffleboardLayout getLayout(String title) throws NoSuchElementException;

  /**
   * Adds a widget to this container to display the given sendable.
   *
   * @param title    the title of the widget
   * @param sendable the sendable to display
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *                                  title
   */
  ComplexWidget add(String title, Sendable sendable) throws IllegalArgumentException;

  /**
   * Adds a widget to this container to display the given video stream.
   *
   * @param title the title of the widget
   * @param video the video stream to display
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *                                  title
   */
  default ComplexWidget add(String title, VideoSource video) throws IllegalArgumentException {
    return add(title, SendableCameraWrapper.wrap(video));
  }

  /**
   * Adds a widget to this container to display the given sendable.
   *
   * @param sendable the sendable to display
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *                                  title, or if the sendable's name has not been specified
   */
  ComplexWidget add(Sendable sendable);

  /**
   * Adds a widget to this container to display the given video stream.
   *
   * @param video the video to display
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this container with the same
   *                                  title as the video source
   */
  default ComplexWidget add(VideoSource video) {
    return add(SendableCameraWrapper.wrap(video));
  }

  /**
   * Adds a widget to this container to display the given data.
   *
   * @param title        the title of the widget
   * @param defaultValue the default value of the widget
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *                                  title
   * @see #addPersistent(String, Object) add(String title, Object defaultValue)
   */
  SimpleWidget add(String title, Object defaultValue) throws IllegalArgumentException;

  /**
   * Adds a widget to this container. The widget will display the data provided by the value
   * supplier. Changes made on the dashboard will not propagate to the widget object, and will be
   * overridden by values from the value supplier.
   *
   * @param title the title of the widget
   * @param valueSupplier the supplier for values
   * @return a widget to display data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *                                  title
   */
  SuppliedValueWidget<String> addString(String title, Supplier<String> valueSupplier)
      throws IllegalArgumentException;

  /**
   * Adds a widget to this container. The widget will display the data provided by the value
   * supplier. Changes made on the dashboard will not propagate to the widget object, and will be
   * overridden by values from the value supplier.
   *
   * @param title the title of the widget
   * @param valueSupplier the supplier for values
   * @return a widget to display data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *                                  title
   */
  SuppliedValueWidget<Double> addNumber(String title, DoubleSupplier valueSupplier)
      throws IllegalArgumentException;

  /**
   * Adds a widget to this container. The widget will display the data provided by the value
   * supplier. Changes made on the dashboard will not propagate to the widget object, and will be
   * overridden by values from the value supplier.
   *
   * @param title the title of the widget
   * @param valueSupplier the supplier for values
   * @return a widget to display data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *                                  title
   */
  SuppliedValueWidget<Boolean> addBoolean(String title, BooleanSupplier valueSupplier)
      throws IllegalArgumentException;

  /**
   * Adds a widget to this container. The widget will display the data provided by the value
   * supplier. Changes made on the dashboard will not propagate to the widget object, and will be
   * overridden by values from the value supplier.
   *
   * @param title the title of the widget
   * @param valueSupplier the supplier for values
   * @return a widget to display data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *                                  title
   */
  SuppliedValueWidget<String[]> addStringArray(String title, Supplier<String[]> valueSupplier)
      throws IllegalArgumentException;

  /**
   * Adds a widget to this container. The widget will display the data provided by the value
   * supplier. Changes made on the dashboard will not propagate to the widget object, and will be
   * overridden by values from the value supplier.
   *
   * @param title the title of the widget
   * @param valueSupplier the supplier for values
   * @return a widget to display data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *                                  title
   */
  SuppliedValueWidget<double[]> addDoubleArray(String title, Supplier<double[]> valueSupplier)
      throws IllegalArgumentException;

  /**
   * Adds a widget to this container. The widget will display the data provided by the value
   * supplier. Changes made on the dashboard will not propagate to the widget object, and will be
   * overridden by values from the value supplier.
   *
   * @param title the title of the widget
   * @param valueSupplier the supplier for values
   * @return a widget to display data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *                                  title
   */
  SuppliedValueWidget<boolean[]> addBooleanArray(String title, Supplier<boolean[]> valueSupplier)
      throws IllegalArgumentException;

  /**
   * Adds a widget to this container. The widget will display the data provided by the value
   * supplier. Changes made on the dashboard will not propagate to the widget object, and will be
   * overridden by values from the value supplier.
   *
   * @param title the title of the widget
   * @param valueSupplier the supplier for values
   * @return a widget to display data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *                                  title
   */
  SuppliedValueWidget<byte[]> addRaw(String title, Supplier<byte[]> valueSupplier)
      throws IllegalArgumentException;

  /**
   * Adds a widget to this container to display a simple piece of data. Unlike
   * {@link #add(String, Object)}, the value in the widget will be saved on the robot and will be
   * used when the robot program next starts rather than {@code defaultValue}.
   *
   * @param title        the title of the widget
   * @param defaultValue the default value of the widget
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *                                  title
   * @see #add(String, Object) add(String title, Object defaultValue)
   */
  default SimpleWidget addPersistent(String title, Object defaultValue)
      throws IllegalArgumentException {
    SimpleWidget widget = add(title, defaultValue);
    widget.getEntry().setPersistent();
    return widget;
  }

}
