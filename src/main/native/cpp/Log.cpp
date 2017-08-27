/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Log.h"

#include "llvm/Path.h"
#include "llvm/StringRef.h"
#include "llvm/raw_ostream.h"

using namespace nt;

ATOMIC_STATIC_INIT(Logger)

static void def_log_func(unsigned int level, const char* file,
                         unsigned int line, const char* msg) {
  if (level == 20) {
    llvm::errs() << "NT: " << msg << '\n';
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
  llvm::errs() << "NT: " << levelmsg << msg << " ("
               << llvm::sys::path::filename(file) << ':' << line << ")\n";
}

Logger::Logger() { SetLogger(def_log_func); }

Logger::~Logger() {}
