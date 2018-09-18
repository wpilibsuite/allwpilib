/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <wpi/StringRef.h>
#include <wpi/Twine.h>
#include <wpi/mutex.h>

#include "frc/Base.h"
#include "frc/Error.h"

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
 *
 * ErrorBase is the base class for most objects since it holds the generated
 * error for that object. In addition, there is a single instance of a global
 * error object.
 */
class ErrorBase {
  // TODO: Consider initializing instance variables and cleanup in destructor
 public:
  ErrorBase();
  virtual ~ErrorBase() = default;

  ErrorBase(ErrorBase&&) = default;
  ErrorBase& operator=(ErrorBase&&) = default;

  /**
   * @brief Retrieve the current error.
   *
   * Get the current error information associated with this sensor.
   */
  virtual Error& GetError();

  /**
   * @brief Retrieve the current error.
   *
   * Get the current error information associated with this sensor.
   */
  virtual const Error& GetError() const;

  /**
   * @brief Clear the current error information associated with this sensor.
   */
  virtual void ClearError() const;

  /**
   * @brief Set error information associated with a C library call that set an
   *        error to the "errno" global variable.
   *
   * @param contextMessage A custom message from the code that set the error.
   * @param filename       Filename of the error source
   * @param function       Function of the error source
   * @param lineNumber     Line number of the error source
   */
  virtual void SetErrnoError(const wpi::Twine& contextMessage,
                             wpi::StringRef filename, wpi::StringRef function,
                             int lineNumber) const;

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
  virtual void SetImaqError(int success, const wpi::Twine& contextMessage,
                            wpi::StringRef filename, wpi::StringRef function,
                            int lineNumber) const;

  /**
   * @brief Set the current error information associated with this sensor.
   *
   * @param code           The error code
   * @param contextMessage A custom message from the code that set the error.
   * @param filename       Filename of the error source
   * @param function       Function of the error source
   * @param lineNumber     Line number of the error source
   */
  virtual void SetError(Error::Code code, const wpi::Twine& contextMessage,
                        wpi::StringRef filename, wpi::StringRef function,
                        int lineNumber) const;

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
  virtual void SetErrorRange(Error::Code code, int32_t minRange,
                             int32_t maxRange, int32_t requestedValue,
                             const wpi::Twine& contextMessage,
                             wpi::StringRef filename, wpi::StringRef function,
                             int lineNumber) const;

  /**
   * @brief Set the current error information associated with this sensor.
   *
   * @param errorMessage   The error message from WPIErrors.h
   * @param contextMessage A custom message from the code that set the error.
   * @param filename       Filename of the error source
   * @param function       Function of the error source
   * @param lineNumber     Line number of the error source
   */
  virtual void SetWPIError(const wpi::Twine& errorMessage, Error::Code code,
                           const wpi::Twine& contextMessage,
                           wpi::StringRef filename, wpi::StringRef function,
                           int lineNumber) const;

  virtual void CloneError(const ErrorBase& rhs) const;

  /**
   * @brief Check if the current error code represents a fatal error.
   *
   * @return true if the current error is fatal.
   */
  virtual bool StatusIsFatal() const;

  static void SetGlobalError(Error::Code code, const wpi::Twine& contextMessage,
                             wpi::StringRef filename, wpi::StringRef function,
                             int lineNumber);

  static void SetGlobalWPIError(const wpi::Twine& errorMessage,
                                const wpi::Twine& contextMessage,
                                wpi::StringRef filename,
                                wpi::StringRef function, int lineNumber);

  /**
   * Retrieve the current global error.
   */
  static const Error& GetGlobalError();

 protected:
  mutable Error m_error;
};

}  // namespace frc
