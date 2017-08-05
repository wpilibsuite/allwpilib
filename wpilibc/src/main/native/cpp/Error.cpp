/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Error.h"

#include "DriverStation.h"
#include "Timer.h"
#include "Utility.h"
#include "llvm/SmallString.h"
#include "llvm/raw_ostream.h"

using namespace frc;

void Error::Clone(const Error& error) {
  m_code = error.m_code;
  m_message = error.m_message;
  m_filename = error.m_filename;
  m_function = error.m_function;
  m_lineNumber = error.m_lineNumber;
  m_originatingObject = error.m_originatingObject;
  m_timestamp = error.m_timestamp;
}

Error::Code Error::GetCode() const { return m_code; }

std::string Error::GetMessage() const { return m_message; }

std::string Error::GetFilename() const { return m_filename; }

std::string Error::GetFunction() const { return m_function; }

int Error::GetLineNumber() const { return m_lineNumber; }

const ErrorBase* Error::GetOriginatingObject() const {
  return m_originatingObject;
}

double Error::GetTimestamp() const { return m_timestamp; }

void Error::Set(Code code, llvm::StringRef contextMessage,
                llvm::StringRef filename, llvm::StringRef function,
                int lineNumber, const ErrorBase* originatingObject) {
  bool report = true;

  if (code == m_code && GetTime() - m_timestamp < 1) {
    report = false;
  }

  m_code = code;
  m_message = contextMessage;
  m_filename = filename;
  m_function = function;
  m_lineNumber = lineNumber;
  m_originatingObject = originatingObject;

  if (report) {
    m_timestamp = GetTime();
    Report();
  }
}

void Error::Report() {
  llvm::SmallString<128> buf;
  llvm::raw_svector_ostream locStream(buf);
  locStream << m_function << " [";

#if defined(_WIN32)
  const int MAX_DIR = 100;
  char basename[MAX_DIR];
  _splitpath_s(m_filename.c_str(), nullptr, 0, basename, MAX_DIR, nullptr, 0,
               nullptr, 0);
  locStream << basename;
#else
  locStream << basename(m_filename.c_str());
#endif
  locStream << ":" << m_lineNumber << "]";

  DriverStation::ReportError(true, m_code, m_message, locStream.str(),
                             GetStackTrace(4));
}

void Error::Clear() {
  m_code = 0;
  m_message = "";
  m_filename = "";
  m_function = "";
  m_lineNumber = 0;
  m_originatingObject = nullptr;
  m_timestamp = 0.0;
}
