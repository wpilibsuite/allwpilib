/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __SMART_DASHBOARD_DATA__
#define __SMART_DASHBOARD_DATA__

#include <string>
#include <memory>
#include "tables/ITable.h"

class Sendable {
 public:
  /**
   * Initializes a table for this sendable object.
   * @param subtable The table to put the values in.
   */
  virtual void InitTable(std::shared_ptr<ITable> subtable) = 0;

  /**
   * @return the table that is currently associated with the sendable
   */
  virtual std::shared_ptr<ITable> GetTable() const = 0;

  /**
   * @return the string representation of the named data type that will be used
   * by the smart dashboard for this sendable
   */
  virtual std::string GetSmartDashboardType() const = 0;
};

#endif
