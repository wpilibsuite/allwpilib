// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.shuffleboard;

import edu.wpi.first.cscore.VideoSource;
import edu.wpi.first.networktables.NetworkTableType;
import edu.wpi.first.util.function.FloatSupplier;
import edu.wpi.first.util.sendable.Sendable;
import java.util.List;
import java.util.NoSuchElementException;
import java.util.function.BooleanSupplier;
import java.util.function.DoubleSupplier;
import java.util.function.LongSupplier;
import java.util.function.Supplier;

/** Common interface for objects that can contain shuffleboard components. */
public sealed interface ShuffleboardContainer extends ShuffleboardValue
    permits ShuffleboardLayout, ShuffleboardTab {
  /**
   * Gets the components that are direct children of this container.
   *
   * @return The components that are direct children of this container.
   */
  List<ShuffleboardComponent<?>> getComponents();

  /**
   * Gets the layout with the given type and title, creating it if it does not already exist at the
   * time this method is called. Note: this method should only be used to use a layout type that is
   * not already built into Shuffleboard. To use a layout built into Shuffleboard, use {@link
   * #getLayout(String, LayoutType)} and the layouts in {@link BuiltInLayouts}.
   *
   * @param title the title of the layout
   * @param type the type of the layout, eg "List Layout" or "Grid Layout"
   * @return the layout
   * @see #getLayout(String, LayoutType)
   */
  ShuffleboardLayout getLayout(String title, String type);

  /**
   * Gets the layout with the given type and title, creating it if it does not already exist at the
   * time this method is called.
   *
   * @param title the title of the layout
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
  ShuffleboardLayout getLayout(String title);

  /**
   * Adds a widget to this container to display the given sendable.
   *
   * @param title the title of the widget
   * @param sendable the sendable to display
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *     title
   */
  ComplexWidget add(String title, Sendable sendable);

  /**
   * Adds a widget to this container to display the given video stream.
   *
   * @param title the title of the widget
   * @param video the video stream to display
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *     title
   */
  default ComplexWidget add(String title, VideoSource video) {
    return add(title, SendableCameraWrapper.wrap(video));
  }

  /**
   * Adds a widget to this container to display the given sendable.
   *
   * @param sendable the sendable to display
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *     title, or if the sendable's name has not been specified
   */
  ComplexWidget add(Sendable sendable);

  /**
   * Adds a widget to this container to display the given video stream.
   *
   * @param video the video to display
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this container with the same
   *     title as the video source
   */
  default ComplexWidget add(VideoSource video) {
    return add(SendableCameraWrapper.wrap(video));
  }

  /**
   * Adds a widget to this container to display the given data.
   *
   * @param title the title of the widget
   * @param defaultValue the default value of the widget
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *     title
   * @see #addPersistent(String, Object) add(String title, Object defaultValue)
   */
  SimpleWidget add(String title, Object defaultValue);

  /**
   * Adds a widget to this container to display the given data.
   *
   * @param title the title of the widget
   * @param typeString the NT type string
   * @param defaultValue the default value of the widget
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *     title
   * @see #addPersistent(String, Object) add(String title, Object defaultValue)
   */
  SimpleWidget add(String title, String typeString, Object defaultValue);

  /**
   * Adds a widget to this container to display a video stream.
   *
   * @param title the title of the widget
   * @param cameraName the name of the streamed camera
   * @param cameraUrls the URLs with which the dashboard can access the camera stream
   * @return a widget to display the camera stream
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *     title
   */
  default ComplexWidget addCamera(String title, String cameraName, String... cameraUrls) {
    return add(title, SendableCameraWrapper.wrap(cameraName, cameraUrls));
  }

  /**
   * Adds a widget to this container. The widget will display the data provided by the value
   * supplier. Changes made on the dashboard will not propagate to the widget object, and will be
   * overridden by values from the value supplier.
   *
   * @param title the title of the widget
   * @param valueSupplier the supplier for values
   * @return a widget to display data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *     title
   */
  SuppliedValueWidget<String> addString(String title, Supplier<String> valueSupplier);

  /**
   * Adds a widget to this container. The widget will display the data provided by the value
   * supplier. Changes made on the dashboard will not propagate to the widget object, and will be
   * overridden by values from the value supplier.
   *
   * @param title the title of the widget
   * @param valueSupplier the supplier for values
   * @return a widget to display data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *     title
   */
  SuppliedValueWidget<Double> addNumber(String title, DoubleSupplier valueSupplier);

  /**
   * Adds a widget to this container. The widget will display the data provided by the value
   * supplier. Changes made on the dashboard will not propagate to the widget object, and will be
   * overridden by values from the value supplier.
   *
   * @param title the title of the widget
   * @param valueSupplier the supplier for values
   * @return a widget to display data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *     title
   */
  SuppliedValueWidget<Double> addDouble(String title, DoubleSupplier valueSupplier);

  /**
   * Adds a widget to this container. The widget will display the data provided by the value
   * supplier. Changes made on the dashboard will not propagate to the widget object, and will be
   * overridden by values from the value supplier.
   *
   * @param title the title of the widget
   * @param valueSupplier the supplier for values
   * @return a widget to display data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *     title
   */
  SuppliedValueWidget<Float> addFloat(String title, FloatSupplier valueSupplier);

  /**
   * Adds a widget to this container. The widget will display the data provided by the value
   * supplier. Changes made on the dashboard will not propagate to the widget object, and will be
   * overridden by values from the value supplier.
   *
   * @param title the title of the widget
   * @param valueSupplier the supplier for values
   * @return a widget to display data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *     title
   */
  SuppliedValueWidget<Long> addInteger(String title, LongSupplier valueSupplier);

  /**
   * Adds a widget to this container. The widget will display the data provided by the value
   * supplier. Changes made on the dashboard will not propagate to the widget object, and will be
   * overridden by values from the value supplier.
   *
   * @param title the title of the widget
   * @param valueSupplier the supplier for values
   * @return a widget to display data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *     title
   */
  SuppliedValueWidget<Boolean> addBoolean(String title, BooleanSupplier valueSupplier);

  /**
   * Adds a widget to this container. The widget will display the data provided by the value
   * supplier. Changes made on the dashboard will not propagate to the widget object, and will be
   * overridden by values from the value supplier.
   *
   * @param title the title of the widget
   * @param valueSupplier the supplier for values
   * @return a widget to display data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *     title
   */
  SuppliedValueWidget<String[]> addStringArray(String title, Supplier<String[]> valueSupplier);

  /**
   * Adds a widget to this container. The widget will display the data provided by the value
   * supplier. Changes made on the dashboard will not propagate to the widget object, and will be
   * overridden by values from the value supplier.
   *
   * @param title the title of the widget
   * @param valueSupplier the supplier for values
   * @return a widget to display data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *     title
   */
  SuppliedValueWidget<double[]> addDoubleArray(String title, Supplier<double[]> valueSupplier);

  /**
   * Adds a widget to this container. The widget will display the data provided by the value
   * supplier. Changes made on the dashboard will not propagate to the widget object, and will be
   * overridden by values from the value supplier.
   *
   * @param title the title of the widget
   * @param valueSupplier the supplier for values
   * @return a widget to display data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *     title
   */
  SuppliedValueWidget<float[]> addFloatArray(String title, Supplier<float[]> valueSupplier);

  /**
   * Adds a widget to this container. The widget will display the data provided by the value
   * supplier. Changes made on the dashboard will not propagate to the widget object, and will be
   * overridden by values from the value supplier.
   *
   * @param title the title of the widget
   * @param valueSupplier the supplier for values
   * @return a widget to display data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *     title
   */
  SuppliedValueWidget<long[]> addIntegerArray(String title, Supplier<long[]> valueSupplier);

  /**
   * Adds a widget to this container. The widget will display the data provided by the value
   * supplier. Changes made on the dashboard will not propagate to the widget object, and will be
   * overridden by values from the value supplier.
   *
   * @param title the title of the widget
   * @param valueSupplier the supplier for values
   * @return a widget to display data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *     title
   */
  SuppliedValueWidget<boolean[]> addBooleanArray(String title, Supplier<boolean[]> valueSupplier);

  /**
   * Adds a widget to this container. The widget will display the data provided by the value
   * supplier. Changes made on the dashboard will not propagate to the widget object, and will be
   * overridden by values from the value supplier.
   *
   * @param title the title of the widget
   * @param valueSupplier the supplier for values
   * @return a widget to display data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *     title
   */
  default SuppliedValueWidget<byte[]> addRaw(String title, Supplier<byte[]> valueSupplier) {
    return addRaw(title, "raw", valueSupplier);
  }

  /**
   * Adds a widget to this container. The widget will display the data provided by the value
   * supplier. Changes made on the dashboard will not propagate to the widget object, and will be
   * overridden by values from the value supplier.
   *
   * @param title the title of the widget
   * @param typeString the NT type string for the value
   * @param valueSupplier the supplier for values
   * @return a widget to display data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *     title
   */
  SuppliedValueWidget<byte[]> addRaw(
      String title, String typeString, Supplier<byte[]> valueSupplier);

  /**
   * Adds a widget to this container to display a simple piece of data. Unlike {@link #add(String,
   * Object)}, the value in the widget will be saved on the robot and will be used when the robot
   * program next starts rather than {@code defaultValue}.
   *
   * @param title the title of the widget
   * @param defaultValue the default value of the widget
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *     title
   * @see #add(String, Object) add(String title, Object defaultValue)
   */
  default SimpleWidget addPersistent(String title, Object defaultValue) {
    return addPersistent(title, NetworkTableType.getStringFromObject(defaultValue), defaultValue);
  }

  /**
   * Adds a widget to this container to display a simple piece of data. Unlike {@link #add(String,
   * Object)}, the value in the widget will be saved on the robot and will be used when the robot
   * program next starts rather than {@code defaultValue}.
   *
   * @param title the title of the widget
   * @param typeString the NT type string
   * @param defaultValue the default value of the widget
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *     title
   * @see #add(String, Object) add(String title, Object defaultValue)
   */
  default SimpleWidget addPersistent(String title, String typeString, Object defaultValue) {
    SimpleWidget widget = add(title, defaultValue);
    widget.getEntry(typeString).getTopic().setPersistent(true);
    return widget;
  }
}
