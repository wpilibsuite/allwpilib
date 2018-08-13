/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/ErrorBase.h"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <set>

#include <hal/HAL.h>
#include <wpi/Format.h>
#include <wpi/SmallString.h>
#include <wpi/raw_ostream.h>

#define WPI_ERRORS_DEFINE_STRINGS
#include "frc/WPIErrors.h"

using namespace frc;

static wpi::mutex globalErrorsMutex;
static std::set<Error> globalErrors;

ErrorBase::ErrorBase() { HAL_Initialize(500, 0); }

Error& ErrorBase::GetError() { return m_error; }

const Error& ErrorBase::GetError() const { return m_error; }

void ErrorBase::ClearError() const { m_error.Clear(); }

void ErrorBase::SetErrnoError(const wpi::Twine& contextMessage,
                              wpi::StringRef filename, wpi::StringRef function,
                              int lineNumber) const {
  wpi::SmallString<128> buf;
  wpi::raw_svector_ostream err(buf);
  int errNo = errno;
  if (errNo == 0) {
    err << "OK: ";
  } else {
    err << std::strerror(errNo) << " (" << wpi::format_hex(errNo, 10, true)
        << "): ";
  }

  // Set the current error information for this object.
  m_error.Set(-1, err.str() + contextMessage, filename, function, lineNumber,
              this);

  // Update the global error if there is not one already set.
  std::lock_guard<wpi::mutex> mutex(globalErrorsMutex);
  globalErrors.insert(m_error);
}

void ErrorBase::SetImaqError(int success, const wpi::Twine& contextMessage,
                             wpi::StringRef filename, wpi::StringRef function,
                             int lineNumber) const {
  // If there was an error
  if (success <= 0) {
    // Set the current error information for this object.
    m_error.Set(success, wpi::Twine(success) + ": " + contextMessage, filename,
                function, lineNumber, this);

    // Update the global error if there is not one already set.
    std::lock_guard<wpi::mutex> mutex(globalErrorsMutex);
    globalErrors.insert(m_error);
  }
}

void ErrorBase::SetError(Error::Code code, const wpi::Twine& contextMessage,
                         wpi::StringRef filename, wpi::StringRef function,
                         int lineNumber) const {
  //  If there was an error
  if (code != 0) {
    //  Set the current error information for this object.
    m_error.Set(code, contextMessage, filename, function, lineNumber, this);

    // Update the global error if there is not one already set.
    std::lock_guard<wpi::mutex> mutex(globalErrorsMutex);
    globalErrors.insert(m_error);
  }
}

void ErrorBase::SetErrorRange(Error::Code code, int32_t minRange,
                              int32_t maxRange, int32_t requestedValue,
                              const wpi::Twine& contextMessage,
                              wpi::StringRef filename, wpi::StringRef function,
                              int lineNumber) const {
  //  If there was an error
  if (code != 0) {
    //  Set the current error information for this object.
    m_error.Set(code,
                contextMessage + ", Minimum Value: " + wpi::Twine(minRange) +
                    ", MaximumValue: " + wpi::Twine(maxRange) +
                    ", Requested Value: " + wpi::Twine(requestedValue),
                filename, function, lineNumber, this);

    // Update the global error if there is not one already set.
    std::lock_guard<wpi::mutex> mutex(globalErrorsMutex);
    globalErrors.insert(m_error);
  }
}

void ErrorBase::SetWPIError(const wpi::Twine& errorMessage, Error::Code code,
                            const wpi::Twine& contextMessage,
                            wpi::StringRef filename, wpi::StringRef function,
                            int lineNumber) const {
  //  Set the current error information for this object.
  m_error.Set(code, errorMessage + ": " + contextMessage, filename, function,
              lineNumber, this);

  // Update the global error if there is not one already set.
  std::lock_guard<wpi::mutex> mutex(globalErrorsMutex);
  globalErrors.insert(m_error);
}

void ErrorBase::CloneError(const ErrorBase& rhs) const {
  m_error = rhs.GetError();
}

bool ErrorBase::StatusIsFatal() const { return m_error.GetCode() < 0; }

void ErrorBase::SetGlobalError(Error::Code code,
                               const wpi::Twine& contextMessage,
                               wpi::StringRef filename, wpi::StringRef function,
                               int lineNumber) {
  // If there was an error
  if (code != 0) {
    std::lock_guard<wpi::mutex> mutex(globalErrorsMutex);

    // Set the current error information for this object.
    globalErrors.emplace(code, contextMessage, filename, function, lineNumber,
                         nullptr);
  }
}

void ErrorBase::SetGlobalWPIError(const wpi::Twine& errorMessage,
                                  const wpi::Twine& contextMessage,
                                  wpi::StringRef filename,
                                  wpi::StringRef function, int lineNumber) {
  std::lock_guard<wpi::mutex> mutex(globalErrorsMutex);
  globalErrors.emplace(-1, errorMessage + ": " + contextMessage, filename,
                       function, lineNumber, nullptr);
}

const Error& ErrorBase::GetGlobalError() {
  std::lock_guard<wpi::mutex> mutex(globalErrorsMutex);
  return *globalErrors.begin();
}
