/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <wpi/StringRef.h>

namespace frc {

/**
 * Adds the given object to the list of options.
 *
 * On the SmartDashboard on the desktop, the object will appear as the given
 * name.
 *
 * @param name   the name of the option
 * @param object the option
 */
template <class T>
void SendableChooser<T>::AddOption(wpi::StringRef name, T object) {
  m_choices[name] = std::move(object);
}

/**
 * Add the given object to the list of options and marks it as the default.
 *
 * Functionally, this is very close to AddOption() except that it will use this
 * as the default option if none other is explicitly selected.
 *
 * @param name   the name of the option
 * @param object the option
 */
template <class T>
void SendableChooser<T>::SetDefaultOption(wpi::StringRef name, T object) {
  m_defaultChoice = name;
  AddOption(name, std::move(object));
}

/**
 * Returns a copy of the selected option (a raw pointer U* if T =
 * std::unique_ptr<U> or a std::weak_ptr<U> if T = std::shared_ptr<U>).
 *
 * If there is none selected, it will return the default. If there is none
 * selected and no default, then it will return a value-initialized instance.
 * For integer types, this is 0. For container types like std::string, this is
 * an empty string.
 *
 * @return The option selected
 */
template <class T>
auto SendableChooser<T>::GetSelected()
    -> decltype(_unwrap_smart_ptr(m_choices[""])) {
  std::string selected = m_defaultChoice;
  {
    std::scoped_lock lock(m_mutex);
    if (m_haveSelected) selected = m_selected;
  }
  if (selected.empty()) {
    return decltype(_unwrap_smart_ptr(m_choices[""])){};
  } else {
    return _unwrap_smart_ptr(m_choices[selected]);
  }
}

template <class T>
void SendableChooser<T>::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("String Chooser");
  builder.GetEntry(kInstance).SetDouble(m_instance);
  builder.AddStringArrayProperty(kOptions,
                                 [=]() {
                                   std::vector<std::string> keys;
                                   for (const auto& choice : m_choices) {
                                     keys.push_back(choice.first());
                                   }

                                   // Unlike std::map, wpi::StringMap elements
                                   // are not sorted
                                   std::sort(keys.begin(), keys.end());

                                   return keys;
                                 },
                                 nullptr);
  builder.AddSmallStringProperty(
      kDefault,
      [=](wpi::SmallVectorImpl<char>&) -> wpi::StringRef {
        return m_defaultChoice;
      },
      nullptr);
  builder.AddSmallStringProperty(
      kActive,
      [=](wpi::SmallVectorImpl<char>& buf) -> wpi::StringRef {
        std::scoped_lock lock(m_mutex);
        if (m_haveSelected) {
          buf.assign(m_selected.begin(), m_selected.end());
          return wpi::StringRef(buf.data(), buf.size());
        } else {
          return m_defaultChoice;
        }
      },
      nullptr);
  {
    std::scoped_lock lock(m_mutex);
    m_activeEntries.emplace_back(builder.GetEntry(kActive));
  }
  builder.AddStringProperty(kSelected, nullptr, [=](wpi::StringRef val) {
    std::scoped_lock lock(m_mutex);
    m_haveSelected = true;
    m_selected = val;
    for (auto& entry : m_activeEntries) entry.SetString(val);
  });
}

template <class T>
template <class U>
U SendableChooser<T>::_unwrap_smart_ptr(const U& value) {
  return value;
}

template <class T>
template <class U>
U* SendableChooser<T>::_unwrap_smart_ptr(const std::unique_ptr<U>& value) {
  return value.get();
}

template <class T>
template <class U>
std::weak_ptr<U> SendableChooser<T>::_unwrap_smart_ptr(
    const std::shared_ptr<U>& value) {
  return value;
}

}  // namespace frc
