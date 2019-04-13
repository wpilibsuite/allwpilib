/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Error.h"

#include <wpi/Path.h>

#include "frc/DriverStation.h"
#include "frc/Timer.h"
#include "frc/Utility.h"

using namespace frc;

Error::Error(Code code, const wpi::Twine& contextMessage,
             wpi::StringRef filename, wpi::StringRef function, int lineNumber,
             const ErrorBase* originatingObject) {
  Set(code, contextMessage, filename, function, lineNumber, originatingObject);
}

bool Error::operator<(const Error& rhs) const {
  if (m_code < rhs.m_code) {
    return true;
  } else if (m_message < rhs.m_message) {
    return true;
  } else if (m_filename < rhs.m_filename) {
    return true;
  } else if (m_function < rhs.m_function) {
    return true;
  } else if (m_lineNumber < rhs.m_lineNumber) {
    return true;
  } else if (m_originatingObject < rhs.m_originatingObject) {
    return true;
  } else if (m_timestamp < rhs.m_timestamp) {
    return true;
  } else {
    return false;
  }
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

void Error::Set(Code code, const wpi::Twine& contextMessage,
                wpi::StringRef filename, wpi::StringRef function,
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
      m_function + wpi::Twine(" [") + wpi::sys::path::filename(m_filename) +
          wpi::Twine(':') + wpi::Twine(m_lineNumber) + wpi::Twine(']'),
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
