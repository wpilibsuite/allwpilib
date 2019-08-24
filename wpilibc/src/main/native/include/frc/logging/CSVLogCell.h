/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>

namespace frc {

/**
 * Represents a column in a CSVLogFile.
 *
 * For the column to be printed, you must register the instance of a
 * CSVLogCell in a CSVLogFile using CSVLogFile.RegisterCell().
 */
class CSVLogCell {
 public:
  /**
   * Instantiate a CSVLogCell passing in the name of the column.
   *
   * @param name The name of the column.
   */
  explicit CSVLogCell(std::string name);

  /**
   * Log any type of value that can be converted into a string.
   *
   * @param value The value to be logged in the cell.
   */
  template <typename T>
  void Log(T value) {
    if (IsStringValid(std::to_string(value))) {
      m_content = std::to_string(value);
    }
  }

  /**
   * Gets the name of the cell.
   *
   * @return The name of the cell.
   */
  std::string GetName();

  /**
   * Get the content of the cell.
   *
   * @return The content of the cell.
   */
  std::string GetContent();

  /**
   * Clear the cell so its content is empty.
   */
  void ClearCell();

 private:
  bool IsStringValid(std::string string);

  std::string m_name;
  std::string m_content;
};

}  // namespace frc
