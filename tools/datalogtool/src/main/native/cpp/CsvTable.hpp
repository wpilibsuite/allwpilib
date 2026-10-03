// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stddef.h>
#include <stdint.h>

#include <algorithm>
#include <string>
#include <string_view>
#include <vector>

#include "wpi/util/fmt/raw_ostream.hpp"
#include "wpi/util/print.hpp"
#include "wpi/util/raw_ostream.hpp"

/** Buffers consecutive CSV values into rows with a bounded timestamp span. */
class CsvTable {
 public:
  /**
   * Constructs a table row writer. Call Flush() after adding the final value.
   *
   * @param os Output stream (the caller writes the header).
   * @param columns Number of data columns, excluding the timestamp.
   * @param timestampFuzziness Maximum timestamp span of a row, in nanoseconds.
   */
  CsvTable(wpi::util::raw_ostream& os, size_t columns,
           int64_t timestampFuzziness)
      : m_os{os},
        m_values(columns),
        m_timestampFuzziness{std::max(int64_t{0}, timestampFuzziness)} {}

  /**
   * Adds a value, writing the pending row if its timestamp span would exceed
   * the fuzziness. The last value added to each column wins within a row.
   *
   * @param timestamp Timestamp, in nanoseconds.
   * @param column Zero-based data column index; must be less than columns.
   * @param value CSV-formatted value, including any necessary escaping.
   */
  void Add(int64_t timestamp, size_t column, std::string_view value) {
    // Bound the entire row's span, even when timestamps arrive out of order.
    // Unsigned subtraction avoids overflow for widely separated timestamps.
    if (m_pending &&
        static_cast<uint64_t>(std::max(timestamp, m_maxTimestamp)) -
                static_cast<uint64_t>(std::min(timestamp, m_minTimestamp)) >
            static_cast<uint64_t>(m_timestampFuzziness)) {
      Flush();
    }
    if (!m_pending) {
      m_minTimestamp = timestamp;
      m_maxTimestamp = timestamp;
      m_pending = true;
    } else {
      m_minTimestamp = std::min(timestamp, m_minTimestamp);
      m_maxTimestamp = std::max(timestamp, m_maxTimestamp);
    }
    m_values[column] = value;
  }

  /** Writes the pending row using its earliest timestamp, then clears it. */
  void Flush() {
    if (!m_pending) {
      return;
    }
    wpi::util::print(m_os, "{}", m_minTimestamp / 1'000'000'000.0);
    for (auto& value : m_values) {
      m_os << ',' << value;
      value.clear();
    }
    m_os << '\n';
    m_pending = false;
  }

 private:
  wpi::util::raw_ostream& m_os;
  std::vector<std::string> m_values;
  int64_t m_timestampFuzziness;
  int64_t m_minTimestamp = 0;
  int64_t m_maxTimestamp = 0;
  bool m_pending = false;
};
