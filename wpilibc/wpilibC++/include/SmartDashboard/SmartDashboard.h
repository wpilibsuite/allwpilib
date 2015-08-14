/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.
 */
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

class SmartDashboard : public SensorBase {
 public:
  static void init();

  static void PutData(llvm::StringRef key, Sendable *data);
  static void PutData(NamedSendable *value);
  static Sendable *GetData(llvm::StringRef keyName);

  static void PutBoolean(llvm::StringRef keyName, bool value);
  static bool GetBoolean(llvm::StringRef keyName, bool defaultValue);

  static void PutNumber(llvm::StringRef keyName, double value);
  static double GetNumber(llvm::StringRef keyName, double defaultValue);

  static void PutString(llvm::StringRef keyName, llvm::StringRef value);
  static std::string GetString(llvm::StringRef keyName,
                               llvm::StringRef defaultValue);

  static void PutValue(llvm::StringRef keyName,
                       std::shared_ptr<nt::Value> value);
  static std::shared_ptr<nt::Value> GetValue(llvm::StringRef keyName);

 private:
  virtual ~SmartDashboard() = default;

  /** The {@link NetworkTable} used by {@link SmartDashboard} */
  static std::shared_ptr<ITable> m_table;

  /**
   * A map linking tables in the SmartDashboard to the {@link
   * SmartDashboardData} objects
   * they came from.
   */
  static std::map<std::shared_ptr<ITable> , Sendable *> m_tablesToData;
};

#endif
