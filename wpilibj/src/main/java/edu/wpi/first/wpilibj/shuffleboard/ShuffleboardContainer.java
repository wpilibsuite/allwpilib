/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

import java.util.List;

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
   * time this method is called.
   *
   * @param type  the type of the layout, eg "List Layout" or "Grid Layout"
   * @param title the title of the layout
   * @return the layout
   * @see #getLayout(LayoutType, String)
   */
  ShuffleboardLayout getLayout(String type, String title);


  /**
   * Gets the layout with the given type and title, creating it if it does not already exist at the
   * time this method is called.
   *
   * @param type  the type of the layout, eg "List" or "Grid"
   * @param title the title of the layout
   * @return the layout
   */
  default ShuffleboardLayout getLayout(LayoutType layoutType, String title) {
    return getLayout(layoutType.getLayoutName(), title);
  }

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
   * Adds a widget to this container to display the given sendable.
   *
   * @param sendable the sendable to display
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this container with the given
   *                                  title, or if the sendable's name has not been specified
   */
  ComplexWidget add(Sendable sendable);

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
   * Adds a widget to this container to display a simple piece of data. Unlike
   * {@link #add(String, Object)}, the value in the widget will be saved on the robot and will be
   * used when the robot program next starts rather than {@code defaultValue}.
   *
   * @param title the title of the widget
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
