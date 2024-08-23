// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <google/protobuf/message.h>

namespace wpi {
template <typename T>
inline T* CreateMessage(google::protobuf::Arena* arena) {
#if GOOGLE_PROTOBUF_VERSION < 4000000
  return google::protobuf::Arena::CreateMessage<T>(arena);
#else
  return google::protobuf::Arena::Create<T>(arena);
#endif
}
}  // namespace wpi
