/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <networktables/NetworkTableEntry.h>
#include <networktables/NetworkTableValue.h>
#include <wpi/ArrayRef.h>
#include <wpi/SmallSet.h>
#include <wpi/StringMap.h>
#include <wpi/Twine.h>

#include "frc/shuffleboard/ShuffleboardComponentBase.h"
#include "frc/shuffleboard/ShuffleboardValue.h"

namespace frc {

class ComplexWidget;
class Sendable;
class ShuffleboardLayout;
class SimpleWidget;

/**
 * Common interface for objects that can contain shuffleboard components.
 */
class ShuffleboardContainer : public virtual ShuffleboardValue {
 public:
  explicit ShuffleboardContainer(const wpi::Twine& title);

  ShuffleboardContainer(ShuffleboardContainer&& rhs) = default;

  virtual ~ShuffleboardContainer() = default;

  /**
   * Gets the components that are direct children of this container.
   */
  const std::vector<std::unique_ptr<ShuffleboardComponentBase>>& GetComponents()
      const;

  /**
   * Gets the layout with the given type and title, creating it if it does not
   * already exist at the time this method is called.
   *
   * @param type  the type of the layout, eg "List" or "Grid"
   * @param title the title of the layout
   * @return the layout
   */
  ShuffleboardLayout& GetLayout(const wpi::Twine& type,
                                const wpi::Twine& title);

  /**
   * Adds a widget to this container to display the given sendable.
   *
   * @param title    the title of the widget
   * @param sendable the sendable to display
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this
   * container with the given title
   */
  ComplexWidget& Add(const wpi::Twine& title, Sendable& sendable);

  /**
   * Adds a widget to this container to display the given sendable.
   *
   * @param sendable the sendable to display
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this
   * container with the given title, or if the sendable's name has not been
   * specified
   */
  ComplexWidget& Add(Sendable& sendable);

  /**
   * Adds a widget to this container to display the given data.
   *
   * @param title the title of the widget
   * @param defaultValue  the default value of the widget
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this
   *         container with the given title
   * @see #addPersistent(String, Object) add(String title, Object defaultValue)
   */
  SimpleWidget& Add(const wpi::Twine& title,
                    std::shared_ptr<nt::Value> defaultValue);

  /**
   * Adds a widget to this container to display the given data.
   *
   * @param title the title of the widget
   * @param defaultValue  the default value of the widget
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this
   *         container with the given title
   * @see #addPersistent(String, Object) add(String title, Object defaultValue)
   */
  SimpleWidget& Add(const wpi::Twine& title, bool defaultValue);

  /**
   * Adds a widget to this container to display the given data.
   *
   * @param title the title of the widget
   * @param defaultValue  the default value of the widget
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this
   *         container with the given title
   * @see #addPersistent(String, Object) add(String title, Object defaultValue)
   */
  SimpleWidget& Add(const wpi::Twine& title, double defaultValue);

  /**
   * Adds a widget to this container to display the given data.
   *
   * @param title the title of the widget
   * @param defaultValue  the default value of the widget
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this
   *         container with the given title
   * @see #addPersistent(String, Object) add(String title, Object defaultValue)
   */
  SimpleWidget& Add(const wpi::Twine& title, int defaultValue);

  /**
   * Adds a widget to this container to display the given data.
   *
   * @param title the title of the widget
   * @param defaultValue  the default value of the widget
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this
   *         container with the given title
   * @see #addPersistent(String, Object) add(String title, Object defaultValue)
   */
  SimpleWidget& Add(const wpi::Twine& title, const wpi::Twine& defaultValue);

  /**
   * Adds a widget to this container to display the given data.
   *
   * @param title the title of the widget
   * @param defaultValue  the default value of the widget
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this
   *         container with the given title
   * @see #addPersistent(String, Object) add(String title, Object defaultValue)
   */
  SimpleWidget& Add(const wpi::Twine& title, const char* defaultValue);

  /**
   * Adds a widget to this container to display the given data.
   *
   * @param title the title of the widget
   * @param defaultValue  the default value of the widget
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this
   *         container with the given title
   * @see #addPersistent(String, Object) add(String title, Object defaultValue)
   */
  SimpleWidget& Add(const wpi::Twine& title, wpi::ArrayRef<bool> defaultValue);

  /**
   * Adds a widget to this container to display the given data.
   *
   * @param title the title of the widget
   * @param defaultValue  the default value of the widget
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this
   *         container with the given title
   * @see #addPersistent(String, Object) add(String title, Object defaultValue)
   */
  SimpleWidget& Add(const wpi::Twine& title,
                    wpi::ArrayRef<double> defaultValue);

  /**
   * Adds a widget to this container to display the given data.
   *
   * @param title the title of the widget
   * @param defaultValue  the default value of the widget
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this
   *         container with the given title
   * @see #addPersistent(String, Object) add(String title, Object defaultValue)
   */
  SimpleWidget& Add(const wpi::Twine& title,
                    wpi::ArrayRef<std::string> defaultValue);

  /**
   * Adds a widget to this container to display a simple piece of data.
   *
   * Unlike {@link #add(String, Object)}, the value in the widget will be saved
   * on the robot and will be used when the robot program next starts rather
   * than {@code defaultValue}.
   *
   * @param title the title of the widget
   * @param defaultValue the default value of the widget
   * @return a widget to display the sendable data
   * @see #add(String, Object) add(String title, Object defaultValue)
   */
  SimpleWidget& AddPersistent(const wpi::Twine& title,
                              std::shared_ptr<nt::Value> defaultValue);

  /**
   * Adds a widget to this container to display a simple piece of data.
   *
   * Unlike {@link #add(String, Object)}, the value in the widget will be saved
   * on the robot and will be used when the robot program next starts rather
   * than {@code defaultValue}.
   *
   * @param title the title of the widget
   * @param defaultValue the default value of the widget
   * @return a widget to display the sendable data
   * @see #add(String, Object) add(String title, Object defaultValue)
   */
  SimpleWidget& AddPersistent(const wpi::Twine& title, bool defaultValue);

  /**
   * Adds a widget to this container to display a simple piece of data.
   *
   * Unlike {@link #add(String, Object)}, the value in the widget will be saved
   * on the robot and will be used when the robot program next starts rather
   * than {@code defaultValue}.
   *
   * @param title the title of the widget
   * @param defaultValue the default value of the widget
   * @return a widget to display the sendable data
   * @see #add(String, Object) add(String title, Object defaultValue)
   */
  SimpleWidget& AddPersistent(const wpi::Twine& title, double defaultValue);

  /**
   * Adds a widget to this container to display a simple piece of data.
   *
   * Unlike {@link #add(String, Object)}, the value in the widget will be saved
   * on the robot and will be used when the robot program next starts rather
   * than {@code defaultValue}.
   *
   * @param title the title of the widget
   * @param defaultValue the default value of the widget
   * @return a widget to display the sendable data
   * @see #add(String, Object) add(String title, Object defaultValue)
   */
  SimpleWidget& AddPersistent(const wpi::Twine& title, int defaultValue);

  /**
   * Adds a widget to this container to display a simple piece of data.
   *
   * Unlike {@link #add(String, Object)}, the value in the widget will be saved
   * on the robot and will be used when the robot program next starts rather
   * than {@code defaultValue}.
   *
   * @param title the title of the widget
   * @param defaultValue the default value of the widget
   * @return a widget to display the sendable data
   * @see #add(String, Object) add(String title, Object defaultValue)
   */
  SimpleWidget& AddPersistent(const wpi::Twine& title,
                              const wpi::Twine& defaultValue);

  /**
   * Adds a widget to this container to display a simple piece of data.
   *
   * Unlike {@link #add(String, Object)}, the value in the widget will be saved
   * on the robot and will be used when the robot program next starts rather
   * than {@code defaultValue}.
   *
   * @param title the title of the widget
   * @param defaultValue the default value of the widget
   * @return a widget to display the sendable data
   * @see #add(String, Object) add(String title, Object defaultValue)
   */
  SimpleWidget& AddPersistent(const wpi::Twine& title,
                              wpi::ArrayRef<bool> defaultValue);

  /**
   * Adds a widget to this container to display a simple piece of data.
   *
   * Unlike {@link #add(String, Object)}, the value in the widget will be saved
   * on the robot and will be used when the robot program next starts rather
   * than {@code defaultValue}.
   *
   * @param title the title of the widget
   * @param defaultValue the default value of the widget
   * @return a widget to display the sendable data
   * @see #add(String, Object) add(String title, Object defaultValue)
   */
  SimpleWidget& AddPersistent(const wpi::Twine& title,
                              wpi::ArrayRef<double> defaultValue);

  /**
   * Adds a widget to this container to display a simple piece of data.
   *
   * Unlike {@link #add(String, Object)}, the value in the widget will be saved
   * on the robot and will be used when the robot program next starts rather
   * than {@code defaultValue}.
   *
   * @param title the title of the widget
   * @param defaultValue the default value of the widget
   * @return a widget to display the sendable data
   * @see #add(String, Object) add(String title, Object defaultValue)
   */
  SimpleWidget& AddPersistent(const wpi::Twine& title,
                              wpi::ArrayRef<std::string> defaultValue);

  void EnableIfActuator() override;

  void DisableIfActuator() override;

 protected:
  bool m_isLayout = false;

 private:
  wpi::SmallSet<std::string, 32> m_usedTitles;
  std::vector<std::unique_ptr<ShuffleboardComponentBase>> m_components;
  wpi::StringMap<ShuffleboardLayout*> m_layouts;

  /**
   * Adds title to internal set if it hasn't already.
   *
   * @return True if title isn't in use; false otherwise.
   */
  void CheckTitle(const wpi::Twine& title);

  friend class SimpleWidget;
};

}  // namespace frc

// Make use of references returned by member functions usable
#include "frc/shuffleboard/ComplexWidget.h"
#include "frc/shuffleboard/ShuffleboardLayout.h"
#include "frc/shuffleboard/SimpleWidget.h"
