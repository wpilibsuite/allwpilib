/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/logging/LogSpreadsheet.h"

#include <chrono>
#include <cstdio>
#include <ctime>
#include <iostream>

using namespace frc;

LogSpreadsheet::LogSpreadsheet(std::string name)
    : m_name(name),
      m_cells(),
      m_timestampCell("Timestamp (ms)"),
      m_time(0),
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

  m_time = std::time(0);
  std::string filename = CreateFilename(m_time);

  m_logFile.open(filename.c_str());

  if (m_logFile.fail()) {
    std::cout << "Could not open file `" << filename << "` for writing."
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
    UpdateFilename();
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

void LogSpreadsheet::UpdateFilename() {
  std::time_t newTime = std::time(0);
  // If the difference between the two timestamps is more than 24 hours
  if (std::difftime(newTime, m_time) > 86400) {
    std::rename(CreateFilename(m_time).c_str(),
                CreateFilename(newTime).c_str());
  }

  m_time = newTime;
}

std::string LogSpreadsheet::CreateFilename(std::time_t time) {
  // Get current date/time, format is YYYY-MM-DD.HH:mm:ss
  struct tm localTime = *std::localtime(&time);
  char datetime[80];
  std::strftime(datetime, sizeof(datetime), "%Y-%m-%d.%X", &localTime);

  return "log-" + m_name + "-" + datetime + ".txt";
}
