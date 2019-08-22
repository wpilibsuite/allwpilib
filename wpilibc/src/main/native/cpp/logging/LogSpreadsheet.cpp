/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/logging/LogSpreadsheet.h"

#include <chrono>
#include <ctime>
#include <iostream>

using namespace frc;

LogSpreadsheet::LogSpreadsheet(std::string name)
    : m_name(name),
      m_cells(),
      m_timestampCell("Timestamp (ms)"),
      m_active(false) {
  RegisterCell(m_timestampCell);
}

LogSpreadsheet::~LogSpreadsheet() {
  if (m_active) m_logFile.close();
}

void LogSpreadsheet::RegisterCell(LogCell& cell) {
  if (m_active) {
    std::cout << "You can't add a new cell when the spreadsheet is active: "
              << cell.GetName() << std::endl;
  } else {
    m_cells.push_back(&cell);
  }
}

void LogSpreadsheet::Start() {
  if (m_active) {
    std::cout << "This table has already been initialized" << std::endl;
    return;
  }

  std::string fileName = "log-" + m_name + "-" + CurrentDateTime() + ".txt";

  m_logFile.open(fileName.c_str());

  if (m_logFile.fail()) {
    std::cout << "Could not open file `" << fileName << "` for writing."
              << std::endl;
    return;
  }

  for (size_t i = 0; i < m_cells.size(); i++) {
    m_logFile << "\"" << m_cells[i]->GetName() << "\",";
  }
  m_logFile << std::endl;

  m_active = true;
}

void LogSpreadsheet::Stop() {
  if (m_active) {
    m_logFile.close();
    m_active = false;
  }
}

void LogSpreadsheet::Periodic() {
  if (m_active) {
    std::chrono::milliseconds timestamp =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch());
    m_timestampCell.Log(timestamp.count());
    WriteRow();
  }
}

void LogSpreadsheet::WriteRow() {
  for (size_t i = 0; i < m_cells.size(); i++) {
    m_logFile << "\"" << m_cells[i]->GetContent() << "\",";
  }
  m_logFile << std::endl;
}

std::string LogSpreadsheet::CurrentDateTime() {
  // Get current date/time, format is YYYY-MM-DD.HH:mm:ss
  time_t now = time(0);
  struct tm time;
  char buf[80];
  time = *localtime(&now);
  strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &time);

  return buf;
}