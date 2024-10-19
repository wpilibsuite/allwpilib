// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "LocalTopic.h"

#include <algorithm>

#include "local/LocalDataLogger.h"

using namespace nt::local;

void LocalTopic::StartStopDataLog(LocalDataLogger* logger, int64_t timestamp,
                                  bool publish) {
  auto it = std::find_if(
      datalogs.begin(), datalogs.end(),
      [&](const auto& elem) { return elem.logger == logger->handle; });
  if (publish && it == datalogs.end()) {
    datalogs.emplace_back(logger->log, logger->Start(this, timestamp),
                          logger->handle);
    datalogType = type;
  } else if (!publish && it != datalogs.end()) {
    it->Finish(timestamp);
    datalogType = NT_UNASSIGNED;
    datalogs.erase(it);
  }
}

void LocalTopic::UpdateDataLogProperties() {
  if (!datalogs.empty()) {
    auto now = Now();
    auto metadata = LocalDataLoggerEntry::MakeMetadata(propertiesStr);
    for (auto&& datalog : datalogs) {
      datalog.SetMetadata(metadata, now);
    }
  }
}
