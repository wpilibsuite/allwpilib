/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include "tables/ITable.h"

namespace frc {

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
   *         by the smart dashboard for this sendable
   */
  virtual std::string GetSmartDashboardType() const = 0;
};

}  // namespace frc
