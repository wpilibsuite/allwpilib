/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once
#include <utility>

#include "Trigger.h"

namespace frc2 {
class Command;
class Button : public Trigger {
 public:
  explicit Button(std::function<bool()> isPressed);

  Button() = default;

  Button WhenPressed(Command* command, bool interruptible = true);

  template <class T, typename = std::enable_if_t<std::is_base_of<
                         Command, std::remove_reference_t<T>>::value>>
  Button WhenPressed(T&& command, bool interruptible = true) {
    WhenActive(std::forward<T>(command), interruptible);
    return *this;
  }
  Button WhenPressed(std::function<void()> toRun);

  Button WhileHeld(Command* command, bool interruptible = true);

  template <class T, typename = std::enable_if_t<std::is_base_of<
                         Command, std::remove_reference_t<T>>::value>>
  Button WhileHeld(T&& command, bool interruptible = true) {
    WhileActiveContinous(std::forward<T>(command), interruptible);
    return *this;
  }
  
  Button WhileHeld(std::function<void()> toRun);

  Button WhenHeld(Command* command, bool interruptible = true);

  template <class T, typename = std::enable_if_t<std::is_base_of<
                         Command, std::remove_reference_t<T>>::value>>
  Button WhenHeld(T&& command, bool interruptible = true) {
    WhileActiveOnce(std::forward<T>(command), interruptible);
    return *this;
  }

  Button WhenReleased(Command* command, bool interruptible = true);

  template <class T, typename = std::enable_if_t<std::is_base_of<
                         Command, std::remove_reference_t<T>>::value>>
  Button WhenReleased(T&& command, bool interruptible = true) {
    WhenInactive(std::forward<T>(command), interruptible);
    return *this;
  }

  Button WhenReleased(std::function<void()> toRun);

  Button ToggleWhenPressed(Command* command, bool interruptible = true);

  template <class T, typename = std::enable_if_t<std::is_base_of<
                         Command, std::remove_reference_t<T>>::value>>
  Button ToggleWhenPressed(T&& command, bool interruptible = true) {
    ToggleWhenActive(std::forward<T>(command), interruptible);
    return *this;
  }

  Button CancelWhenPressed(Command* command);
};
}  // namespace frc2
