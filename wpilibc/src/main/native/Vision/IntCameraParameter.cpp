/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "IntCameraParameter.h"
#include "pcre.h"
#include <stdio.h>
#include <string.h>

/**
 * Constructor for an integer camera parameter.
 * @param setString The string to set a value in the HTTP request
 * @param getString The string to retrieve a value in the HTTP request
 */
IntCameraParameter::IntCameraParameter(const char *setString, const char *getString, bool requiresRestart)
{
	m_changed = false;
	m_value = 0;
	m_setString = setString;
	m_getString = getString;
	m_requiresRestart = requiresRestart;
}

/**
 * Get a value for a camera parameter.
 * @returns The camera parameter cached valued.
 */
int IntCameraParameter::GetValue()
{
	return m_value;
}

/**
 * Set a value for a camera parameter.
 * Mark the value for change. The value will be updated in the parameter
 * change loop.
 */
void IntCameraParameter::SetValue(int value)
{
	m_value = value;
	m_changed = true;
}

/**
 * Check if a parameter has changed and update.
 * Check if a parameter has changed and send the update string if it
 * has changed. This is called from the loop in the parameter task loop.
 * @returns true if the camera needs to restart
 */
bool IntCameraParameter::CheckChanged(bool &changed, char *param)
{
	changed = m_changed;
	if (m_changed)
	{
		sprintf(param, m_setString, m_value);
		m_changed = false;
		return m_requiresRestart;
	}
	return false;
}

/**
 * Get a parameter value from the string.
 * Get a parameter value from the camera status string. If it has been changed
 * been changed by the program, then don't update it. Program values have
 * precedence over those written in the camera.
 */
void IntCameraParameter::GetParamFromString(const char *string, int stringLength)
{
	char resultString[150];
	if (SearchForParam(m_getString, string, stringLength, resultString) >= 0)
	{	
		if (!m_changed) m_value = atoi(resultString);
	}
}

/**
 * @param pattern: the regular expression
 * @param searchString the text to search
 * @param searchStringLen the length of searchString
 * @param result buffer to put resulting text into, must be pre-allocated
 */
int IntCameraParameter::SearchForParam(const char *pattern, const char *searchString, int searchStringLen, char *result)
{
	int vectorLen = 10;
	int resultVector[vectorLen];
	const char *error;
	int erroffset;
	pcre *compiledPattern = pcre_compile(
			pattern, //"root.Image.I0.Appearance.Resolution=(.*)", 
			PCRE_CASELESS, 
			&error, // for error message 
			&erroffset, // for error offset 
			NULL); // use default character tables 
	int rc;
	rc = pcre_exec(compiledPattern, 
					NULL, 
					searchString, 
					searchStringLen, 
					0, 
					0, 
					resultVector,		//locations of submatches 
					vectorLen);			//size of ovector
	int length = resultVector[3] - resultVector[2];
	memcpy(result, &searchString[resultVector[2]], length);
	result[length] = '\0';
	return rc;
}

