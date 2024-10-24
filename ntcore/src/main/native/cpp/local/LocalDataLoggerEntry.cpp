// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "LocalDataLoggerEntry.h"

#include <string>
#include <string_view>

#include <fmt/format.h>
#include <wpi/StringExtras.h>

#include "networktables/NetworkTableValue.h"

using namespace nt::local;

std::string LocalDataLoggerEntry::MakeMetadata(std::string_view properties) {
  return fmt::format("{{\"properties\":{},\"source\":\"NT\"}}", properties);
}

void LocalDataLoggerEntry::Append(const Value& v) {
  auto time = v.time();
  switch (v.type()) {
    case NT_BOOLEAN:
      log->AppendBoolean(entry, v.GetBoolean(), time);
      break;
    case NT_INTEGER:
      log->AppendInteger(entry, v.GetInteger(), time);
      break;
    case NT_FLOAT:
      log->AppendFloat(entry, v.GetFloat(), time);
      break;
    case NT_DOUBLE:
      log->AppendDouble(entry, v.GetDouble(), time);
      break;
    case NT_STRING:
      log->AppendString(entry, v.GetString(), time);
      break;
    case NT_RAW: {
      auto val = v.GetRaw();
      log->AppendRaw(entry,
                     {reinterpret_cast<const uint8_t*>(val.data()), val.size()},
                     time);
      break;
    }
    case NT_BOOLEAN_ARRAY:
      log->AppendBooleanArray(entry, v.GetBooleanArray(), time);
      break;
    case NT_INTEGER_ARRAY:
      log->AppendIntegerArray(entry, v.GetIntegerArray(), time);
      break;
    case NT_FLOAT_ARRAY:
      log->AppendFloatArray(entry, v.GetFloatArray(), time);
      break;
    case NT_DOUBLE_ARRAY:
      log->AppendDoubleArray(entry, v.GetDoubleArray(), time);
      break;
    case NT_STRING_ARRAY:
      log->AppendStringArray(entry, v.GetStringArray(), time);
      break;
    default:
      break;
  }
}
