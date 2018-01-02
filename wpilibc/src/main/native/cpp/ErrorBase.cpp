/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ErrorBase.h"

#include <cerrno>
#include <cstdio>
#include <cstring>

#include <HAL/HAL.h>

#define WPI_ERRORS_DEFINE_STRINGS
#include "WPIErrors.h"
#include "llvm/Format.h"
#include "llvm/SmallString.h"
#include "llvm/raw_ostream.h"

using namespace frc;

wpi::mutex ErrorBase::_globalErrorMutex;
Error ErrorBase::_globalError;

ErrorBase::ErrorBase() { HAL_Initialize(500, 0); }

/**
 * @brief Retrieve the current error.
 *
 * Get the current error information associated with this sensor.
 */
Error& ErrorBase::GetError() { return m_error; }

const Error& ErrorBase::GetError() const { return m_error; }

/**
 * @brief Clear the current error information associated with this sensor.
 */
void ErrorBase::ClearError() const { m_error.Clear(); }

/**
 * @brief Set error information associated with a C library call that set an
 *        error to the "errno" global variable.
 *
 * @param contextMessage A custom message from the code that set the error.
 * @param filename       Filename of the error source
 * @param function       Function of the error source
 * @param lineNumber     Line number of the error source
 */
void ErrorBase::SetErrnoError(const llvm::Twine& contextMessage,
                              llvm::StringRef filename,
                              llvm::StringRef function, int lineNumber) const {
  llvm::SmallString<128> buf;
  llvm::raw_svector_ostream err(buf);
  int errNo = errno;
  if (errNo == 0) {
    err << "OK: ";
  } else {
    err << std::strerror(errNo) << " (" << llvm::format_hex(errNo, 10, true)
        << "): ";
  }

  // Set the current error information for this object.
  m_error.Set(-1, err.str() + contextMessage, filename, function, lineNumber,
              this);

  // Update the global error if there is not one already set.
  std::lock_guard<wpi::mutex> mutex(_globalErrorMutex);
  if (_globalError.GetCode() == 0) {
    _globalError.Clone(m_error);
  }
}

/**
 * @brief Set the current error information associated from the nivision Imaq
 *        API.
 *
 * @param success        The return from the function
 * @param contextMessage A custom message from the code that set the error.
 * @param filename       Filename of the error source
 * @param function       Function of the error source
 * @param lineNumber     Line number of the error source
 */
void ErrorBase::SetImaqError(int success, const llvm::Twine& contextMessage,
                             llvm::StringRef filename, llvm::StringRef function,
                             int lineNumber) const {
  // If there was an error
  if (success <= 0) {
    // Set the current error information for this object.
    m_error.Set(success, llvm::Twine(success) + ": " + contextMessage, filename,
                function, lineNumber, this);

    // Update the global error if there is not one already set.
    std::lock_guard<wpi::mutex> mutex(_globalErrorMutex);
    if (_globalError.GetCode() == 0) {
      _globalError.Clone(m_error);
    }
  }
}

/**
 * @brief Set the current error information associated with this sensor.
 *
 * @param code           The error code
 * @param contextMessage A custom message from the code that set the error.
 * @param filename       Filename of the error source
 * @param function       Function of the error source
 * @param lineNumber     Line number of the error source
 */
void ErrorBase::SetError(Error::Code code, const llvm::Twine& contextMessage,
                         llvm::StringRef filename, llvm::StringRef function,
                         int lineNumber) const {
  //  If there was an error
  if (code != 0) {
    //  Set the current error information for this object.
    m_error.Set(code, contextMessage, filename, function, lineNumber, this);

    // Update the global error if there is not one already set.
    std::lock_guard<wpi::mutex> mutex(_globalErrorMutex);
    if (_globalError.GetCode() == 0) {
      _globalError.Clone(m_error);
    }
  }
}

/**
 * @brief Set the current error information associated with this sensor.
 * Range versions use for initialization code.
 *
 * @param code           The error code
 * @param minRange       The minimum allowed allocation range
 * @param maxRange       The maximum allowed allocation range
 * @param requestedValue The requested value to allocate
 * @param contextMessage A custom message from the code that set the error.
 * @param filename       Filename of the error source
 * @param function       Function of the error source
 * @param lineNumber     Line number of the error source
 */
void ErrorBase::SetErrorRange(Error::Code code, int32_t minRange,
                              int32_t maxRange, int32_t requestedValue,
                              const llvm::Twine& contextMessage,
                              llvm::StringRef filename,
                              llvm::StringRef function, int lineNumber) const {
  //  If there was an error
  if (code != 0) {
    //  Set the current error information for this object.
    m_error.Set(code,
                contextMessage + ", Minimum Value: " + llvm::Twine(minRange) +
                    ", MaximumValue: " + llvm::Twine(maxRange) +
                    ", Requested Value: " + llvm::Twine(requestedValue),
                filename, function, lineNumber, this);

    // Update the global error if there is not one already set.
    std::lock_guard<wpi::mutex> mutex(_globalErrorMutex);
    if (_globalError.GetCode() == 0) {
      _globalError.Clone(m_error);
    }
  }
}

/**
 * @brief Set the current error information associated with this sensor.
 *
 * @param errorMessage   The error message from WPIErrors.h
 * @param contextMessage A custom message from the code that set the error.
 * @param filename       Filename of the error source
 * @param function       Function of the error source
 * @param lineNumber     Line number of the error source
 */
void ErrorBase::SetWPIError(const llvm::Twine& errorMessage, Error::Code code,
                            const llvm::Twine& contextMessage,
                            llvm::StringRef filename, llvm::StringRef function,
                            int lineNumber) const {
  //  Set the current error information for this object.
  m_error.Set(code, errorMessage + ": " + contextMessage, filename, function,
              lineNumber, this);

  // Update the global error if there is not one already set.
  std::lock_guard<wpi::mutex> mutex(_globalErrorMutex);
  if (_globalError.GetCode() == 0) {
    _globalError.Clone(m_error);
  }
}

void ErrorBase::CloneError(const ErrorBase& rhs) const {
  m_error.Clone(rhs.GetError());
}

/**
 * @brief Check if the current error code represents a fatal error.
 *
 * @return true if the current error is fatal.
 */
bool ErrorBase::StatusIsFatal() const { return m_error.GetCode() < 0; }

void ErrorBase::SetGlobalError(Error::Code code,
                               const llvm::Twine& contextMessage,
                               llvm::StringRef filename,
                               llvm::StringRef function, int lineNumber) {
  // If there was an error
  if (code != 0) {
    std::lock_guard<wpi::mutex> mutex(_globalErrorMutex);

    // Set the current error information for this object.
    _globalError.Set(code, contextMessage, filename, function, lineNumber,
                     nullptr);
  }
}

void ErrorBase::SetGlobalWPIError(const llvm::Twine& errorMessage,
                                  const llvm::Twine& contextMessage,
                                  llvm::StringRef filename,
                                  llvm::StringRef function, int lineNumber) {
  std::lock_guard<wpi::mutex> mutex(_globalErrorMutex);
  if (_globalError.GetCode() != 0) {
    _globalError.Clear();
  }
  _globalError.Set(-1, errorMessage + ": " + contextMessage, filename, function,
                   lineNumber, nullptr);
}

/**
 * Retrieve the current global error.
 */
Error& ErrorBase::GetGlobalError() {
  std::lock_guard<wpi::mutex> mutex(_globalErrorMutex);
  return _globalError;
}
