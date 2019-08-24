/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <vector>

#include "frc/logging/LogFile.h"
#include "frc/logging/CSVLogCell.h"

namespace frc {

/**
 * A CSVLogFile is a LogFile that writes to a file the contents of each of its
 * registered CSVLogCell's.
 *
 * For the CSVLogFile to write log informations, you must call Periodic()
 * periodically.
 */
class CSVLogFile : public LogFile {
 public:
  /**
   * Instantiate a LogFile passing in its prefix and its extension.
   *
   * If you want the file to be saved in a existing directory, you can add its
   * path before the file prefix. Exemple : to save the file in a usb stick on
   * the roborio ("/media/sda1/") : LogFile("/media/sda1/log").
   *
   * @param filePrefix The prefix of the LogFile.
   */
  explicit CSVLogFile(std::string filePrefix = "log");

  /**
   * Register a new column to be logged. You will figure this value out
   * in CSVLogCell.GetContent().
   *
   * A CSVLogCell can only be registered if the spreadsheet is inactive:
   * see Start() and Stop() to activate or inactivate it.
   *
   * @param cell The CSVLogCell to register.
   */
  void RegisterCell(CSVLogCell& cell);

  /**
   * Start the CSVLogFile: open a new file and write the column headers.
   *
   * It causes the CSVLogFile to be "active".
   */
  void Start();

  /**
   * Call it regularly. If the CSVLogFile is active, write a row of the file.
   */
  void Periodic();

 private:
  /**
   * Write a row of the file.
   */
  void WriteRow();

  std::vector<CSVLogCell*> m_cells;
  CSVLogCell m_timestampCell;
};

}  // namespace frc
