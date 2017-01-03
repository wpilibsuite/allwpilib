/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2017. All Rights Reserved.                        */
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

namespace frc {

/**
 * Adds the given object to the list of options.
 *
 * On the {@link SmartDashboard} on the desktop, the object will appear as the
 * given name.
 *
 * @param name   the name of the option
 * @param object the option
 */
template <class T>
void SendableChooser<T>::AddObject(llvm::StringRef name, const T& object) {
  m_choices[name] = object;
}

/**
 * Add the given object to the list of options and marks it as the default.
 *
 * Functionally, this is very close to {@link SendableChooser#AddObject(const
 * char *name, void *object) AddObject(...)} except that it will use this as
 * the default option if none other is explicitly selected.
 *
 * @param name   the name of the option
 * @param object the option
 */
template <class T>
void SendableChooser<T>::AddDefault(llvm::StringRef name, const T& object) {
  m_defaultChoice = name;
  AddObject(name, object);
}

/**
 * Returns the selected option.
 *
 * If there is none selected, it will return the default.  If there is none
 * selected and no default, then it will return {@code nullptr}.
 *
 * @return the option selected
 */
template <class T>
T SendableChooser<T>::GetSelected() {
  std::string selected = m_table->GetString(kSelected, m_defaultChoice);
  if (selected == "") {
    return nullptr;
  } else {
    return m_choices[selected];
  }
}

template <class T>
void SendableChooser<T>::InitTable(std::shared_ptr<ITable> subtable) {
  std::vector<std::string> keys;
  m_table = subtable;
  if (m_table != nullptr) {
    for (const auto& choice : m_choices) {
      keys.push_back(choice.first());
    }

    // Unlike std::map, llvm::StringMap elements are not sorted
    std::sort(keys.begin(), keys.end());

    m_table->PutValue(kOptions, nt::Value::MakeStringArray(std::move(keys)));
    m_table->PutString(kDefault, m_defaultChoice);
  }
}

}  // namespace frc
