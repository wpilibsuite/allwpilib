// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include <wpi/StringRef.h>
#include <wpi/Twine.h>

#ifdef _WIN32
#pragma push_macro("GetMessage")
#undef GetMessage
#endif

namespace frc {

class ErrorBase;

/**
 * Error object represents a library error.
 */
class Error {
 public:
  using Code = int;

  Error() = default;
  Error(Code code, const wpi::Twine& contextMessage, wpi::StringRef filename,
        wpi::StringRef function, int lineNumber,
        const ErrorBase* originatingObject);

  bool operator<(const Error& rhs) const;

  Code GetCode() const;
  std::string GetMessage() const;
  std::string GetFilename() const;
  std::string GetFunction() const;
  int GetLineNumber() const;
  const ErrorBase* GetOriginatingObject() const;
  double GetTimestamp() const;
  void Clear();
  void Set(Code code, const wpi::Twine& contextMessage, wpi::StringRef filename,
           wpi::StringRef function, int lineNumber,
           const ErrorBase* originatingObject);

 private:
  void Report();

  Code m_code = 0;
  std::string m_message;
  std::string m_filename;
  std::string m_function;
  int m_lineNumber = 0;
  const ErrorBase* m_originatingObject = nullptr;
  double m_timestamp = 0.0;
};

}  // namespace frc

#ifdef _WIN32
#pragma pop_macro("GetMessage")
#endif
