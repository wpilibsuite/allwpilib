/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/logging/CSVLogFile.h"

#include <chrono>
#include <iostream>

using namespace frc;

CSVLogFile::CSVLogFile(std::string filePrefix)
    : LogFile(filePrefix, "csv"),
      m_cells(),
      m_timestampCell("Timestamp (ms)"),
      m_active(false) {
  RegisterCell(m_timestampCell);
}

void CSVLogFile::RegisterCell(CSVLogCell& cell) {
  if (m_active) {
    std::cout << "You can't add a new cell when the spreadsheet is active: "
              << cell.GetName() << std::endl;
  } else {
    m_cells.push_back(&cell);
  }
}

void CSVLogFile::Start() {
  if (m_active) {
    std::cout << "This table has already been initialized" << std::endl;
    return;
  }

  for (size_t i = 0; i < m_cells.size(); i++) {
    Log("\"" + m_cells[i]->GetName() + "\",");
  }
  Log("\n");

  m_active = true;
}

void CSVLogFile::Periodic() {
  if (m_active) {
    std::chrono::milliseconds timestamp =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch());
    m_timestampCell.Log(timestamp.count());
    WriteRow();
  }
}

void CSVLogFile::WriteRow() {
  for (size_t i = 0; i < m_cells.size(); i++) {
    Log("\"" + m_cells[i]->GetContent() + "\",");
  }
  Log("\n");
}
