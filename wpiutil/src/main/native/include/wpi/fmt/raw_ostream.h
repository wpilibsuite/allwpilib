// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_FMT_RAW_OSTREAM_H_
#define WPIUTIL_WPI_FMT_RAW_OSTREAM_H_

#include <fmt/format.h>

#include "wpi/raw_ostream.h"

FMT_BEGIN_NAMESPACE

inline void vprint(wpi::raw_ostream& os, string_view format_str,
                   fmt::format_args args) {
  memory_buffer buffer;
  detail::vformat_to(buffer, format_str, args);
  os.write(buffer.data(), buffer.size());
}

/**
 * Prints formatted data to the stream *os*.
 */
template <typename S, typename... Args>
void print(wpi::raw_ostream& os, const S& format_str, Args&&... args) {
  vprint(os, format_str, fmt::make_format_args(args...));
}

FMT_END_NAMESPACE

#endif  // WPIUTIL_WPI_FMT_RAW_OSTREAM_H_
