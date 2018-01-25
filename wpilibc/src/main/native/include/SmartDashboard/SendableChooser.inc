/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
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

#include <llvm/StringRef.h>

#include "SendableBuilder.h"

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
void SendableChooser<T>::AddObject(llvm::StringRef name, T object) {
  m_choices[name] = std::move(object);
}

/**
 * Add the given object to the list of options and marks it as the default.
 *
 * Functionally, this is very close to AddObject() except that it will use this
 * as the default option if none other is explicitly selected.
 *
 * @param name   the name of the option
 * @param object the option
 */
template <class T>
void SendableChooser<T>::AddDefault(llvm::StringRef name, T object) {
  m_defaultChoice = name;
  AddObject(name, std::move(object));
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
  llvm::StringRef selected = m_defaultChoice;
  if (m_selectedEntry) {
    selected = m_selectedEntry.GetString(m_defaultChoice);
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
  builder.AddStringArrayProperty(kOptions,
                                 [=]() {
                                   std::vector<std::string> keys;
                                   for (const auto& choice : m_choices) {
                                     keys.push_back(choice.first());
                                   }

                                   // Unlike std::map, llvm::StringMap elements
                                   // are not sorted
                                   std::sort(keys.begin(), keys.end());

                                   return keys;
                                 },
                                 nullptr);
  builder.AddSmallStringProperty(
      kDefault,
      [=](const llvm::SmallVectorImpl<char>&) -> llvm::StringRef {
        return m_defaultChoice;
      },
      nullptr);
  m_selectedEntry = builder.GetEntry(kSelected);
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
