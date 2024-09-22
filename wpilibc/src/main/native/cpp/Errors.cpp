// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Errors.h"

#include <string>
#include <utility>

#include <hal/DriverStation.h>
#include <hal/HALBase.h>
#include <wpi/StackTrace.h>
#include <wpi/fs.h>

using namespace frc;

RuntimeError::RuntimeError(int32_t code, std::string&& loc, std::string&& stack,
                           std::string&& message)
    : runtime_error{std::move(message)}, m_data{std::make_shared<Data>()} {
  m_data->code = code;
  m_data->loc = std::move(loc);
  m_data->stack = stack;
}

RuntimeError::RuntimeError(int32_t code, const char* fileName, int lineNumber,
                           const char* funcName, std::string&& stack,
                           std::string&& message)
    : RuntimeError{
          code,
          fmt::format("{} [{}:{}]", funcName,
                      fs::path{fileName}.filename().string(), lineNumber),
          std::move(stack), std::move(message)} {}

void RuntimeError::Report() const {
  HAL_SendError(m_data->code < 0, m_data->code, 0, what(), m_data->loc.c_str(),
                m_data->stack.c_str(), 1);
}

const char* frc::GetErrorMessage(int32_t* code) {
  switch (*code) {
#define S(label, offset, message) \
  case err::label:                \
    return message;
#include "frc/WPIErrors.mac"
#undef S
#define S(label, offset, message) \
  case warn::label:               \
    return message;
#include "frc/WPIWarnings.mac"
#undef S
    default:
      return HAL_GetLastError(code);
  }
}

void frc::ReportErrorV(int32_t status, const char* fileName, int lineNumber,
                       const char* funcName, fmt::string_view format,
                       fmt::format_args args) {
  if (status == 0) {
    return;
  }
  fmt::memory_buffer out;
  fmt::format_to(fmt::appender{out}, "{}: ", GetErrorMessage(&status));
  fmt::vformat_to(fmt::appender{out}, format, args);
  out.push_back('\0');
  HAL_SendError(status < 0, status, 0, out.data(), funcName,
                wpi::GetStackTrace(2).c_str(), 1);
}

RuntimeError frc::MakeErrorV(int32_t status, const char* fileName,
                             int lineNumber, const char* funcName,
                             fmt::string_view format, fmt::format_args args) {
  fmt::memory_buffer out;
  fmt::format_to(fmt::appender{out}, "{}: ", GetErrorMessage(&status));
  fmt::vformat_to(fmt::appender{out}, format, args);
  return RuntimeError{status,
                      fileName,
                      lineNumber,
                      funcName,
                      wpi::GetStackTrace(2),
                      fmt::to_string(out)};
}
