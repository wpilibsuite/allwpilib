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

#include "wpi/tunables/ComplexTunable.hpp"
#include "wpi/util/StringMap.hpp"

namespace wpi::tunables {

namespace detail {
class SelectableBase : public wpi::tunables::ComplexTunable {
 public:
  SelectableBase();

  SelectableBase(const SelectableBase& oth) = delete;
  SelectableBase(SelectableBase&& oth) = default;
  SelectableBase& operator=(const SelectableBase& oth) = delete;
  SelectableBase& operator=(SelectableBase&& oth) = default;

  void PublishTunable(wpi::tunables::TunableTable& table) override;
  std::string_view GetTunableType() const override;

 protected:
  virtual void Changed(std::string_view val) = 0;

  std::string m_defaultChoice;
  std::vector<std::string> m_options;
  std::string m_selected;
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

  auto FindSelectedOrDefault(std::string_view selected) const {
    if (!selected.empty()) {
      auto selectedIt = m_map.find(selected);
      if (selectedIt != m_map.end()) {
        return selectedIt;
      }
    }
    if (m_defaultChoice.empty()) {
      return m_map.end();
    }
    return m_map.find(m_defaultChoice);
  }

 public:
  using CopyType = decltype(_unwrap_smart_ptr(m_map.find("")->second));

  /** Constructs a selectable tunable. */
  Selectable() = default;

  /**
   * Move constructs a selectable tunable.
   *
   * @param rhs selectable tunable to move from
   */
  Selectable(Selectable&& rhs) = default;

  /**
   * Move assigns this selectable tunable.
   *
   * @param rhs selectable tunable to move from
   * @return this selectable tunable
   */
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
      std::erase_if(m_options, [name](const std::string& option) {
        return option == name;
      });
    }
    m_options.emplace_back(name);
    SetChildTunableChanged("options");
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
   * Removes the option with the given name. If the removed option is the
   * default, the default is reset. Does not change the selected option;
   * GetSelected() will fall back to the default after this is called if the
   * selected option name no longer matches an option.
   *
   * @param name the name of the option
   */
  void Remove(std::string_view name) {
    if (m_map.erase(name) == 0) {
      return;
    }
    std::erase_if(m_options,
                  [name](const std::string& option) { return option == name; });
    if (m_defaultChoice == name) {
      m_defaultChoice = "";
      SetChildTunableChanged("default");
    }
    SetChildTunableChanged("options");
  }

  /**
   * Marks the given option as the default.
   *
   * @param name   the name of the option
   */
  void SetDefault(std::string_view name) {
    m_defaultChoice = name;
    SetChildTunableChanged("default");
  }

  /**
   * Clears the list of options and resets the default. Does not change the
   * selected option; GetSelected() will return a value-initialized instance
   * after this is called until a new matching option or default is added.
   */
  void Clear() {
    m_map.clear();
    m_options.clear();
    m_defaultChoice = "";
    SetChildTunableChanged("default");
    SetChildTunableChanged("options");
  }

  /**
   * Returns a copy of the selected option (a std::weak_ptr&lt;U&gt; if T =
   * std::shared_ptr&lt;U&gt;).
   *
   * If there is none or no valid option selected, it will return the default.
   * If there is no valid option selected and no default, then it will return a
   * value-initialized instance. For integer types, this is 0. For container
   * types like std::string, this is an empty string.
   *
   * @return The option selected
   */
  CopyType GetSelected() const {
    auto it = FindSelectedOrDefault(m_selected);
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
      auto it = FindSelectedOrDefault(val);
      if (it != m_map.end()) {
        m_listener(it->second);
      }
    }
  }
};

}  // namespace wpi::tunables
