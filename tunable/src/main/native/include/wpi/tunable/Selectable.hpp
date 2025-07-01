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

#include "wpi/tunable/ComplexTunable.hpp"
#include "wpi/tunable/Tunable.hpp"
#include "wpi/util/StringMap.hpp"

namespace wpi {

namespace detail {
class SelectableBase : public wpi::ComplexTunable {
 public:
  SelectableBase();

  SelectableBase(const SelectableBase& oth) = delete;
  SelectableBase(SelectableBase&& oth) = default;
  SelectableBase& operator=(const SelectableBase& oth) = delete;
  SelectableBase& operator=(SelectableBase&& oth) = default;

  void PublishTunable(wpi::TunableTable& table) override;
  std::string_view GetTunableType() const override;

 protected:
  virtual void Changed(std::string_view val) = 0;

  wpi::Tunable<std::string> m_defaultChoice;
  wpi::Tunable<std::vector<std::string>> m_options;
  wpi::Tunable<std::string> m_selected;
};
}  // namespace detail

/**
 * The Selectable class is a useful tool for presenting a selection of
 * options on a dashboard.
 *
 * @tparam T The type of values to be stored
 */
template <class T>
  requires std::copy_constructible<T> && std::default_initializable<T>
class Selectable final : public detail::SelectableBase {
  wpi::util::StringMap<T> m_map;
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
  using CopyType = decltype(_unwrap_smart_ptr(m_map.find("")->second));

  Selectable() = default;
  Selectable(Selectable&& rhs) = default;
  Selectable& operator=(Selectable&& rhs) = default;

  /**
   * Adds the given object to the list of options.
   *
   * On the dashboard, the object will appear as the given name.
   *
   * @param name   the name of the option
   * @param object the option
   */
  void Add(std::string_view name, T object) {
    if (!m_map.insert_or_assign(name, std::move(object)).second) {
      std::erase_if(m_options.Mutate(), [name](const std::string& option) {
        return option == name;
      });
    }
    m_options.Mutate().emplace_back(name);
  }

  /**
   * Adds the given object to the list of options and marks it as the default.
   *
   * @param name   the name of the option
   * @param object the option
   */
  void AddDefault(std::string_view name, T object) {
    Add(name, std::move(object));
    SetDefault(name);
  }

  /**
   * Marks the given option as the default.
   *
   * @param name   the name of the option
   */
  void SetDefault(std::string_view name) { m_defaultChoice = name; }

  /**
   * Clears the list of options and resets the default. Does not change the
   * selected option; GetSelected() will return a value-initialized instance
   * after this is called until a new matching option is added.
   */
  void Clear() {
    m_map.clear();
    m_options.Mutate().clear();
    m_defaultChoice = "";
  }

  /**
   * Returns a copy of the selected option (a std::weak_ptr&lt;U&gt; if T =
   * std::shared_ptr&lt;U&gt;).
   *
   * If there is none selected, it will return the default. If there is none
   * selected and no default, then it will return a value-initialized instance.
   * For integer types, this is 0. For container types like std::string, this is
   * an empty string.
   *
   * @return The option selected
   */
  CopyType GetSelected() const {
    const std::string& selected =
        !m_selected.Get().empty() ? m_selected : m_defaultChoice;
    if (selected.empty()) {
      return CopyType{};
    }

    auto it = m_map.find(selected);
    if (it == m_map.end()) {
      return CopyType{};
    }
    return _unwrap_smart_ptr(it->second);
  }

  /**
   * Bind a listener that's called when the selected value changes.
   * Only one listener can be bound. Calling this function will replace the
   * previous listener.
   * @param listener The function to call that accepts the new value
   */
  void OnChange(std::function<void(T)> listener) { m_listener = listener; }

 protected:
  void Changed(std::string_view val) override {
    if (m_listener) {
      m_listener(m_map[val]);
    }
  }
};

}  // namespace wpi
