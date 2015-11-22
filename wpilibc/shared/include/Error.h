/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "Base.h"

#ifdef _WIN32
	#include <Windows.h>
	//Windows.h defines #define GetMessage GetMessageW, which is stupid and we don't want it.
	#undef GetMessage
#endif

#include <string>
#include <stdint.h>
#include "llvm/StringRef.h"

//  Forward declarations
class ErrorBase;

/**
 * Error object represents a library error.
 */
class Error {
 public:
  typedef int32_t Code;

  Error() = default;

  Error(const Error&) = delete;
  Error& operator=(const Error&) = delete;

  void Clone(const Error& error);
  Code GetCode() const;
  std::string GetMessage() const;
  std::string GetFilename() const;
  std::string GetFunction() const;
  uint32_t GetLineNumber() const;
  const ErrorBase* GetOriginatingObject() const;
  double GetTimestamp() const;
  void Clear();
  void Set(Code code, llvm::StringRef contextMessage,
           llvm::StringRef filename, llvm::StringRef function,
           uint32_t lineNumber, const ErrorBase* originatingObject);

 private:
  void Report();

  Code m_code = 0;
  std::string m_message;
  std::string m_filename;
  std::string m_function;
  uint32_t m_lineNumber = 0;
  const ErrorBase* m_originatingObject = nullptr;
  double m_timestamp = 0.0;
};
