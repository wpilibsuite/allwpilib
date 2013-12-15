/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "EnumCameraParameter.h"
#include <stdio.h>
#include <string.h>

/**
 * Constructor for an enumeration camera parameter.
 * Enumeration camera parameters have lists of value choices and strings that go
 * with them. There are also C++ enumerations to go along with them.
 * @param setString The string for an HTTP request to set the value.
 * @param getString The string for an HTTP request to get the value.
 * @param choices An array of strings of the parameter choices set in the http strings.
 * @param numChoices The number of choices in the enumeration set.
 */
EnumCameraParameter::EnumCameraParameter(const char *setString, const char *getString, bool requiresRestart,
													const char *const*choices, int numChoices)
	: IntCameraParameter(setString, getString, requiresRestart)
{
	m_enumValues = choices;
	m_numChoices = numChoices;
}

/*
 * Check if an enumeration camera parameter has changed.
 * Check if the parameter has changed and update the camera if it has. This is called
 * from a loop in the parameter checker task.
 * @returns true if the camera needs to restart
 */
bool EnumCameraParameter::CheckChanged(bool &changed, char *param)
{
	changed = m_changed;
	if (m_changed)
	{
		m_changed = false;
		sprintf(param, m_setString, m_enumValues[m_value]);
		return m_requiresRestart;
	}
	return false;
}

/**
 * Extract the parameter value from a string.
 * Extract the parameter value from the camera status message.
 * @param string The string returned from the camera.
 * @param length The length of the string from the camera.
 */
void EnumCameraParameter::GetParamFromString(const char *string, int stringLength)
{
	char resultString[50];
	if (SearchForParam(m_getString, string, stringLength, resultString) < 0) return;
	for (int i = 0; i < m_numChoices; i++)
	{
		if (strcmp(resultString, m_enumValues[i]) == 0)
		{
			if (!m_changed)	  // don't change parameter that's been set in code
			{
				m_value = i;
			}
		}
	}
}

