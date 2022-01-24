// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <networktables/NetworkTableEntry.h>
#include <networktables/NetworkTableValue.h>
#include <wpi/SmallSet.h>
#include <wpi/StringMap.h>
#include <wpi/span.h>

#include "frc/shuffleboard/BuiltInLayouts.h"
#include "frc/shuffleboard/LayoutType.h"
#include "frc/shuffleboard/ShuffleboardComponentBase.h"
#include "frc/shuffleboard/ShuffleboardValue.h"
#include "frc/shuffleboard/SuppliedValueWidget.h"

namespace cs {
class VideoSource;
}  // namespace cs

namespace wpi {
class Sendable;
}  // namespace wpi

namespace frc {

class ComplexWidget;
class ShuffleboardLayout;
class SimpleWidget;

/**
 * Common interface for objects that can contain shuffleboard components.
 */
class ShuffleboardContainer : public virtual ShuffleboardValue {
 public:
  explicit ShuffleboardContainer(std::string_view title);

  ShuffleboardContainer(ShuffleboardContainer&& rhs) = default;

  ~ShuffleboardContainer() override = default;

  /**
   * Gets the components that are direct children of this container.
   */
  const std::vector<std::unique_ptr<ShuffleboardComponentBase>>& GetComponents()
      const;

  /**
   * Gets the layout with the given type and title, creating it if it does not
   * already exist at the time this method is called.
   *
   * @param title the title of the layout
   * @param type  the type of the layout, eg "List" or "Grid"
   * @return the layout
   */
  ShuffleboardLayout& GetLayout(std::string_view title, BuiltInLayouts type);

  /**
   * Gets the layout with the given type and title, creating it if it does not
   * already exist at the time this method is called.
   *
   * @param title the title of the layout
   * @param type  the type of the layout, eg "List" or "Grid"
   * @return the layout
   */
  ShuffleboardLayout& GetLayout(std::string_view title, const LayoutType& type);

  /**
   * Gets the layout with the given type and title, creating it if it does not
   * already exist at the time this method is called. Note: this method should
   * only be used to use a layout type that is not already built into
   * Shuffleboard. To use a layout built into Shuffleboard, use
   * GetLayout(std::string_view, const LayoutType&) and the layouts in
   * BuiltInLayouts.
   *
   * @param title the title of the layout
   * @param type  the type of the layout, eg "List Layout" or "Grid Layout"
   * @return the layout
   * @see GetLayout(std::string_view, const LayoutType&)
   */
  ShuffleboardLayout& GetLayout(std::string_view title, std::string_view type);

  /**
   * Gets the already-defined layout in this container with the given title.
   *
   * <pre>{@code
   * Shuffleboard::GetTab("Example Tab")->getLayout("My Layout",
   * &BuiltInLayouts.kList);
   *
   * // Later...
   * Shuffleboard::GetTab("Example Tab")->GetLayout("My Layout");
   * }</pre>
   *
   * @param title the title of the layout to get
   * @return the layout with the given title
   * @throws if no layout has yet been defined with the given title
   */
  ShuffleboardLayout& GetLayout(std::string_view title);

  /**
   * Adds a widget to this container to display the given sendable.
   *
   * @param title    the title of the widget
   * @param sendable the sendable to display
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this
   * container with the given title
   */
  ComplexWidget& Add(std::string_view title, wpi::Sendable& sendable);

  /**
   * Adds a widget to this container to display the given video stream.
   *
   * @param title    the title of the widget
   * @param video    the video stream to display
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this
   * container with the given title
   */
  ComplexWidget& Add(std::string_view title, const cs::VideoSource& video);

  /**
   * Adds a widget to this container to display a video stream.
   *
   * @param title      the title of the widget
   * @param cameraName the name of the streamed camera
   * @param cameraUrls the URLs with which the dashboard can access the camera
   * stream
   * @return a widget to display the camera stream
   */
  ComplexWidget& AddCamera(std::string_view title, std::string_view cameraName,
                           wpi::span<const std::string> cameraUrls);

  /**
   * Adds a widget to this container to display the given sendable.
   *
   * @param sendable the sendable to display
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this
   * container with the given title, or if the sendable's name has not been
   * specified
   */
  ComplexWidget& Add(wpi::Sendable& sendable);

  /**
   * Adds a widget to this container to display the given video stream.
   *
   * @param video the video to display
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this
   * container with the same title as the video source
   */
  ComplexWidget& Add(const cs::VideoSource& video);

  /**
   * Adds a widget to this container to display the given data.
   *
   * @param title the title of the widget
   * @param defaultValue  the default value of the widget
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this
   *         container with the given title
   * @see AddPersistent(std::string_view, std::shared_ptr<nt::Value>)
   *      Add(std::string_view title, std::shared_ptr<nt::Value> defaultValue)
   */
  SimpleWidget& Add(std::string_view title,
                    std::shared_ptr<nt::Value> defaultValue);

  /**
   * Adds a widget to this container to display the given data.
   *
   * @param title the title of the widget
   * @param defaultValue  the default value of the widget
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this
   *         container with the given title
   * @see AddPersistent(std::string_view, bool)
   *      Add(std::string_view title, bool defaultValue)
   */
  SimpleWidget& Add(std::string_view title, bool defaultValue);

  /**
   * Adds a widget to this container to display the given data.
   *
   * @param title the title of the widget
   * @param defaultValue  the default value of the widget
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this
   *         container with the given title
   * @see AddPersistent(std::string_view, double)
   *      Add(std::string_view title, double defaultValue)
   */
  SimpleWidget& Add(std::string_view title, double defaultValue);

  /**
   * Adds a widget to this container to display the given data.
   *
   * @param title the title of the widget
   * @param defaultValue  the default value of the widget
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this
   *         container with the given title
   * @see AddPersistent(std::string_view, int)
   *      Add(std::string_view title, int defaultValue)
   */
  SimpleWidget& Add(std::string_view title, int defaultValue);

  /**
   * Adds a widget to this container to display the given data.
   *
   * @param title the title of the widget
   * @param defaultValue  the default value of the widget
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this
   *         container with the given title
   * @see AddPersistent(std::string_view, std::string_view)
   *      Add(std::string_view title, std::string_view defaultValue)
   */
  SimpleWidget& Add(std::string_view title, std::string_view defaultValue);

  /**
   * Adds a widget to this container to display the given data.
   *
   * @param title the title of the widget
   * @param defaultValue  the default value of the widget
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this
   *         container with the given title
   * @see AddPersistent(std::string_view, const char*)
   *      Add(std::string_view title, const char* defaultValue)
   */
  SimpleWidget& Add(std::string_view title, const char* defaultValue);

  /**
   * Adds a widget to this container to display the given data.
   *
   * @param title the title of the widget
   * @param defaultValue  the default value of the widget
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this
   *         container with the given title
   * @see AddPersistent(std::string_view, wpi::span<const bool>)
   *      Add(std::string_view title, wpi::span<const bool> defaultValue)
   */
  SimpleWidget& Add(std::string_view title, wpi::span<const bool> defaultValue);

  /**
   * Adds a widget to this container to display the given data.
   *
   * @param title the title of the widget
   * @param defaultValue  the default value of the widget
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this
   *         container with the given title
   * @see AddPersistent(std::string_view, wpi::span<const double>)
   *      Add(std::string_view title, wpi::span<const double> defaultValue)
   */
  SimpleWidget& Add(std::string_view title,
                    wpi::span<const double> defaultValue);

  /**
   * Adds a widget to this container to display the given data.
   *
   * @param title the title of the widget
   * @param defaultValue  the default value of the widget
   * @return a widget to display the sendable data
   * @throws IllegalArgumentException if a widget already exists in this
   *         container with the given title
   * @see AddPersistent(std::string_view, wpi::span<const std::string>)
   *      Add(std::string_view title, wpi::span<const std::string> defaultValue)
   */
  SimpleWidget& Add(std::string_view title,
                    wpi::span<const std::string> defaultValue);

  /**
   * Adds a widget to this container. The widget will display the data provided
   * by the value supplier. Changes made on the dashboard will not propagate to
   * the widget object, and will be overridden by values from the value
   * supplier.
   *
   * @param title the title of the widget
   * @param supplier the supplier for values
   * @return a widget to display data
   */
  SuppliedValueWidget<std::string>& AddString(
      std::string_view title, std::function<std::string()> supplier);

  /**
   * Adds a widget to this container. The widget will display the data provided
   * by the value supplier. Changes made on the dashboard will not propagate to
   * the widget object, and will be overridden by values from the value
   * supplier.
   *
   * @param title the title of the widget
   * @param supplier the supplier for values
   * @return a widget to display data
   */
  SuppliedValueWidget<double>& AddNumber(std::string_view title,
                                         std::function<double()> supplier);

  /**
   * Adds a widget to this container. The widget will display the data provided
   * by the value supplier. Changes made on the dashboard will not propagate to
   * the widget object, and will be overridden by values from the value
   * supplier.
   *
   * @param title the title of the widget
   * @param supplier the supplier for values
   * @return a widget to display data
   */
  SuppliedValueWidget<bool>& AddBoolean(std::string_view title,
                                        std::function<bool()> supplier);

  /**
   * Adds a widget to this container. The widget will display the data provided
   * by the value supplier. Changes made on the dashboard will not propagate to
   * the widget object, and will be overridden by values from the value
   * supplier.
   *
   * @param title the title of the widget
   * @param supplier the supplier for values
   * @return a widget to display data
   */
  SuppliedValueWidget<std::vector<std::string>>& AddStringArray(
      std::string_view title,
      std::function<std::vector<std::string>()> supplier);

  /**
   * Adds a widget to this container. The widget will display the data provided
   * by the value supplier. Changes made on the dashboard will not propagate to
   * the widget object, and will be overridden by values from the value
   * supplier.
   *
   * @param title the title of the widget
   * @param supplier the supplier for values
   * @return a widget to display data
   */
  SuppliedValueWidget<std::vector<double>>& AddNumberArray(
      std::string_view title, std::function<std::vector<double>()> supplier);

  /**
   * Adds a widget to this container. The widget will display the data provided
   * by the value supplier. Changes made on the dashboard will not propagate to
   * the widget object, and will be overridden by values from the value
   * supplier.
   *
   * @param title the title of the widget
   * @param supplier the supplier for values
   * @return a widget to display data
   */
  SuppliedValueWidget<std::vector<int>>& AddBooleanArray(
      std::string_view title, std::function<std::vector<int>()> supplier);

  /**
   * Adds a widget to this container. The widget will display the data provided
   * by the value supplier. Changes made on the dashboard will not propagate to
   * the widget object, and will be overridden by values from the value
   * supplier.
   *
   * @param title the title of the widget
   * @param supplier the supplier for values
   * @return a widget to display data
   */
  SuppliedValueWidget<std::string_view>& AddRaw(
      std::string_view title, std::function<std::string_view()> supplier);

  /**
   * Adds a widget to this container to display a simple piece of data.
   *
   * Unlike Add(std::string_view, std::shared_ptr<nt::Value>), the value in the
   * widget will be saved on the robot and will be used when the robot program
   * next starts rather than {@code defaultValue}.
   *
   * @param title the title of the widget
   * @param defaultValue the default value of the widget
   * @return a widget to display the sendable data
   * @see Add(stdd::string_view, std::shared_ptr<nt::Value>)
   *      Add(std::string_view title, std::shared_ptr<nt::Value> defaultValue)
   */
  SimpleWidget& AddPersistent(std::string_view title,
                              std::shared_ptr<nt::Value> defaultValue);

  /**
   * Adds a widget to this container to display a simple piece of data.
   *
   * Unlike Add(std::string_view, bool), the value in the widget will be saved
   * on the robot and will be used when the robot program next starts rather
   * than {@code defaultValue}.
   *
   * @param title the title of the widget
   * @param defaultValue the default value of the widget
   * @return a widget to display the sendable data
   * @see Add(std::string_view, bool)
   *      Add(std::string_view title, bool defaultValue)
   */
  SimpleWidget& AddPersistent(std::string_view title, bool defaultValue);

  /**
   * Adds a widget to this container to display a simple piece of data.
   *
   * Unlike Add(std::string_view, double), the value in the widget will be saved
   * on the robot and will be used when the robot program next starts rather
   * than {@code defaultValue}.
   *
   * @param title the title of the widget
   * @param defaultValue the default value of the widget
   * @return a widget to display the sendable data
   * @see Add(std::string_view, double)
   *      Add(std::string_view title, double defaultValue)
   */
  SimpleWidget& AddPersistent(std::string_view title, double defaultValue);

  /**
   * Adds a widget to this container to display a simple piece of data.
   *
   * Unlike Add(std::string_view, int), the value in the widget will be saved on
   * the robot and will be used when the robot program next starts rather than
   * {@code defaultValue}.
   *
   * @param title the title of the widget
   * @param defaultValue the default value of the widget
   * @return a widget to display the sendable data
   * @see Add(std:string_view, int)
   *      Add(std::string_view title, int defaultValue)
   */
  SimpleWidget& AddPersistent(std::string_view title, int defaultValue);

  /**
   * Adds a widget to this container to display a simple piece of data.
   *
   * Unlike Add(std::string_view, std::string_view), the value in the widget
   * will be saved on the robot and will be used when the robot program next
   * starts rather than {@code defaultValue}.
   *
   * @param title the title of the widget
   * @param defaultValue the default value of the widget
   * @return a widget to display the sendable data
   * @see Add(std::string_view, std::string_view)
   *      Add(std::string_view title, std::string_view defaultValue)
   */
  SimpleWidget& AddPersistent(std::string_view title,
                              std::string_view defaultValue);

  /**
   * Adds a widget to this container to display a simple piece of data.
   *
   * Unlike Add(std::string_view, wpi::span<const bool>), the value in the
   * widget will be saved on the robot and will be used when the robot program
   * next starts rather than {@code defaultValue}.
   *
   * @param title the title of the widget
   * @param defaultValue the default value of the widget
   * @return a widget to display the sendable data
   * @see Add(std::string_view, wpi::span<const bool>)
   *      Add(std::string_view title, wpi::span<const bool> defaultValue)
   */
  SimpleWidget& AddPersistent(std::string_view title,
                              wpi::span<const bool> defaultValue);

  /**
   * Adds a widget to this container to display a simple piece of data.
   *
   * Unlike Add(std::string_view, wpi::span<const double>), the value in the
   * widget will be saved on the robot and will be used when the robot program
   * next starts rather than {@code defaultValue}.
   *
   * @param title the title of the widget
   * @param defaultValue the default value of the widget
   * @return a widget to display the sendable data
   * @see Add(std::string_view, wpi::span<const double>)
   *      Add(std::string_view title, wpi::span<const double> defaultValue)
   */
  SimpleWidget& AddPersistent(std::string_view title,
                              wpi::span<const double> defaultValue);

  /**
   * Adds a widget to this container to display a simple piece of data.
   *
   * Unlike Add(std::string_view, wpi::span<const std::string>), the value in
   * the widget will be saved on the robot and will be used when the robot
   * program next starts rather than {@code defaultValue}.
   *
   * @param title the title of the widget
   * @param defaultValue the default value of the widget
   * @return a widget to display the sendable data
   * @see Add(std::string_view, wpi::span<const std::string>)
   *      Add(std::string_view title, wpi::span<const std::string> defaultValue)
   */
  SimpleWidget& AddPersistent(std::string_view title,
                              wpi::span<const std::string> defaultValue);

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
  void CheckTitle(std::string_view title);

  friend class SimpleWidget;
};

}  // namespace frc

// Make use of references returned by member functions usable
#include "frc/shuffleboard/ComplexWidget.h"
#include "frc/shuffleboard/ShuffleboardLayout.h"
#include "frc/shuffleboard/SimpleWidget.h"

#ifndef DYNAMIC_CAMERA_SERVER
#include "frc/shuffleboard/SendableCameraWrapper.h"

inline frc::ComplexWidget& frc::ShuffleboardContainer::Add(
    const cs::VideoSource& video) {
  return Add(frc::SendableCameraWrapper::Wrap(video));
}

inline frc::ComplexWidget& frc::ShuffleboardContainer::Add(
    std::string_view title, const cs::VideoSource& video) {
  return Add(title, frc::SendableCameraWrapper::Wrap(video));
}

inline frc::ComplexWidget& frc::ShuffleboardContainer::AddCamera(
    std::string_view title, std::string_view cameraName,
    wpi::span<const std::string> cameraUrls) {
  return Add(title, frc::SendableCameraWrapper::Wrap(cameraName, cameraUrls));
}
#endif
