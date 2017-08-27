/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <HAL/cpp/priority_mutex.h>
#include <llvm/StringRef.h>

#include "Base.h"
#include "Error.h"

#define wpi_setErrnoErrorWithContext(context) \
  this->SetErrnoError((context), __FILE__, __FUNCTION__, __LINE__)
#define wpi_setErrnoError() wpi_setErrnoErrorWithContext("")
#define wpi_setImaqErrorWithContext(code, context)                             \
  do {                                                                         \
    if ((code) != 0)                                                           \
      this->SetImaqError((code), (context), __FILE__, __FUNCTION__, __LINE__); \
  } while (0)
#define wpi_setErrorWithContext(code, context)                             \
  do {                                                                     \
    if ((code) != 0)                                                       \
      this->SetError((code), (context), __FILE__, __FUNCTION__, __LINE__); \
  } while (0)
#define wpi_setErrorWithContextRange(code, min, max, req, context)          \
  do {                                                                      \
    if ((code) != 0)                                                        \
      this->SetErrorRange((code), (min), (max), (req), (context), __FILE__, \
                          __FUNCTION__, __LINE__);                          \
  } while (0)
#define wpi_setError(code) wpi_setErrorWithContext(code, "")
#define wpi_setStaticErrorWithContext(object, code, context)                 \
  do {                                                                       \
    if ((code) != 0)                                                         \
      object->SetError((code), (context), __FILE__, __FUNCTION__, __LINE__); \
  } while (0)
#define wpi_setStaticError(object, code) \
  wpi_setStaticErrorWithContext(object, code, "")
#define wpi_setGlobalErrorWithContext(code, context)                \
  do {                                                              \
    if ((code) != 0)                                                \
      ::frc::ErrorBase::SetGlobalError((code), (context), __FILE__, \
                                       __FUNCTION__, __LINE__);     \
  } while (0)
#define wpi_setGlobalError(code) wpi_setGlobalErrorWithContext(code, "")
#define wpi_setWPIErrorWithContext(error, context)                    \
  this->SetWPIError((wpi_error_s_##error), (wpi_error_value_##error), \
                    (context), __FILE__, __FUNCTION__, __LINE__)
#define wpi_setWPIError(error) (wpi_setWPIErrorWithContext(error, ""))
#define wpi_setStaticWPIErrorWithContext(object, error, context)  \
  object->SetWPIError((wpi_error_s_##error), (context), __FILE__, \
                      __FUNCTION__, __LINE__)
#define wpi_setStaticWPIError(object, error) \
  wpi_setStaticWPIErrorWithContext(object, error, "")
#define wpi_setGlobalWPIErrorWithContext(error, context)                \
  ::frc::ErrorBase::SetGlobalWPIError((wpi_error_s_##error), (context), \
                                      __FILE__, __FUNCTION__, __LINE__)
#define wpi_setGlobalWPIError(error) wpi_setGlobalWPIErrorWithContext(error, "")

namespace frc {

/**
 * Base class for most objects.
 * ErrorBase is the base class for most objects since it holds the generated
 * error
 * for that object. In addition, there is a single instance of a global error
 * object
 */
class ErrorBase {
  // TODO: Consider initializing instance variables and cleanup in destructor
 public:
  ErrorBase() = default;
  virtual ~ErrorBase() = default;

  ErrorBase(const ErrorBase&) = delete;
  ErrorBase& operator=(const ErrorBase&) = delete;

  virtual Error& GetError();
  virtual const Error& GetError() const;
  virtual void SetErrnoError(llvm::StringRef contextMessage,
                             llvm::StringRef filename, llvm::StringRef function,
                             int lineNumber) const;
  virtual void SetImaqError(int success, llvm::StringRef contextMessage,
                            llvm::StringRef filename, llvm::StringRef function,
                            int lineNumber) const;
  virtual void SetError(Error::Code code, llvm::StringRef contextMessage,
                        llvm::StringRef filename, llvm::StringRef function,
                        int lineNumber) const;
  virtual void SetErrorRange(Error::Code code, int32_t minRange,
                             int32_t maxRange, int32_t requestedValue,
                             llvm::StringRef contextMessage,
                             llvm::StringRef filename, llvm::StringRef function,
                             int lineNumber) const;
  virtual void SetWPIError(llvm::StringRef errorMessage, Error::Code code,
                           llvm::StringRef contextMessage,
                           llvm::StringRef filename, llvm::StringRef function,
                           int lineNumber) const;
  virtual void CloneError(const ErrorBase& rhs) const;
  virtual void ClearError() const;
  virtual bool StatusIsFatal() const;
  static void SetGlobalError(Error::Code code, llvm::StringRef contextMessage,
                             llvm::StringRef filename, llvm::StringRef function,
                             int lineNumber);
  static void SetGlobalWPIError(llvm::StringRef errorMessage,
                                llvm::StringRef contextMessage,
                                llvm::StringRef filename,
                                llvm::StringRef function, int lineNumber);
  static Error& GetGlobalError();

 protected:
  mutable Error m_error;
  // TODO: Replace globalError with a global list of all errors.
  static hal::priority_mutex _globalErrorMutex;
  static Error _globalError;
};

}  // namespace frc
