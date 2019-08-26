/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/logging/CSVLogFile.h"

#include <chrono>

#include <wpi/raw_ostream.h>

using namespace frc;

CSVLogFile::CSVLogFile(wpi::StringRef filePrefix)
    : m_logFile(filePrefix, "csv"), m_timestampCell("Timestamp (ms)") {
  RegisterCell(m_timestampCell);
}

void CSVLogFile::RegisterCell(CSVLogCell& cell) {
  if (m_active) {
    wpi::outs() << "You can't add a new cell when the spreadsheet is active: "
                << cell.GetName() << '\n';
  } else {
    m_cells.push_back(&cell);
  }
}

void CSVLogFile::Start() {
  if (m_active) {
    wpi::outs() << "This table has already been initialized" << '\n';
    return;
  }

  for (const auto& cell : m_cells) {
    m_logFile.Log("\"" + cell->GetName() + "\",");
  }
  m_logFile.Log("\n");

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
  for (const auto& cell : m_cells) {
    m_logFile.Log("\"" + cell->GetContent() + "\",");
  }
  m_logFile.Log("\n");
}
