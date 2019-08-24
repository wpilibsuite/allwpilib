/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/logging/CSVLogCell.h"

#include <algorithm>
#include <iostream>

using namespace frc;

CSVLogCell::CSVLogCell(std::string name) {
  if (IsStringValid(name)) {
    m_name = name;
  }
}

std::string CSVLogCell::GetName() { return m_name; }

std::string CSVLogCell::GetContent() { return m_content; }

void CSVLogCell::ClearCell() { m_content.clear(); }

bool CSVLogCell::IsStringValid(std::string string) {
  if (string.find('\"') != std::string::npos) {
    std::cout << "CSVLogCell's name/content cannot contain double quotes"
              << std::endl;
    return false;
  } else if (string.find('\n') != std::string::npos) {
    std::cout << "CSVLogCell's name/content cannot contain newlines"
              << std::endl;
    return false;
  }
  return true;
}
