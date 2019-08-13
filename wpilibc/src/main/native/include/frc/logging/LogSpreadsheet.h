/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "frc/logging/LogCell.h"

namespace frc {

/**
 * A LogSpreadsheet writes to a file the contents of each of its registered
 * LogCell's.
 *
 * For the LogSpreadsheet to write log informations, you must call Periodic()
 * periodically.
 */
class LogSpreadsheet {
 public:
  /**
   * Instantiate a LogSpreadsheet passing in the name of the table.
   *
   * @param name The name of the table.
   */
  explicit LogSpreadsheet(std::string name = "");
  virtual ~LogSpreadsheet();

  /**
   * Register a new column to be logged. You will figure this value out
   * in LogCell.GetContent().
   *
   * A LogCell can only be registered if the spreadsheet is inactive:
   * see Start() and Stop() to activate or inactivate it.
   *
   * @param cell The LogCell to register.
   */
  void RegisterCell(LogCell& cell);

  /**
   * Start the table: open a new file and write the column headers.
   *
   * It causes the spreadsheet to be "active".
   */
  void Start();

  /**
   * Release the table and close the file.
   *
   * It causes the spreadsheet to be "inactive".
   * Previously registered LogCell's remain unchanged.
   */
  void Stop();

  /**
   * Call it regularly. If the table is active, write a row of the table.
   */
  void Periodic();

 private:
  void WriteRow();

  std::string m_name;
  std::vector<LogCell*> m_cells;
  LogCell m_timestampCell;
  std::ofstream m_logFile;
  bool m_active;
};

}  // namespace frc
