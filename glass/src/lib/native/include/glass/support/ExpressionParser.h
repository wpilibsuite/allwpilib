// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace glass {
namespace expression {

struct ParseResult {
  static ParseResult CreateSuccess(double value);
  static ParseResult CreateError(const char* errorMessage);

  enum { Success, Error } kind;

  // If kind is Success, expression value is in successVal
  // If kind is Error, error message is in errorMessage
  union {
    double successVal;
    const char* errorMessage;
  };
};

ParseResult TryParseExpr(const char* expr);

}  // namespace expression
}  // namespace glass
