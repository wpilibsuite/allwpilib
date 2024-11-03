// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/protobuf/Protobuf.h"

#include <string>
#include <vector>

#include <fmt/format.h>

#include "wpi/SmallVector.h"

using namespace wpi;

std::string detail::GetTypeString(const pb_msgdesc_t* msg) {
  return fmt::format("proto:{}", msg->proto_name);
}

void detail::ForEachProtobufDescriptor(
    const pb_msgdesc_t* msg,
    function_ref<bool(std::string_view filename)> exists,
    function_ref<void(std::string_view filename,
                      std::span<const uint8_t> descriptor)>
        fn) {
  std::string name = fmt::format("proto:{}", msg->file_descriptor.file_name);
  if (exists(name)) {
    return;
  }
  const pb_msgdesc_t* const* nested = msg->submsg_info;
  while (*nested) {
    ForEachProtobufDescriptor(*nested, exists, fn);
    nested++;
  }
  fn(name, msg->file_descriptor.file_descriptor);
}

bool detail::WriteFromSmallVector(pb_ostream_t* stream, const pb_byte_t* buf,
                                  size_t count) {
  SmallVectorType* vec = reinterpret_cast<SmallVectorType*>(stream->state);
  vec->append(buf, buf + count);
  return true;
}

bool detail::WriteFromStdVector(pb_ostream_t* stream, const pb_byte_t* buf,
                                size_t count) {
  StdVectorType* vec = reinterpret_cast<StdVectorType*>(stream->state);
  vec->insert(vec->end(), buf, buf + count);
  return true;
}
