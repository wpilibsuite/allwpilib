/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <string>

#include <llvm/StringRef.h>
#include <llvm/Twine.h>

#ifdef _WIN32
#include <Windows.h>
// Windows.h defines #define GetMessage GetMessageW, which we don't want.
#undef GetMessage
#endif

#include "Base.h"

namespace frc {

class ErrorBase;

/**
 * Error object represents a library error.
 */
class Error {
 public:
  typedef int Code;

  Error() = default;

  Error(const Error&) = delete;
  Error& operator=(const Error&) = delete;

  void Clone(const Error& error);
  Code GetCode() const;
  std::string GetMessage() const;
  std::string GetFilename() const;
  std::string GetFunction() const;
  int GetLineNumber() const;
  const ErrorBase* GetOriginatingObject() const;
  double GetTimestamp() const;
  void Clear();
  void Set(Code code, const llvm::Twine& contextMessage,
           llvm::StringRef filename, llvm::StringRef function, int lineNumber,
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
