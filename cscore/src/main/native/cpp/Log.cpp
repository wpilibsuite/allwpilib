/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Log.h"

#include <llvm/Path.h>
#include <llvm/SmallString.h>
#include <llvm/StringRef.h>
#include <llvm/raw_ostream.h>

using namespace cs;

static void def_log_func(unsigned int level, const char* file,
                         unsigned int line, const char* msg) {
  llvm::SmallString<128> buf;
  llvm::raw_svector_ostream oss(buf);
  if (level == 20) {
    oss << "CS: " << msg << '\n';
    llvm::errs() << oss.str();
    return;
  }

  llvm::StringRef levelmsg;
  if (level >= 50)
    levelmsg = "CRITICAL: ";
  else if (level >= 40)
    levelmsg = "ERROR: ";
  else if (level >= 30)
    levelmsg = "WARNING: ";
  else
    return;
  oss << "CS: " << levelmsg << msg << " (" << llvm::sys::path::filename(file)
      << ':' << line << ")\n";
  llvm::errs() << oss.str();
}

Logger::Logger() { SetDefaultLogger(); }

Logger::~Logger() {}

void Logger::SetDefaultLogger() { SetLogger(def_log_func); }
