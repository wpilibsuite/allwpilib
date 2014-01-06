/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Preferences.h"

//#include "NetworkCommunication/UsageReporting.h"
#include "HAL/cpp/Synchronized.h"
#include "WPIErrors.h"

#include <stdio.h>
#include <algorithm>

/** Private NI function needed to write to the VxWorks target */
extern "C" int Priv_SetWriteFileAllowed(uint32_t enable); 

/** The Preferences table name */
static const char *kTableName = "Preferences";
/** The value of the save field */
static const char *kSaveField = "~S A V E~";
/** The file to save to */
static const char *kFileName = "/c/wpilib-preferences.ini";
/** The characters to put between a field and value */
static const char *kValuePrefix = "=\"";
/** The characters to put after the value */
static const char *kValueSuffix = "\"\n";
/** The singleton instance */
Preferences *Preferences::_instance = NULL;

Preferences::Preferences() :
	m_fileLock(NULL),
	m_fileOpStarted(NULL),
	m_tableLock(NULL),
	m_readTask("PreferencesReadTask", (FUNCPTR)Preferences::InitReadTask),
	m_writeTask("PreferencesWriteTask", (FUNCPTR)Preferences::InitWriteTask)
{
	m_fileLock = initializeMutexNormal();
	m_fileOpStarted = initializeSemaphore (SEMAPHORE_EMPTY);
	m_tableLock = initializeMutexNormal();

	Synchronized sync(m_fileLock);
	m_readTask.Start((uint32_t)this);
	takeSemaphore(m_fileOpStarted);

	HALReport(HALUsageReporting::kResourceType_Preferences, 0);
}

Preferences::~Preferences()
{
	takeMutex(m_tableLock);
	deleteMutex(m_tableLock);
	takeMutex(m_fileLock);
	deleteSemaphore(m_fileOpStarted);
	deleteMutex(m_fileLock);
}

/**
 * Get the one and only {@link Preferences} object
 * @return pointer to the {@link Preferences}
 */
Preferences *Preferences::GetInstance()
{
	if (_instance == NULL)
		_instance = new Preferences;
	return _instance;
}

/**
 * Returns a vector of all the keys
 * @return a vector of the keys
 */
std::vector<std::string> Preferences::GetKeys()
{
	return m_keys;
}

/**
 * Returns the string at the given key.  If this table does not have a value
 * for that position, then the given defaultValue will be returned.
 * @param key the key
 * @param defaultValue the value to return if none exists in the table
 * @return either the value in the table, or the defaultValue
 */
std::string Preferences::GetString(const char *key, const char *defaultValue)
{
	std::string value = Get(key);
	return value.empty() ? defaultValue : value;
}

/**
 * Returns the string at the given key.  If this table does not have a value
 * for that position, then the given defaultValue will be returned.
 * @param key the key
 * @param value the buffer to copy the value into
 * @param valueSize the size of value
 * @param defaultValue the value to return if none exists in the table
 * @return The size of the returned string
 */
int Preferences::GetString(const char *key, char *value, int valueSize, const char *defaultValue)
{
	std::string stringValue = GetString(key, defaultValue);
	stringValue.copy(value, valueSize);
	return stringValue.size();
}

/**
 * Returns the int at the given key.  If this table does not have a value
 * for that position, then the given defaultValue value will be returned.
 * @param key the key
 * @param defaultValue the value to return if none exists in the table
 * @return either the value in the table, or the defaultValue
 */
int Preferences::GetInt(const char *key, int defaultValue)
{
	std::string value = Get(key);
	if (value.empty())
		return defaultValue;

	return strtol(value.c_str(), NULL, 0);
}

/**
 * Returns the double at the given key.  If this table does not have a value
 * for that position, then the given defaultValue value will be returned.
 * @param key the key
 * @param defaultValue the value to return if none exists in the table
 * @return either the value in the table, or the defaultValue
 */
double Preferences::GetDouble(const char *key, double defaultValue)
{
	std::string value = Get(key);
	if (value.empty())
		return defaultValue;

	return strtod(value.c_str(), NULL);
}

/**
 * Returns the float at the given key.  If this table does not have a value
 * for that position, then the given defaultValue value will be returned.
 * @param key the key
 * @param defaultValue the value to return if none exists in the table
 * @return either the value in the table, or the defaultValue
 */
float Preferences::GetFloat(const char *key, float defaultValue)
{
	std::string value = Get(key);
	if (value.empty())
		return defaultValue;

	return strtod(value.c_str(), NULL);
}

/**
 * Returns the boolean at the given key.  If this table does not have a value
 * for that position, then the given defaultValue value will be returned.
 * @param key the key
 * @param defaultValue the value to return if none exists in the table
 * @return either the value in the table, or the defaultValue
 */
bool Preferences::GetBoolean(const char *key, bool defaultValue)
{
	std::string value = Get(key);
	if (value.empty())
		return defaultValue;

	if (value.compare("true") == 0)
		return true;
	else if (value.compare("false") == 0)
		return false;

	wpi_setWPIErrorWithContext(ParameterOutOfRange, "Boolean value does not contain \"true\" or \"false\"");
	return false;
}

/**
 * Returns the long (int64_t) at the given key.  If this table does not have a value
 * for that position, then the given defaultValue value will be returned.
 * @param key the key
 * @param defaultValue the value to return if none exists in the table
 * @return either the value in the table, or the defaultValue
 */
int64_t Preferences::GetLong(const char *key, int64_t defaultValue)
{
	std::string value = Get(key);
	if (value.empty())
		return defaultValue;

	// Ummm... not available in our VxWorks...
	//return strtoll(value.c_str(), NULL, 0);
	int64_t intVal;
	sscanf(value.c_str(), "%lld", &intVal);
	return intVal;
}

/**
 * Puts the given string into the preferences table.
 *
 * <p>The value may not have quotation marks, nor may the key
 * have any whitespace nor an equals sign</p>
 *
 * <p>This will <b>NOT</b> save the value to memory between power cycles,
 * to do that you must call {@link Preferences#Save() Save()} (which must be used with care).
 * at some point after calling this.</p>
 * @param key the key
 * @param value the value
 */
void Preferences::PutString(const char *key, const char *value)
{
	if (value == NULL)
	{
		wpi_setWPIErrorWithContext(NullParameter, "value");
		return;
	}
	if (std::string(value).find_first_of("\"") != std::string::npos)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "value contains illegal characters");
		return;
	}
	Put(key, value);
}

/**
 * Puts the given int into the preferences table.
 *
 * <p>The key may not have any whitespace nor an equals sign</p>
 *
 * <p>This will <b>NOT</b> save the value to memory between power cycles,
 * to do that you must call {@link Preferences#Save() Save()} (which must be used with care)
 * at some point after calling this.</p>
 * @param key the key
 * @param value the value
 */
void Preferences::PutInt(const char *key, int value)
{
	char buf[32];
	snprintf(buf, 32, "%d", value);
	Put(key, buf);
}

/**
 * Puts the given double into the preferences table.
 *
 * <p>The key may not have any whitespace nor an equals sign</p>
 *
 * <p>This will <b>NOT</b> save the value to memory between power cycles,
 * to do that you must call {@link Preferences#Save() Save()} (which must be used with care)
 * at some point after calling this.</p>
 * @param key the key
 * @param value the value
 */
void Preferences::PutDouble(const char *key, double value)
{
	char buf[32];
	snprintf(buf, 32, "%f", value);
	Put(key, buf);
}

/**
 * Puts the given float into the preferences table.
 *
 * <p>The key may not have any whitespace nor an equals sign</p>
 *
 * <p>This will <b>NOT</b> save the value to memory between power cycles,
 * to do that you must call {@link Preferences#Save() Save()} (which must be used with care)
 * at some point after calling this.</p>
 * @param key the key
 * @param value the value
 */
void Preferences::PutFloat(const char *key, float value)
{
	char buf[32];
	snprintf(buf, 32, "%f", value);
	Put(key, buf);
}

/**
 * Puts the given boolean into the preferences table.
 *
 * <p>The key may not have any whitespace nor an equals sign</p>
 *
 * <p>This will <b>NOT</b> save the value to memory between power cycles,
 * to do that you must call {@link Preferences#Save() Save()} (which must be used with care)
 * at some point after calling this.</p>
 * @param key the key
 * @param value the value
 */
void Preferences::PutBoolean(const char *key, bool value)
{
	Put(key, value ? "true" : "false");
}

/**
 * Puts the given long (int64_t) into the preferences table.
 *
 * <p>The key may not have any whitespace nor an equals sign</p>
 *
 * <p>This will <b>NOT</b> save the value to memory between power cycles,
 * to do that you must call {@link Preferences#Save() Save()} (which must be used with care)
 * at some point after calling this.</p>
 * @param key the key
 * @param value the value
 */
void Preferences::PutLong(const char *key, int64_t value)
{
	char buf[32];
	snprintf(buf, 32, "%lld", value);
	Put(key, buf);
}

/**
 * Saves the preferences to a file on the cRIO.
 *
 * <p>This should <b>NOT</b> be called often.
 * Too many writes can damage the cRIO's flash memory.
 * While it is ok to save once or twice a match, this should never
 * be called every run of {@link IterativeRobot#TeleopPeriodic()}, etc.</p>
 *
 * <p>The actual writing of the file is done in a separate thread.
 * However, any call to a get or put method will wait until the table is fully saved before continuing.</p>
 */
void Preferences::Save()
{
	Synchronized sync(m_fileLock);
	m_writeTask.Start((uint32_t)this);
	takeSemaphore(m_fileOpStarted);
}

/**
 * Returns whether or not there is a key with the given name.
 * @param key the key
 * @return if there is a value at the given key
 */
bool Preferences::ContainsKey(const char *key)
{
	return !Get(key).empty();
}

/**
 * Remove a preference
 * @param key the key
 */
void Preferences::Remove(const char *key)
{
	m_values.erase(std::string(key));
	std::vector<std::string>::iterator it = m_keys.begin();
	for (; it != m_keys.end(); it++)
	{
		if (it->compare(key) == 0)
		{
			m_keys.erase(it);
			break;
		}
	}
}

/**
 * Returns the value at the given key.
 * @param key the key
 * @return the value (or empty if none exists)
 */
std::string Preferences::Get(const char *key)
{
	Synchronized sync(m_tableLock);
	if (key == NULL)
	{
		wpi_setWPIErrorWithContext(NullParameter, "key");
		return std::string("");
	}
	return m_values[std::string(key)];
}

/**
 * Puts the given value into the given key position
 * @param key the key
 * @param value the value
 */
void Preferences::Put(const char *key, std::string value)
{
	Synchronized sync(m_tableLock);
	if (key == NULL)
	{
		wpi_setWPIErrorWithContext(NullParameter, "key");
		return;
	}

	if (std::string(key).find_first_of("=\n\r \t\"") != std::string::npos)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "key contains illegal characters");
		return;
	}

	std::pair<StringMap::iterator, bool> ret =
		m_values.insert(StringMap::value_type(key, value));
	if (ret.second)
		m_keys.push_back(key);
	else
		ret.first->second = value;

	NetworkTable::GetTable(kTableName)->PutString(key, value);
}

/**
 * The internal method to read from a file.
 * This will be called in its own thread when the preferences singleton is
 * first created.
 */
void Preferences::ReadTaskRun()
{
	Synchronized sync(m_tableLock);
	giveSemaphore(m_fileOpStarted);

	std::string comment;

	FILE *file = NULL;
	file = fopen(kFileName, "r");

	if (file != NULL)
	{
		std::string buffer;
		while (true)
		{
			char value;
			do
			{
				value = fgetc(file);
			} while (value == ' ' || value == '\t');
			
			if (value == '\n' || value == ';')
			{
				if (value == '\n')
				{
					comment += "\n";
				}
				else
				{
					buffer.clear();
					for (; value != '\n' && !feof(file); value = fgetc(file))
						buffer += value;
					buffer += '\n';
					comment += buffer;
				}
			}
			else if (value == '[')
			{
				// Find the end of the section and the new line after it and throw it away
				for (; value != ']' && !feof(file); value = fgetc(file));
				for (; value != '\n' && !feof(file); value = fgetc(file));
			}
			else
			{
				buffer.clear();
				for (; value != '=' && !feof(file); )
				{
					buffer += value;
					do
					{
						value = fgetc(file);
					} while (value == ' ' || value == '\t');
				}
				std::string name = buffer;
				buffer.clear();

				bool shouldBreak = false;

				do
				{
					value = fgetc(file);
				} while (value == ' ' || value == '\t');

				if (value == '"')
				{
					for (value = fgetc(file); value != '"' && !feof(file); value = fgetc(file))
						buffer += value;

					// Clear the line
					while (fgetc(file) != '\n' && !feof(file));
				}
				else
				{
					for (; value != '\n' && !feof(file);)
					{
						buffer += value;
						do
						{
							value = fgetc(file);
						} while (value == ' ' || value == '\t');
					}
					if (feof(file))
						shouldBreak = true;
				}

				std::string value = buffer;

				if (!name.empty() && !value.empty())
				{
					m_keys.push_back(name);
					m_values.insert(std::pair<std::string, std::string>(name, value));
					NetworkTable::GetTable(kTableName)->PutString(name, value);

					if (!comment.empty())
					{
						m_comments.insert(std::pair<std::string, std::string>(name, comment));
						comment.clear();
					}
				}

				if (shouldBreak)
					break;
			}
		}
	}
	else
	{
		wpi_setWPIErrorWithContext(NoAvailableResources, "Failed to open preferences file.");
	}

	if (file != NULL)
		fclose(file);

	if (!comment.empty())
		m_endComment = comment;
	
	NetworkTable::GetTable(kTableName)->PutBoolean(kSaveField, false);
	NetworkTable::GetTable(kTableName)->AddTableListener(this);
}

/**
 * Internal method that actually writes the table to a file.
 * This is called in its own thread when {@link Preferences#Save() Save()} is called.
 */
void Preferences::WriteTaskRun()
{
	Synchronized sync(m_tableLock);
	giveSemaphore(m_fileOpStarted);

	FILE *file = NULL;
	Priv_SetWriteFileAllowed(1);
	file = fopen(kFileName, "w");

	fputs("[Preferences]\n", file);
	std::vector<std::string>::iterator it = m_keys.begin();
	for (; it != m_keys.end(); it++)
	{
		std::string key = *it;
		std::string value = m_values[key];
		std::string comment = m_comments[key];

		if (!comment.empty())
			fputs(comment.c_str(), file);

		fputs(key.c_str(), file);
		fputs(kValuePrefix, file);
		fputs(value.c_str(), file);
		fputs(kValueSuffix, file);
	}

	if (!m_endComment.empty())
		fputs(m_endComment.c_str(), file);

	if (file != NULL)
		fclose(file);

	NetworkTable::GetTable(kTableName)->PutBoolean(kSaveField, false);
}

static bool isKeyAcceptable(const std::string& value) {
    for (unsigned int i = 0; i < value.length(); i++) {
        char letter = value.at(i);
        switch (letter) {
            case '=':
            case '\n':
            case '\r':
            case ' ':
            case '\t':
            	return false;
        }
    }
    return true;
}
void Preferences::ValueChanged(ITable* table, const std::string& key, EntryValue value, bool isNew)
{
	if (key==kSaveField)
	{
		if (table->GetBoolean(kSaveField, false))
			Save();
	}
	else
	{
		Synchronized sync(m_tableLock);

		if (!isKeyAcceptable(key) || table->GetString(key, "").find('"')!=std::string::npos)
		{
			if(m_values.find(key) != m_values.end()){
				m_values.erase(key);
				std::vector<std::string>::iterator it = m_keys.begin();
				for (; it != m_keys.end(); it++)
				{
					if (key==*it)
					{
						m_keys.erase(it);
						break;
					}
				}
				table->PutString(key, "\"");
			}
		}
		else
		{
			std::pair<StringMap::iterator, bool> ret =
				m_values.insert(StringMap::value_type(key, table->GetString(key, "")));
			if (ret.second)
				m_keys.push_back(key);
			else
				ret.first->second = table->GetString(key, "");
		}
	}
}
