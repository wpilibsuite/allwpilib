/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Instance.h"

#include <wpi/Path.h>
#include <wpi/SmallString.h>
#include <wpi/StringRef.h>
#include <wpi/raw_ostream.h>

using namespace cs;

static void def_log_func(unsigned int level, const char* file,
                         unsigned int line, const char* msg) {
  wpi::SmallString<128> buf;
  wpi::raw_svector_ostream oss(buf);
  if (level == 20) {
    oss << "CS: " << msg << '\n';
    wpi::errs() << oss.str();
    return;
  }

  wpi::StringRef levelmsg;
  if (level >= 50)
    levelmsg = "CRITICAL: ";
  else if (level >= 40)
    levelmsg = "ERROR: ";
  else if (level >= 30)
    levelmsg = "WARNING: ";
  else
    return;
  oss << "CS: " << levelmsg << msg << " (" << wpi::sys::path::filename(file)
      << ':' << line << ")\n";
  wpi::errs() << oss.str();
}

Instance::Instance() : telemetry(notifier), network_listener(logger, notifier) {
  SetDefaultLogger();
}

Instance::~Instance() {}

Instance& Instance::GetInstance() {
  static Instance inst;
  return inst;
}

void Instance::SetDefaultLogger() { logger.SetLogger(def_log_func); }

std::pair<CS_Source, std::shared_ptr<SourceData>> Instance::FindSource(
    const SourceImpl& source) {
  return sources.FindIf(
      [&](const SourceData& data) { return data.source.get() == &source; });
}

std::pair<CS_Sink, std::shared_ptr<SinkData>> Instance::FindSink(
    const SinkImpl& sink) {
  return sinks.FindIf(
      [&](const SinkData& data) { return data.sink.get() == &sink; });
}
