/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ErrorBase.h"
#define WPI_ERRORS_DEFINE_STRINGS
#include "WPIErrors.h"

#include <errno.h>
#include <cstring>
#include <cstdio>
#include <sstream>

priority_mutex ErrorBase::_globalErrorMutex;
Error ErrorBase::_globalError;

/**
 * @brief Retrieve the current error.
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
 * error to the "errno" global variable.
 *
 * @param contextMessage A custom message from the code that set the error.
 * @param filename Filename of the error source
 * @param function Function of the error source
 * @param lineNumber Line number of the error source
 */
void ErrorBase::SetErrnoError(llvm::StringRef contextMessage,
                              llvm::StringRef filename,
                              llvm::StringRef function,
                              uint32_t lineNumber) const {
  std::string err;
  int errNo = errno;
  if (errNo == 0) {
    err = "OK: ";
    err += contextMessage;
  } else {
    char buf[256];
    snprintf(buf, 256, "%s (0x%08X): %.*s", strerror(errNo), errNo,
             contextMessage.size(), contextMessage.data());
    err = buf;
  }

  //  Set the current error information for this object.
  m_error.Set(-1, err, filename, function, lineNumber, this);

  // Update the global error if there is not one already set.
  std::lock_guard<priority_mutex> mutex(_globalErrorMutex);
  if (_globalError.GetCode() == 0) {
    _globalError.Clone(m_error);
  }
}

/**
 * @brief Set the current error information associated from the nivision Imaq
 * API.
 *
 * @param success The return from the function
 * @param contextMessage A custom message from the code that set the error.
 * @param filename Filename of the error source
 * @param function Function of the error source
 * @param lineNumber Line number of the error source
 */
void ErrorBase::SetImaqError(int success, llvm::StringRef contextMessage,
                             llvm::StringRef filename, llvm::StringRef function,
                             uint32_t lineNumber) const {
  //  If there was an error
  if (success <= 0) {
    std::stringstream err;
    err << success << ": " << contextMessage;

    //  Set the current error information for this object.
    m_error.Set(success, err.str(), filename, function, lineNumber, this);

    // Update the global error if there is not one already set.
    std::lock_guard<priority_mutex> mutex(_globalErrorMutex);
    if (_globalError.GetCode() == 0) {
      _globalError.Clone(m_error);
    }
  }
}

/**
 * @brief Set the current error information associated with this sensor.
 *
 * @param code The error code
 * @param contextMessage A custom message from the code that set the error.
 * @param filename Filename of the error source
 * @param function Function of the error source
 * @param lineNumber Line number of the error source
 */
void ErrorBase::SetError(Error::Code code, llvm::StringRef contextMessage,
                         llvm::StringRef filename, llvm::StringRef function,
                         uint32_t lineNumber) const {
  //  If there was an error
  if (code != 0) {
    //  Set the current error information for this object.
    m_error.Set(code, contextMessage, filename, function, lineNumber, this);

    // Update the global error if there is not one already set.
    std::lock_guard<priority_mutex> mutex(_globalErrorMutex);
    if (_globalError.GetCode() == 0) {
      _globalError.Clone(m_error);
    }
  }
}

/**
 * @brief Set the current error information associated with this sensor.
 *
 * @param errorMessage The error message from WPIErrors.h
 * @param contextMessage A custom message from the code that set the error.
 * @param filename Filename of the error source
 * @param function Function of the error source
 * @param lineNumber Line number of the error source
 */
void ErrorBase::SetWPIError(llvm::StringRef errorMessage, Error::Code code,
                            llvm::StringRef contextMessage,
                            llvm::StringRef filename, llvm::StringRef function,
                            uint32_t lineNumber) const {
  std::string err = errorMessage.str() + ": " + contextMessage.str();

  //  Set the current error information for this object.
  m_error.Set(code, err, filename, function, lineNumber, this);

  // Update the global error if there is not one already set.
  std::lock_guard<priority_mutex> mutex(_globalErrorMutex);
  if (_globalError.GetCode() == 0) {
    _globalError.Clone(m_error);
  }
}

void ErrorBase::CloneError(const ErrorBase& rhs) const {
  m_error.Clone(rhs.GetError());
}

/**
@brief Check if the current error code represents a fatal error.

@return true if the current error is fatal.
*/
bool ErrorBase::StatusIsFatal() const { return m_error.GetCode() < 0; }

void ErrorBase::SetGlobalError(Error::Code code, llvm::StringRef contextMessage,
                               llvm::StringRef filename,
                               llvm::StringRef function, uint32_t lineNumber) {
  //  If there was an error
  if (code != 0) {
    std::lock_guard<priority_mutex> mutex(_globalErrorMutex);

    //  Set the current error information for this object.
    _globalError.Set(code, contextMessage, filename, function, lineNumber,
                     nullptr);
  }
}

void ErrorBase::SetGlobalWPIError(llvm::StringRef errorMessage,
                                  llvm::StringRef contextMessage,
                                  llvm::StringRef filename,
                                  llvm::StringRef function,
                                  uint32_t lineNumber) {
  std::string err = errorMessage.str() + ": " + contextMessage.str();

  std::lock_guard<priority_mutex> mutex(_globalErrorMutex);
  if (_globalError.GetCode() != 0) {
    _globalError.Clear();
  }
  _globalError.Set(-1, err, filename, function, lineNumber, nullptr);
}

/**
  * Retrieve the current global error.
*/
Error& ErrorBase::GetGlobalError() {
  std::lock_guard<priority_mutex> mutex(_globalErrorMutex);
  return _globalError;
}
