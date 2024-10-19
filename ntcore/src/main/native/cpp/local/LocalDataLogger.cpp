// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "LocalDataLogger.h"

#include <fmt/format.h>
#include <wpi/DataLog.h>
#include <wpi/StringExtras.h>

#include "local/LocalDataLoggerEntry.h"
#include "local/LocalTopic.h"

using namespace nt::local;

int LocalDataLogger::Start(LocalTopic* topic, int64_t time) {
  std::string_view typeStr = topic->typeStr;
  // NT and DataLog use different standard representations for int and int[]
  if (typeStr == "int") {
    typeStr = "int64";
  } else if (typeStr == "int[]") {
    typeStr = "int64[]";
  }
  return log.Start(
      fmt::format(
          "{}{}", logPrefix,
          wpi::remove_prefix(topic->name, prefix).value_or(topic->name)),
      typeStr, LocalDataLoggerEntry::MakeMetadata(topic->propertiesStr), time);
}
