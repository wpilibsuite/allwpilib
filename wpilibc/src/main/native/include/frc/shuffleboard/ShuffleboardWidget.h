/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <wpi/Twine.h>

#include "frc/shuffleboard/BuiltInWidgets.h"
#include "frc/shuffleboard/ShuffleboardComponent.h"
#include "frc/shuffleboard/WidgetType.h"

namespace frc {

class ShuffleboardContainer;

namespace detail {
const char* GetStringForWidgetType(BuiltInWidgets type);
}  // namespace detail

/**
 * Abstract superclass for widgets.
 *
 * <p>This class is package-private to minimize API surface area.
 *
 * @tparam Derived the self type
 */
template <typename Derived>
class ShuffleboardWidget : public ShuffleboardComponent<Derived> {
 public:
  ShuffleboardWidget(ShuffleboardContainer& parent, const wpi::Twine& title)
      : ShuffleboardValue(title),
        ShuffleboardComponent<Derived>(parent, title) {}

  /**
   * Sets the type of widget used to display the data. If not set, the default
   * widget type will be used.
   *
   * @param widgetType the type of the widget used to display the data
   * @return this widget object
   * @see BuiltInWidgets
   */
  Derived& WithWidget(BuiltInWidgets widgetType) {
    return WithWidget(detail::GetStringForWidgetType(widgetType));
  }

  /**
   * Sets the type of widget used to display the data. If not set, the default
   * widget type will be used.
   *
   * @param widgetType the type of the widget used to display the data
   * @return this widget object
   */
  Derived& WithWidget(const WidgetType& widgetType) {
    return WithWidget(widgetType.GetWidgetName());
  }

  /**
   * Sets the type of widget used to display the data. If not set, the default
   * widget type will be used. This method should only be used to use a widget
   * that does not come built into Shuffleboard (i.e. one that comes with a
   * custom or thrid-party plugin). To use a widget that is built into
   * Shuffleboard, use {@link #withWidget(WidgetType)} and {@link
   * BuiltInWidgets}.
   *
   * @param widgetType the type of the widget used to display the data
   * @return this widget object
   */
  Derived& WithWidget(const wpi::Twine& widgetType) {
    this->SetType(widgetType);
    return *static_cast<Derived*>(this);
  }
};

}  // namespace frc
