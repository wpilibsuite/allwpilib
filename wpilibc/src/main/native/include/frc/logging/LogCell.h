/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <mutex>
#include <string>

namespace frc {

/**
 * Represents a column in a LogSpreadsheet.
 *
 * For the column to be printed, you must register the instance of a LogCell
 * in a LogSpreadsheet using LogSpreadsheet.RegisterCell().
 */
class LogCell {
 public:
  /**
   * Instantiate a LogCell passing in the name of the column.
   *
   * @param name The name of the column.
   */
  explicit LogCell(std::string name);

  /**
   * Log any type of value that can be converted into a string.
   *
   * @param value The value to be logged in the cell.
   */
  template <typename T>
  void Log(T value) {
    AcquireLock();
    m_content = std::to_string(value);
    ReleaseLock();
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

  /**
   * Acquire the lock on the cell's content.
   */
  void AcquireLock();

  /**
   * Release the lock on the cell's content.
   */
  void ReleaseLock();

 private:
  std::string m_name;
  std::string m_content;
  std::mutex m_mutex;
};

}  // namespace frc
