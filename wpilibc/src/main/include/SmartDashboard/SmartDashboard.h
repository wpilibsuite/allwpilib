/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __SMART_DASHBOARD_H__
#define __SMART_DASHBOARD_H__

#include "SensorBase.h"
#include <map>
#include <string>
#include "SmartDashboard/Sendable.h"
#include "SmartDashboard/NamedSendable.h"
#include "tables/ITable.h"


class SmartDashboard : public SensorBase
{
public:
	static void init();
	
	static void PutData(std::string key, Sendable *data);
	static void PutData(NamedSendable *value);
	//static Sendable* GetData(std::string keyName);
	
	static void PutBoolean(std::string keyName, bool value);
	static bool GetBoolean(std::string keyName);
	
	static void PutNumber(std::string keyName, double value);
	static double GetNumber(std::string keyName);
	
	static void PutString(std::string keyName, std::string value);
	static int GetString(std::string keyName, char *value, unsigned int valueLen);
	static std::string GetString(std::string keyName);
	
	static void PutValue(std::string keyName, ComplexData& value);
	static void RetrieveValue(std::string keyName, ComplexData& value);
private:
	SmartDashboard();
	virtual ~SmartDashboard();
	DISALLOW_COPY_AND_ASSIGN(SmartDashboard);

	/** The {@link NetworkTable} used by {@link SmartDashboard} */
	static ITable* m_table;
	
	/** 
	 * A map linking tables in the SmartDashboard to the {@link SmartDashboardData} objects
	 * they came from.
	 */
	static std::map<ITable *, Sendable *> m_tablesToData;
};

#endif

