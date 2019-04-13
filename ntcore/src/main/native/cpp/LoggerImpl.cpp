/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "LoggerImpl.h"

#include <wpi/Path.h>
#include <wpi/SmallString.h>
#include <wpi/StringRef.h>
#include <wpi/raw_ostream.h>

using namespace nt;

static void DefaultLogger(unsigned int level, const char* file,
                          unsigned int line, const char* msg) {
  wpi::SmallString<128> buf;
  wpi::raw_svector_ostream oss(buf);
  if (level == 20) {
    oss << "NT: " << msg << '\n';
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
  oss << "NT: " << levelmsg << msg << " (" << file << ':' << line << ")\n";
  wpi::errs() << oss.str();
}

LoggerImpl::LoggerImpl(int inst) : m_inst(inst) {}

void LoggerImpl::Start() { DoStart(m_inst); }

unsigned int LoggerImpl::Add(
    std::function<void(const LogMessage& msg)> callback, unsigned int min_level,
    unsigned int max_level) {
  return DoAdd(callback, min_level, max_level);
}

unsigned int LoggerImpl::AddPolled(unsigned int poller_uid,
                                   unsigned int min_level,
                                   unsigned int max_level) {
  return DoAdd(poller_uid, min_level, max_level);
}

unsigned int LoggerImpl::GetMinLevel() {
  auto thr = GetThread();
  if (!thr) return NT_LOG_INFO;
  unsigned int level = NT_LOG_INFO;
  for (size_t i = 0; i < thr->m_listeners.size(); ++i) {
    const auto& listener = thr->m_listeners[i];
    if (listener && listener.min_level < level) level = listener.min_level;
  }
  return level;
}

void LoggerImpl::Log(unsigned int level, const char* file, unsigned int line,
                     const char* msg) {
  // this is safe because it's null terminated and always the end
  const char* filename = wpi::sys::path::filename(file).data();
  {
    auto thr = GetThread();
    if (!thr || thr->m_listeners.empty())
      DefaultLogger(level, filename, line, msg);
  }
  Send(UINT_MAX, 0, level, filename, line, msg);
}
