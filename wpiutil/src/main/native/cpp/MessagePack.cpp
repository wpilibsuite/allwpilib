// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/MessagePack.h"

#include <string>

using namespace mpack;

mpack_error_t mpack::mpack_expect_str(mpack_reader_t* reader, std::string* out,
                                      uint32_t maxLen) {
  uint32_t count = mpack_expect_str_max(reader, maxLen);
  mpack_error_t err = mpack_reader_error(reader);
  if (err != mpack_ok) {
    return err;
  }
  const char* bytes = mpack_read_bytes_inplace(reader, count);
  if (bytes) {
    out->assign(bytes, count);
  } else {
    return mpack_reader_error(reader);
  }
  mpack_done_str(reader);
  return mpack_ok;
}

mpack_error_t mpack::mpack_read_str(mpack_reader_t* reader, mpack_tag_t* tag,
                                    std::string* out, uint32_t maxLen) {
  uint32_t count = mpack_tag_str_length(tag);
  mpack_error_t err = mpack_reader_error(reader);
  if (err != mpack_ok) {
    return err;
  }
  if (count > maxLen) {
    mpack_reader_flag_error(reader, mpack_error_too_big);
    return mpack_error_too_big;
  }
  const char* bytes = mpack_read_bytes_inplace(reader, count);
  if (bytes) {
    out->assign(bytes, count);
  } else {
    return mpack_reader_error(reader);
  }
  mpack_done_str(reader);
  return mpack_ok;
}
