/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/logging/LogCell.h"

#include <algorithm>
#include <iostream>

using namespace frc;

LogCell::LogCell(std::string name) {
  if (IsStringValid(name)) {
    m_name = name;
  }
}

std::string LogCell::GetName() { return m_name; }

std::string LogCell::GetContent() { return m_content; }

void LogCell::ClearCell() { m_content.clear(); }

bool LogCell::IsStringValid(std::string string) {
  if (string.find('\"') != std::string::npos) {
    std::cout << "LogCell's name/content cannot contain double quotes"
              << std::endl;
    return false;
  } else if (string.find('\n') != std::string::npos) {
    std::cout << "LogCell's name/content cannot contain newlines" << std::endl;
    return false;
  }
  return true;
}
