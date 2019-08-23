/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc2/command/Command.h>
#include <frc2/command/CommandScheduler.h>

#include <atomic>
#include <memory>
#include <utility>

namespace frc2 {
class Command;
class Trigger {
 public:
  explicit Trigger(std::function<bool()> isActive)
      : m_isActive{std::move(isActive)} {}

  Trigger() {
    m_isActive = [] { return false; };
  }

  Trigger(const Trigger& other);

  virtual bool Get() const { return m_isActive(); }

  Trigger WhenActive(Command* command, bool interruptible = true);
  template <class T, typename = std::enable_if_t<std::is_base_of<
                         Command, std::remove_reference_t<T>>::value>>
  Trigger WhenActive(T&& command, bool interruptible = true) {
    CommandScheduler::GetInstance().AddButton(
        [pressedLast = Get(), *this,
         command = std::make_unique<std::remove_reference_t<T>>(
             std::forward<T>(command)),
         interruptible]() mutable {
          bool pressed = Get();

          if (!pressedLast && pressed) {
            command->Schedule(interruptible);
          }

          pressedLast = pressed;
        });

    return *this;
  }
  Trigger WhenActive(std::function<void()> toRun);
  Trigger WhileActiveContinous(Command* command, bool interruptible = true);
  template <class T, typename = std::enable_if_t<std::is_base_of<
                         Command, std::remove_reference_t<T>>::value>>
  Trigger WhileActiveContinous(T&& command, bool interruptible = true) {
    CommandScheduler::GetInstance().AddButton(
        [pressedLast = Get(), *this,
         command = std::make_unique<std::remove_reference_t<T>>(
             std::forward<T>(command)),
         interruptible]() mutable {
          bool pressed = Get();

          if (pressed) {
            command->Schedule(interruptible);
          } else if (pressedLast && !pressed) {
            command->Cancel();
          }

          pressedLast = pressed;
        });
    return *this;
  }
  Trigger WhileActiveContinous(std::function<void()> toRun);
  Trigger WhileActiveOnce(Command* command, bool interruptible = true);
  template <class T, typename = std::enable_if_t<std::is_base_of<
                         Command, std::remove_reference_t<T>>::value>>
  Trigger WhileActiveOnce(T&& command, bool interruptible = true) {
    CommandScheduler::GetInstance().AddButton(
        [pressedLast = Get(), *this,
         command = std::make_unique<std::remove_reference_t<T>>(
             std::forward<T>(command)),
         interruptible]() mutable {
          bool pressed = Get();

          if (!pressedLast && pressed) {
            command->Schedule(interruptible);
          } else if (pressedLast && !pressed) {
            command->Cancel();
          }

          pressedLast = pressed;
        });
    return *this;
  }
  Trigger WhenInactive(Command* command, bool interruptible = true);
  template <class T, typename = std::enable_if_t<std::is_base_of<
                         Command, std::remove_reference_t<T>>::value>>
  Trigger WhenInactive(T&& command, bool interruptible = true) {
    CommandScheduler::GetInstance().AddButton(
        [pressedLast = Get(), *this,
         command = std::make_unique<std::remove_reference_t<T>>(
             std::forward<T>(command)),
         interruptible]() mutable {
          bool pressed = Get();

          if (pressedLast && !pressed) {
            command->Schedule(interruptible);
          }

          pressedLast = pressed;
        });
    return *this;
  }
  Trigger WhenInactive(std::function<void()> toRun);
  Trigger ToggleWhenActive(Command* command, bool interruptible = true);
  template <class T, typename = std::enable_if_t<std::is_base_of<
                         Command, std::remove_reference_t<T>>::value>>
  Trigger ToggleWhenActive(T&& command, bool interruptible = true) {
    CommandScheduler::GetInstance().AddButton(
        [pressedLast = Get(), *this,
         command = std::make_unique<std::remove_reference_t<T>>(
             std::forward<T>(command)),
         interruptible]() mutable {
          bool pressed = Get();

          if (!pressedLast && pressed) {
            if (command->IsScheduled()) {
              command->Cancel();
            } else {
              command->Schedule(interruptible);
            }
          }

          pressedLast = pressed;
        });
    return *this;
  }
  Trigger CancelWhenActive(Command* command);

  Trigger operator&&(Trigger rhs) {
    return Trigger([*this, rhs] { return Get() && rhs.Get(); });
  }

  Trigger operator||(Trigger rhs) {
    return Trigger([*this, rhs] { return Get() || rhs.Get(); });
  }

  Trigger operator!() {
    return Trigger([*this] { return !Get(); });
  }

 private:
  std::function<bool()> m_isActive;
};
}  // namespace frc2
