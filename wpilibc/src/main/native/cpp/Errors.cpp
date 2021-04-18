// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Errors.h"

#include <exception>

#include <hal/DriverStation.h>
#include <hal/HALBase.h>
#include <wpi/Path.h>
#include <wpi/SmallString.h>
#include <wpi/StackTrace.h>

using namespace frc;

RuntimeError::RuntimeError(int32_t code, const wpi::Twine& message,
                           const wpi::Twine& loc, wpi::StringRef stack)
    : runtime_error{message.str()}, m_data{std::make_shared<Data>()} {
  m_data->code = code;
  m_data->loc = loc.str();
  m_data->stack = stack;
}

RuntimeError::RuntimeError(int32_t code, const wpi::Twine& message,
                           const char* fileName, int lineNumber,
                           const char* funcName, wpi::StringRef stack)
    : RuntimeError{code, message,
                   funcName + wpi::Twine{" ["} +
                       wpi::sys::path::filename(fileName) + ":" +
                       wpi::Twine{lineNumber} + "]",
                   stack} {}

void RuntimeError::Report() const {
  HAL_SendError(m_data->code < 0, m_data->code, 0, what(), m_data->loc.c_str(),
                m_data->stack.c_str(), 1);
}

const char* frc::GetErrorMessage(int32_t code) {
  using namespace err;
  using namespace warn;
  switch (code) {
#define S(label, offset, message) \
  case label:                     \
    return message;
#include "frc/WPIErrors.mac"
#include "frc/WPIWarnings.mac"
#undef S
    default:
      return HAL_GetErrorMessage(code);
  }
}

void frc::ReportError(int32_t status, const wpi::Twine& message,
                      const char* fileName, int lineNumber,
                      const char* funcName) {
  if (status == 0) {
    return;
  }
  const char* statusMessage = GetErrorMessage(status);
  auto stack = wpi::GetStackTrace(2);
  wpi::SmallString<128> buf;
  HAL_SendError(status < 0, status, 0,
                (statusMessage + wpi::Twine{": "} + message)
                    .toNullTerminatedStringRef(buf)
                    .data(),
                funcName, stack.c_str(), 1);
}

RuntimeError frc::MakeError(int32_t status, const wpi::Twine& message,
                            const char* fileName, int lineNumber,
                            const char* funcName) {
  const char* statusMessage = GetErrorMessage(status);
  auto stack = wpi::GetStackTrace(2);
  return RuntimeError{status,   statusMessage + wpi::Twine{": "} + message,
                      fileName, lineNumber,
                      funcName, stack};
}
