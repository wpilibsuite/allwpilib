// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <concepts>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <wpi/StringMap.h>
#include <wpi/sendable/SendableBuilder.h>

#include "frc/smartdashboard/SendableChooserBase.h"

namespace frc {

/**
 * The SendableChooser class is a useful tool for presenting a selection of
 * options to the SmartDashboard.
 *
 * For instance, you may wish to be able to select between multiple autonomous
 * modes. You can do this by putting every possible Command you want to run as
 * an autonomous into a SendableChooser and then put it into the SmartDashboard
 * to have a list of options appear on the laptop. Once autonomous starts,
 * simply ask the SendableChooser what the selected value is.
 *
 * @tparam T The type of values to be stored
 * @see SmartDashboard
 */
template <class T>
  requires std::copy_constructible<T> && std::default_initializable<T>
class SendableChooser : public SendableChooserBase {
  wpi::StringMap<T> m_choices;
  std::function<void(T)> m_listener;
  template <class U>
  static U _unwrap_smart_ptr(const U& value) {
    return value;
  }

  template <class U>
  static std::weak_ptr<U> _unwrap_smart_ptr(const std::shared_ptr<U>& value) {
    return value;
  }

 public:
  using CopyType = decltype(_unwrap_smart_ptr(m_choices.lookup("")));

  SendableChooser() = default;
  ~SendableChooser() override = default;
  SendableChooser(SendableChooser&& rhs) = default;
  SendableChooser& operator=(SendableChooser&& rhs) = default;

  /**
   * Adds the given object to the list of options.
   *
   * On the SmartDashboard on the desktop, the object will appear as the given
   * name.
   *
   * @param name   the name of the option
   * @param object the option
   */
  void AddOption(std::string_view name, T object) {
    m_choices[name] = std::move(object);
  }

  /**
   * Add the given object to the list of options and marks it as the default.
   *
   * Functionally, this is very close to AddOption() except that it will use
   * this as the default option if none other is explicitly selected.
   *
   * @param name   the name of the option
   * @param object the option
   */
  void SetDefaultOption(std::string_view name, T object) {
    m_defaultChoice = name;
    AddOption(name, std::move(object));
  }

  /**
   * Returns a copy of the selected option (a std::weak_ptr<U> if T =
   * std::shared_ptr<U>).
   *
   * If there is none selected, it will return the default. If there is none
   * selected and no default, then it will return a value-initialized instance.
   * For integer types, this is 0. For container types like std::string, this is
   * an empty string.
   *
   * @return The option selected
   */
  CopyType GetSelected() const {
    std::string selected = m_defaultChoice;
    {
      std::scoped_lock lock(m_mutex);
      if (m_haveSelected) {
        selected = m_selected;
      }
    }
    if (selected.empty()) {
      return CopyType{};
    } else {
      return _unwrap_smart_ptr(m_choices.lookup(selected));
    }
  }

  /**
   * Bind a listener that's called when the selected value changes.
   * Only one listener can be bound. Calling this function will replace the
   * previous listener.
   * @param listener The function to call that accepts the new value
   */
  void OnChange(std::function<void(T)> listener) {
    std::scoped_lock lock(m_mutex);
    m_listener = listener;
  }

  void InitSendable(wpi::SendableBuilder& builder) override {
    builder.SetSmartDashboardType("String Chooser");
    builder.PublishConstInteger(kInstance, m_instance);
    builder.AddStringArrayProperty(
        kOptions,
        [=, this] {
          std::vector<std::string> keys;
          for (const auto& choice : m_choices) {
            keys.emplace_back(choice.first());
          }

          // Unlike std::map, wpi::StringMap elements
          // are not sorted
          std::sort(keys.begin(), keys.end());

          return keys;
        },
        nullptr);
    builder.AddSmallStringProperty(
        kDefault,
        [=, this](wpi::SmallVectorImpl<char>&) -> std::string_view {
          return m_defaultChoice;
        },
        nullptr);
    builder.AddSmallStringProperty(
        kActive,
        [=, this](wpi::SmallVectorImpl<char>& buf) -> std::string_view {
          std::scoped_lock lock(m_mutex);
          if (m_haveSelected) {
            buf.assign(m_selected.begin(), m_selected.end());
            return {buf.data(), buf.size()};
          } else {
            return m_defaultChoice;
          }
        },
        nullptr);
    builder.AddStringProperty(kSelected, nullptr,
                              [=, this](std::string_view val) {
                                T choice{};
                                std::function<void(T)> listener;
                                {
                                  std::scoped_lock lock(m_mutex);
                                  m_haveSelected = true;
                                  m_selected = val;
                                  if (m_previousVal != val && m_listener) {
                                    choice = m_choices[val];
                                    listener = m_listener;
                                  }
                                  m_previousVal = val;
                                }
                                if (listener) {
                                  listener(choice);
                                }
                              });
  }
};

}  // namespace frc
