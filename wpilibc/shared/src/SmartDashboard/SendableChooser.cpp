/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SmartDashboard/SendableChooser.h"

#include <stdio.h>

static const std::string kDefault = "default";
static const std::string kOptions = "options";
static const std::string kSelected = "selected";

/**
 * Adds the given object to the list of options.
 *
 * On the {@link SmartDashboard} on the desktop, the object will appear as the
 * given name.
 *
 * @param name   the name of the option
 * @param object the option
 */
void SendableChooser::AddObject(const std::string& name, void* object) {
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
void SendableChooser::AddDefault(const std::string& name, void* object) {
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
void* SendableChooser::GetSelected() {
  std::string selected = GetTable()->GetString(kSelected, m_defaultChoice);
  if (selected == "")
    return nullptr;
  else
    return m_choices[selected];
}

void SendableChooser::InitTable(std::shared_ptr<ITable> subtable) {
  if (subtable) {
    std::vector<std::string> keys;
    for (auto& choice : m_choices) {
      keys.push_back(choice.first);
    }
    subtable->PutValue(kOptions, nt::Value::MakeStringArray(std::move(keys)));
    subtable->PutString(kDefault, m_defaultChoice);
  }
  GetTable() = std::move(subtable);
}

std::string SendableChooser::GetSmartDashboardType() const {
  return "String Chooser";
}
