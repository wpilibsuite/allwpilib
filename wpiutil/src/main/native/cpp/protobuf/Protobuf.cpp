// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/protobuf/Protobuf.h"

#include <string>
#include <vector>

#include "wpi/SmallVector.h"

using namespace wpi;

bool ProtoOutputStream::WriteFromSmallVector(pb_ostream_t* stream,
                                             const pb_byte_t* buf,
                                             size_t count) {
  SmallVectorType* vec = reinterpret_cast<SmallVectorType*>(stream->state);
  vec->append(buf, buf + count);
  return true;
}

bool ProtoOutputStream::WriteFromStdVector(pb_ostream_t* stream,
                                           const pb_byte_t* buf, size_t count) {
  StdVectorType* vec = reinterpret_cast<StdVectorType*>(stream->state);
  vec->insert(vec->end(), buf, buf + count);
  return true;
}
