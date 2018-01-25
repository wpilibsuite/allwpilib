/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Error.h"

#include <llvm/Path.h>

#include "DriverStation.h"
#include "Timer.h"
#include "Utility.h"

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

void Error::Set(Code code, const llvm::Twine& contextMessage,
                llvm::StringRef filename, llvm::StringRef function,
                int lineNumber, const ErrorBase* originatingObject) {
  bool report = true;

  if (code == m_code && GetTime() - m_timestamp < 1) {
    report = false;
  }

  m_code = code;
  m_message = contextMessage.str();
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
  DriverStation::ReportError(
      true, m_code, m_message,
      m_function + llvm::Twine(" [") + llvm::sys::path::filename(m_filename) +
          llvm::Twine(':') + llvm::Twine(m_lineNumber) + llvm::Twine(']'),
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
