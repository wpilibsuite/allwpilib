/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __DASHBOARD_H__
#define __DASHBOARD_H__

#include "DashboardBase.h"
//#include "NetworkCommunication/FRCComm.h"
#include <stack>
#include <vector>
#include "HAL/HAL.h"
#include "HAL/Semaphore.h"

/**
 * Pack data into the "user data" field that gets sent to the dashboard laptop
 * via the driver station.
 */
class Dashboard : public DashboardBase
{
public:
	explicit Dashboard(MUTEX_ID statusDataSemaphore);
	virtual ~Dashboard();

	enum Type {kI8, kI16, kI32, kU8, kU16, kU32, kFloat, kDouble, kBoolean, kString, kOther};
	enum ComplexType {kArray, kCluster};

	void AddI8(int8_t value);
	void AddI16(int16_t value);
	void AddI32(int32_t value);
	void AddU8(uint8_t value);
	void AddU16(uint16_t value);
	void AddU32(uint32_t value);
	void AddFloat(float value);
	void AddDouble(double value);
	void AddBoolean(bool value);
	void AddString(char* value);
	void AddString(char* value, int32_t length);

	void AddArray();
	void FinalizeArray();
	void AddCluster();
	void FinalizeCluster();

	void Printf(const char *writeFmt, ...);

	int32_t Finalize();
	void GetStatusBuffer(char** userStatusData, int32_t* userStatusDataSize);
	void Flush() {}
private:
	static const int32_t kMaxDashboardDataSize = HAL_USER_STATUS_DATA_SIZE - sizeof(uint32_t) * 3 - sizeof(uint8_t); // 13 bytes needed for 3 size parameters and the sequence number

	// Usage Guidelines...
	DISALLOW_COPY_AND_ASSIGN(Dashboard);

	bool ValidateAdd(int32_t size);
	void AddedElement(Type type);
	bool IsArrayRoot();

	char *m_userStatusData;
	int32_t m_userStatusDataSize;
	char *m_localBuffer;
	char *m_localPrintBuffer;
	char *m_packPtr;
	std::vector<Type> m_expectedArrayElementType;
	std::vector<int32_t> m_arrayElementCount;
	std::vector<int32_t*> m_arraySizePtr;
	std::stack<ComplexType> m_complexTypeStack;
	MUTEX_ID m_printSemaphore;
	MUTEX_ID m_statusDataSemaphore;
};

#endif

