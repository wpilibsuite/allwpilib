// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <functional>
#include <string_view>

namespace wpi::log {
class DataLogReader;
}  // namespace wpi::log
namespace wpi::util {
class raw_ostream;
}  // namespace wpi::util

namespace dlt {

/** CSV output layout. */
enum class CsvStyle { LIST, TABLE };

/** Returns true for entry names to include in the output. */
using EntryFilter = std::function<bool(std::string_view)>;

/**
 * Writes a quoted CSV field, escaping embedded quotes.
 *
 * @param os Output stream.
 * @param value Field contents.
 */
void WriteCsvString(wpi::util::raw_ostream& os, std::string_view value);

/**
 * Exports a log using the GUI's CSV formats. Table columns are sorted by name;
 * values are decoded using each record's active entry type.
 *
 * @param reader Input log, which must remain alive throughout the export.
 * @param os Output stream.
 * @param style List or table layout.
 * @param timestampFuzziness Maximum table row timestamp span, in nanoseconds.
 * @param selected Entry selector; an empty function selects all entries.
 */
void ExportCsv(const wpi::log::DataLogReader& reader,
               wpi::util::raw_ostream& os, CsvStyle style,
               int64_t timestampFuzziness = 0,
               const EntryFilter& selected = {});

}  // namespace dlt
