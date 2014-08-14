/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "Base.h"
#include <string>
#include <stdint.h>

//  Forward declarations
class ErrorBase;

/**
 * Error object represents a library error.
 */
class Error {
 public:
  typedef int32_t Code;

  Error();
  void Clone(Error& error);
  Code GetCode() const;
  const char* GetMessage() const;
  const char* GetFilename() const;
  const char* GetFunction() const;
  uint32_t GetLineNumber() const;
  const ErrorBase* GetOriginatingObject() const;
  double GetTimestamp() const;
  void Clear();
  void Set(Code code, const char* contextMessage, const char* filename,
           const char* function, uint32_t lineNumber,
           const ErrorBase* originatingObject);

 private:
  void Report();

  Code m_code = 0;
  std::string m_message;
  std::string m_filename;
  std::string m_function;
  uint32_t m_lineNumber = 0;
  const ErrorBase* m_originatingObject = nullptr;
  double m_timestamp = 0.0;

  DISALLOW_COPY_AND_ASSIGN(Error);
};
