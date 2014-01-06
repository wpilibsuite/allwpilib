/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Dashboard.h"
#include "DriverStation.h"
//#include "NetworkCommunication/UsageReporting.h"
#include "HAL/cpp/Synchronized.h"
#include "WPIErrors.h"
#include <string.h>
#include <stdarg.h>

const int32_t Dashboard::kMaxDashboardDataSize;

/**
 * Dashboard contructor.
 * 
 * This is only called once when the DriverStation constructor is called.
 */
Dashboard::Dashboard(MUTEX_ID statusDataSem)
	: m_userStatusData (NULL)
	, m_userStatusDataSize (0)
	, m_localBuffer (NULL)
	, m_localPrintBuffer (NULL)
	, m_packPtr (NULL)
	, m_printSemaphore (0)
	, m_statusDataSemaphore (statusDataSem)
{
	m_userStatusData = new char[kMaxDashboardDataSize];
	m_localBuffer = new char[kMaxDashboardDataSize];
	m_localPrintBuffer = new char[kMaxDashboardDataSize * 2];
	m_localPrintBuffer[0] = 0;
	m_packPtr = m_localBuffer;
	m_printSemaphore = initializeMutexNormal();
}

/**
 * Dashboard destructor.
 * 
 * Called only when the DriverStation class is destroyed.
 */
Dashboard::~Dashboard()
{
	deleteMutex(m_printSemaphore);
	m_packPtr = NULL;
	delete [] m_localPrintBuffer;
	m_localPrintBuffer = NULL;
	delete [] m_localBuffer;
	m_localBuffer = NULL;
	delete [] m_userStatusData;
	m_userStatusData = NULL;
}

/**
 * Pack a signed 8-bit int into the dashboard data structure.
 * @param value Data to be packed into the structure.
 */
void Dashboard::AddI8(int8_t value)
{
	if (!ValidateAdd(sizeof(int8_t))) return;
	memcpy(m_packPtr, (char*)&value, sizeof(value));
	m_packPtr += sizeof(value);
	AddedElement(kI8);
}

/**
 * Pack a signed 16-bit int into the dashboard data structure.
 * @param value Data to be packed into the structure.
 */
void Dashboard::AddI16(int16_t value)
{
	if (!ValidateAdd(sizeof(int16_t))) return;
	memcpy(m_packPtr, (char*)&value, sizeof(value));
	m_packPtr += sizeof(value);
	AddedElement(kI16);
}

/**
 * Pack a signed 32-bit int into the dashboard data structure.
 * @param value Data to be packed into the structure.
 */
void Dashboard::AddI32(int32_t value)
{
	if (!ValidateAdd(sizeof(int32_t))) return;
	memcpy(m_packPtr, (char*)&value, sizeof(value));
	m_packPtr += sizeof(value);
	AddedElement(kI32);
}

/**
 * Pack an unsigned 8-bit int into the dashboard data structure.
 * @param value Data to be packed into the structure.
 */
void Dashboard::AddU8(uint8_t value)
{
	if (!ValidateAdd(sizeof(uint8_t))) return;
	memcpy(m_packPtr, (char*)&value, sizeof(value));
	m_packPtr += sizeof(value);
	AddedElement(kU8);
}

/**
 * Pack an unsigned 16-bit int into the dashboard data structure.
 * @param value Data to be packed into the structure.
 */
void Dashboard::AddU16(uint16_t value)
{
	if (!ValidateAdd(sizeof(uint16_t))) return;
	memcpy(m_packPtr, (char*)&value, sizeof(value));
	m_packPtr += sizeof(value);
	AddedElement(kU16);
}

/**
 * Pack an unsigned 32-bit int into the dashboard data structure.
 * @param value Data to be packed into the structure.
 */
void Dashboard::AddU32(uint32_t value)
{
	if (!ValidateAdd(sizeof(uint32_t))) return;
	memcpy(m_packPtr, (char*)&value, sizeof(value));
	m_packPtr += sizeof(value);
	AddedElement(kU32);
}

/**
 * Pack a 32-bit floating point number into the dashboard data structure.
 * @param value Data to be packed into the structure.
 */
void Dashboard::AddFloat(float value)
{
	if (!ValidateAdd(sizeof(float))) return;
	memcpy(m_packPtr, (char*)&value, sizeof(value));
	m_packPtr += sizeof(value);
	AddedElement(kFloat);
}

/**
 * Pack a 64-bit floating point number into the dashboard data structure.
 * @param value Data to be packed into the structure.
 */
void Dashboard::AddDouble(double value)
{
	if (!ValidateAdd(sizeof(double))) return;
	memcpy(m_packPtr, (char*)&value, sizeof(value));
	m_packPtr += sizeof(value);
	AddedElement(kDouble);
}

/**
 * Pack a boolean into the dashboard data structure.
 * @param value Data to be packed into the structure.
 */
void Dashboard::AddBoolean(bool value)
{
	if (!ValidateAdd(sizeof(char))) return;
	*m_packPtr = value ? 1 : 0;
	m_packPtr += sizeof(char);	
	AddedElement(kBoolean);
}

/**
 * Pack a NULL-terminated string of 8-bit characters into the dashboard data structure.
 * @param value Data to be packed into the structure.
 */
void Dashboard::AddString(char* value)
{
	AddString(value, strlen(value));
}

/**
 * Pack a string of 8-bit characters of specified length into the dashboard data structure.
 * @param value Data to be packed into the structure.
 * @param length The number of bytes in the string to pack.
 */
void Dashboard::AddString(char* value, int32_t length)
{
	if (!ValidateAdd(length + sizeof(length))) return;
	memcpy(m_packPtr, (char*)&length, sizeof(length));
	m_packPtr += sizeof(length);
	memcpy(m_packPtr, value, length);
	m_packPtr += length;
	AddedElement(kString);
}

/**
 * Start an array in the packed dashboard data structure.
 * 
 * After calling AddArray(), call the appropriate Add method for each element of the array.
 * Make sure you call the same add each time.  An array must contain elements of the same type.
 * You can use clusters inside of arrays to make each element of the array contain a structure of values.
 * You can also nest arrays inside of other arrays.
 * Every call to AddArray() must have a matching call to FinalizeArray().
 */
void Dashboard::AddArray()
{
	if (!ValidateAdd(sizeof(int32_t))) return;
	m_complexTypeStack.push(kArray);
	m_arrayElementCount.push_back(0);
	m_arraySizePtr.push_back((int32_t*)m_packPtr);
	m_packPtr += sizeof(int32_t);
}

/**
 * Indicate the end of an array packed into the dashboard data structure.
 * 
 * After packing data into the array, call FinalizeArray().
 * Every call to AddArray() must have a matching call to FinalizeArray().
 */
void Dashboard::FinalizeArray()
{
	if (m_complexTypeStack.top() != kArray)
	{
		wpi_setWPIError(MismatchedComplexTypeClose);
		return;
	}
	m_complexTypeStack.pop();
	*(m_arraySizePtr.back()) = m_arrayElementCount.back();
	m_arraySizePtr.pop_back();
	if (m_arrayElementCount.back() != 0)
	{
		m_expectedArrayElementType.pop_back();
	}
	m_arrayElementCount.pop_back();
	AddedElement(kOther);
}

/**
 * Start a cluster in the packed dashboard data structure.
 * 
 * After calling AddCluster(), call the appropriate Add method for each element of the cluster.
 * You can use clusters inside of arrays to make each element of the array contain a structure of values.
 * Every call to AddCluster() must have a matching call to FinalizeCluster().
 */
void Dashboard::AddCluster()
{
	m_complexTypeStack.push(kCluster);
}

/**
 * Indicate the end of a cluster packed into the dashboard data structure.
 * 
 * After packing data into the cluster, call FinalizeCluster().
 * Every call to AddCluster() must have a matching call to FinalizeCluster().
 */
void Dashboard::FinalizeCluster()
{
	if (m_complexTypeStack.top() != kCluster)
	{
		wpi_setWPIError(MismatchedComplexTypeClose);
		return;
	}
	m_complexTypeStack.pop();
	AddedElement(kOther);
}

/**
 * Print a string to the UserData text on the Dashboard.
 * 
 * This will add text to the buffer to send to the dashboard.
 * You must call Finalize() periodically to actually send the buffer to the dashboard if you are not using the packed dashboard data.
 */
void Dashboard::Printf(const char *writeFmt, ...)
{
	va_list args;
	int32_t size;

	// Check if the buffer has already been used for packing.
	if (m_packPtr != m_localBuffer)
	{
		wpi_setWPIError(DashboardDataCollision);
		return;
	}
	va_start (args, writeFmt);
	{
		Synchronized sync(m_printSemaphore);
		vsprintf(m_localPrintBuffer + strlen(m_localPrintBuffer), writeFmt, args);
		size = strlen(m_localPrintBuffer);
	}
	if (size > kMaxDashboardDataSize)
	{
		wpi_setWPIError(DashboardDataOverflow);
	}

	va_end (args);
}

/**
 * Indicate that the packing is complete and commit the buffer to the DriverStation.
 * 
 * The packing of the dashboard packet is complete.
 * If you are not using the packed dashboard data, you can call Finalize() to commit the Printf() buffer and the error string buffer.
 * In effect, you are packing an empty structure.
 * Prepares a packet to go to the dashboard...
 * @return The total size of the data packed into the userData field of the status packet.
 */
int32_t Dashboard::Finalize()
{
	if (!m_complexTypeStack.empty())
	{
		wpi_setWPIError(MismatchedComplexTypeClose);
		return 0;
	}

	static bool reported = false;
	if (!reported)
	{
		HALReport(HALUsageReporting::kResourceType_Dashboard, 0);
		reported = true;
	}

	Synchronized sync(m_statusDataSemaphore);

	// Sequence number
	DriverStation::GetInstance()->IncrementUpdateNumber();

	// Packed Dashboard Data
	m_userStatusDataSize = m_packPtr - m_localBuffer;
	memcpy(m_userStatusData, m_localBuffer, m_userStatusDataSize);
	m_packPtr = m_localBuffer;

	return m_userStatusDataSize;
}

/**
 * Called by the DriverStation class to retrieve buffers, sizes, etc. for writing
 *   to the NetworkCommunication task.
 * This function is called while holding the m_statusDataSemaphore.
 */
void Dashboard::GetStatusBuffer(char **userStatusData, int32_t* userStatusDataSize)
{
	// User printed strings
	if (m_localPrintBuffer[0] != 0)
	{
		// Sequence number
		DriverStation::GetInstance()->IncrementUpdateNumber();

		int32_t printSize;
		Synchronized syncPrint(m_printSemaphore);
		printSize = strlen(m_localPrintBuffer);
		m_userStatusDataSize = printSize;
		memcpy(m_userStatusData, m_localPrintBuffer, m_userStatusDataSize);
		m_localPrintBuffer[0] = 0;
	}

	*userStatusData = m_userStatusData;
	*userStatusDataSize = m_userStatusDataSize;
}

/**
 * Validate that the data being packed will fit in the buffer.
 */
bool Dashboard::ValidateAdd(int32_t size)
{
	if ((m_packPtr - m_localBuffer) + size > kMaxDashboardDataSize)
	{
		wpi_setWPIError(DashboardDataOverflow);
		return false;
	}
	// Make sure printf is not being used at the same time.
	if (m_localPrintBuffer[0] != 0)
	{
		wpi_setWPIError(DashboardDataCollision);
		return false;
	}
	return true;
}

/**
 * Check for consistent types when adding elements to an array and keep track of the number of elements in the array.
 */
void Dashboard::AddedElement(Type type)
{
	if(IsArrayRoot())
	{
		if (m_arrayElementCount.back() == 0)
		{
			m_expectedArrayElementType.push_back(type);
		}
		else
		{
			if (type != m_expectedArrayElementType.back())
			{
				wpi_setWPIError(InconsistentArrayValueAdded);
			}
		}
		m_arrayElementCount.back() = m_arrayElementCount.back() + 1;
	}
}

/**
 * If the top of the type stack an array?
 */
bool Dashboard::IsArrayRoot()
{
	return !m_complexTypeStack.empty() && m_complexTypeStack.top() == kArray;
}

