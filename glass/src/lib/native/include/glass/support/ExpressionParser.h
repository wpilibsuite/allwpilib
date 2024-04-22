// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

namespace glass {
namespace expression {

enum class ParseResultKind {
  Success,
  Error,
};

template <typename V>
struct ParseResult {
  static ParseResult<V> CreateSuccess(V value);
  static ParseResult<V> CreateError(const char* errorMessage);

  // If kind is Success, expression value is in successVal
  // If kind is Error, error message is in errorMessage
  ParseResultKind kind;
  union {
    V successVal;
    const char* errorMessage;
  };
};

template <typename V>
ParseResult<V> TryParseExpr(const char* expr);

extern template ParseResult<double> TryParseExpr(const char*);
extern template ParseResult<float> TryParseExpr(const char*);
extern template ParseResult<int64_t> TryParseExpr(const char*);

}  // namespace expression
}  // namespace glass
