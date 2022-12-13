// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "NetworkLoopQueue.h"

#include <wpi/Logger.h>

using namespace nt::net;

static constexpr size_t kMaxSize = 2 * 1024 * 1024;

void NetworkLoopQueue::SetValue(NT_Publisher pubHandle, const Value& value) {
  std::scoped_lock lock{m_mutex};
  switch (value.type()) {
    case NT_STRING:
      m_size += value.GetString().size();  // imperfect but good enough
      break;
    case NT_RAW:
      m_size += value.GetRaw().size_bytes();
      break;
    case NT_BOOLEAN_ARRAY:
      m_size += value.GetBooleanArray().size_bytes();
      break;
    case NT_INTEGER_ARRAY:
      m_size += value.GetIntegerArray().size_bytes();
      break;
    case NT_FLOAT_ARRAY:
      m_size += value.GetFloatArray().size_bytes();
      break;
    case NT_DOUBLE_ARRAY:
      m_size += value.GetDoubleArray().size_bytes();
      break;
    case NT_STRING_ARRAY: {
      auto arr = value.GetStringArray();
      m_size += arr.size_bytes();
      for (auto&& s : arr) {
        m_size += s.capacity();
      }
      break;
    }
    default:
      break;
  }
  m_size += sizeof(ClientMessage);
  if (m_size > kMaxSize) {
    if (!m_sizeErrored) {
      WPI_ERROR(m_logger, "NT: dropping value set due to memory limits");
      m_sizeErrored = true;
    }
    return;  // avoid potential out of memory
  }
  m_queue.emplace_back(ClientMessage{ClientValueMsg{pubHandle, value}});
}
