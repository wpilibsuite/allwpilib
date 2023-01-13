// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <span>
#include <string>
#include <string_view>

#include "wpi/mpack.h"

namespace mpack {

inline void mpack_write_str(mpack_writer_t* writer, std::string_view str) {
  mpack_write_str(writer, str.data(), str.size());
}

inline void mpack_write_bytes(mpack_writer_t* writer,
                              std::span<const uint8_t> data) {
  mpack_write_bytes(writer, reinterpret_cast<const char*>(data.data()),
                    data.size());
}

inline void mpack_reader_init_data(mpack_reader_t* reader,
                                   std::span<const uint8_t> data) {
  mpack_reader_init_data(reader, reinterpret_cast<const char*>(data.data()),
                         data.size());
}

mpack_error_t mpack_expect_str(mpack_reader_t* reader, std::string* out,
                               uint32_t maxLen = 1024);

mpack_error_t mpack_read_str(mpack_reader_t* reader, mpack_tag_t* tag,
                             std::string* out, uint32_t maxLen = 1024);

}  // namespace mpack
