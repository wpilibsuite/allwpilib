/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "SmartDashboard/SendableChooser.h"
#include "networktables2/type/StringArray.h"

#include <stdio.h>

static const char *kDefault = "default";
static const char *kOptions = "options";
static const char *kSelected = "selected";

SendableChooser::SendableChooser()
{
	m_defaultChoice = "";
}

/**
 * Adds the given object to the list of options.  On the {@link SmartDashboard} on the desktop,
 * the object will appear as the given name.
 * @param name the name of the option
 * @param object the option
 */
void SendableChooser::AddObject(const char *name, void *object)
{
	m_choices[name] = object;
}

/**
 * Add the given object to the list of options and marks it as the default.
 * Functionally, this is very close to {@link SendableChooser#AddObject(const char *name, void *object) AddObject(...)}
 * except that it will use this as the default option if none other is explicitly selected.
 * @param name the name of the option
 * @param object the option
 */
void SendableChooser::AddDefault(const char *name, void *object)
{
	m_defaultChoice = name;
	AddObject(name, object);
}

/**
 * Returns the selected option.  If there is none selected, it will return the default.  If there is none selected
 * and no default, then it will return {@code NULL}.
 * @return the option selected
 */
void *SendableChooser::GetSelected()
{
	std::string selected = m_table->GetString(kSelected, m_defaultChoice);
	if (selected == "")
		return NULL;
	else
		return m_choices[selected];
}

void SendableChooser::InitTable(ITable* subtable) {
	StringArray keys;
	m_table = subtable;
	if (m_table != NULL) {
		std::map<std::string, void *>::iterator iter;
		for (iter = m_choices.begin(); iter != m_choices.end(); iter++) {
			keys.add(iter->first);
		}
		m_table->PutValue(kOptions, keys);
		m_table->PutString(kDefault, m_defaultChoice);
	}
}

ITable* SendableChooser::GetTable() {
	return m_table;
}

std::string SendableChooser::GetSmartDashboardType() {
	return "String Chooser";
}
