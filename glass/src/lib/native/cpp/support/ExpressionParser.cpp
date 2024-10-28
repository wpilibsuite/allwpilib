// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/support/ExpressionParser.h"

#include <cmath>
#include <stack>
#include <string>
#include <type_traits>

#include <wpi/StringExtras.h>
#include <wpi/expected>

namespace glass::expression {

enum class TokenType {
  Number,
  Add,
  Subtract,
  Multiply,
  Divide,
  Exponent,
  OpenParen,
  CloseParen,

  End,    // Hit end of input
  Error,  // Invalid character found
};

struct Token {
  TokenType type;
  const char* str;
  int strLen;

  explicit Token(TokenType type) : type(type), str(nullptr), strLen(0) {}

  Token(TokenType type, const char* str, int strLen)
      : type(type), str(str), strLen(strLen) {}
};

class Lexer {
 public:
  explicit Lexer(const char* input, bool isInteger)
      : input(input), isInteger(isInteger) {}

  Token NextToken() {
    // Skip leading whitespace
    startIdx = currentIdx;
    while (std::isspace(input[startIdx])) {
      startIdx++;
    }
    if (input[startIdx] == 0) {
      return Token(TokenType::End);
    }
    currentIdx = startIdx;

    char c = input[currentIdx];
    currentIdx++;
    switch (c) {
      case '+':
        return Token(TokenType::Add);
      case '-':
        return Token(TokenType::Subtract);
      case '*':
        return Token(TokenType::Multiply);
      case '/':
        return Token(TokenType::Divide);
      case '^':
        return Token(TokenType::Exponent);
      case '(':
        return Token(TokenType::OpenParen);
      case ')':
        return Token(TokenType::CloseParen);
      default:
        currentIdx--;
        if (wpi::isDigit(c) || c == '.') {
          return nextNumber();
        }
        return Token(TokenType::Error, &input[currentIdx], 1);
    }
  }

  // Makes NextToken() return the same token as previously
  void Repeat() { currentIdx = startIdx; }

 private:
  const char* input;
  bool isInteger;
  int startIdx = 0, currentIdx = 0;

  Token nextNumber() {
    // Read whole part
    bool hasDigitsBeforeDecimal = false;
    while (wpi::isDigit(input[currentIdx])) {
      currentIdx++;
      hasDigitsBeforeDecimal = true;
    }

    // Read decimal part if it exists
    if (input[currentIdx] == '.') {
      // Integers can't have fractional part
      if (isInteger) {
        return Token(TokenType::Error, &input[currentIdx], 1);
      }

      currentIdx++;
      // Report a single '.' with no digits as an error
      if (!hasDigitsBeforeDecimal && !wpi::isDigit(input[currentIdx])) {
        // Report the decimal as the unexpected char
        return Token(TokenType::Error, &input[currentIdx - 1], 1);
      }

      while (wpi::isDigit(input[currentIdx])) {
        currentIdx++;
      }

      // Make sure the number has at most one decimal point
      if (input[currentIdx] == '.') {
        return Token(TokenType::Error, &input[currentIdx], 1);
      }
    }

    return Token(TokenType::Number, input + startIdx, currentIdx - startIdx);
  }
};

enum class Operator { Add, Subtract, Multiply, Divide, Exponent, Negate, None };

Operator GetOperator(TokenType type) {
  switch (type) {
    case TokenType::Add:
      return Operator::Add;
    case TokenType::Subtract:
      return Operator::Subtract;
    case TokenType::Multiply:
      return Operator::Multiply;
    case TokenType::Divide:
      return Operator::Divide;
    case TokenType::Exponent:
      return Operator::Exponent;
    default:
      return Operator::None;
  }
}

int OperatorPrecedence(Operator op) {
  switch (op) {
    case Operator::Add:
    case Operator::Subtract:
      return 1;
    case Operator::Multiply:
    case Operator::Divide:
      return 2;
    case Operator::Exponent:
      return 3;
    case Operator::Negate:
      return 4;
    case Operator::None:
      return 0;
  }
  return 0;
}

bool IsOperatorRightAssociative(Operator op) {
  return op == Operator::Exponent || op == Operator::Negate;
}

template <typename V>
void ApplyOperator(std::stack<V>& valStack, Operator op) {
  V right = valStack.top();
  valStack.pop();
  V left = valStack.top();
  valStack.pop();

  V val = 0;
  switch (op) {
    case Operator::Add:
      val = left + right;
      break;
    case Operator::Subtract:
      val = left - right;
      break;
    case Operator::Multiply:
      val = left * right;
      break;
    case Operator::Divide:
      val = left / right;
      break;
    case Operator::Exponent:
      val = std::pow(left, right);
      break;
    case Operator::Negate:
      val = -right;
      break;
    case Operator::None:
      break;
  }

  valStack.push(val);
}

template <typename V>
std::optional<V> ValueFromString(std::string_view str);

template <>
std::optional<int64_t> ValueFromString(std::string_view str) {
  return wpi::parse_integer<int64_t>(str, 10);
}

template <>
std::optional<float> ValueFromString(std::string_view str) {
  return wpi::parse_float<float>(str);
}

template <>
std::optional<double> ValueFromString(std::string_view str) {
  return wpi::parse_float<double>(str);
}

template <typename V>
wpi::expected<V, std::string> EvalAll(std::stack<Operator>& operStack,
                                      std::stack<V>& valStack) {
  while (!operStack.empty()) {
    if (valStack.size() < 2) {
      return wpi::unexpected("Missing operand");
    }
    ApplyOperator<V>(valStack, operStack.top());
    operStack.pop();
  }
  if (valStack.empty()) {
    return wpi::unexpected("No value");
  }

  // Intentionally leaves the result value on top of valStack so unmatched
  // closing parentheses work
  return valStack.top();
}

template <typename V>
wpi::expected<V, std::string> ParseExpr(Lexer& lexer, bool insideParen) {
  std::stack<Operator> operStack;
  std::stack<V> valStack;

  bool prevWasOp = true;
  TokenType prevType = TokenType::Add;

  while (true) {
    Token token = lexer.NextToken();

    bool wasOp = false;
    switch (token.type) {
      case TokenType::End:
        goto end;
      case TokenType::Number: {
        // Check for two numbers in a row (ex: "2 4"). Implicit multiplication
        // is probably not what the user intended in this case, so give them an
        // error.
        if (prevType == TokenType::Number) {
          return wpi::unexpected("Missing operator");
        }

        // Implicit multiplication. Ex: "2(4 + 5)"
        if (!prevWasOp) {
          operStack.push(Operator::Multiply);
        }

        auto value =
            ValueFromString<V>(std::string_view(token.str, token.strLen));
        if (value) {
          valStack.push(value.value());
        } else {
          return wpi::unexpected("Invalid number");
        }

        break;
      }

      case TokenType::OpenParen: {
        // Implicit multiplication
        if (!prevWasOp) {
          operStack.push(Operator::Multiply);
        }

        wpi::expected<V, std::string> result = ParseExpr<V>(lexer, true);
        if (!result) {
          return result;
        }
        valStack.push(result.value());

        TokenType nextType = lexer.NextToken().type;
        if (nextType != TokenType::CloseParen) {
          if (nextType == TokenType::End) {
            goto end;  // Act as if closed at end of expression
          }
          return wpi::unexpected("Expected )");
        }
        break;
      }

      case TokenType::CloseParen: {
        if (insideParen) {
          lexer.Repeat();
          goto end;
        }

        // Acts as if there was open paren at start of expression. EvalAll will
        // clear both stacks, and leave the result value on top of valStack.
        // This makes sure everything inside the parentheses is evaluated first
        wpi::expected<V, std::string> result = EvalAll<V>(operStack, valStack);
        if (!result) {
          return result;
        }
        break;
      }

      case TokenType::Error:
        return wpi::unexpected(std::string("Unexpected character: ")
                                   .append(token.str, token.strLen));

      default:
        Operator op = GetOperator(token.type);
        if (op == Operator::None) {
          lexer.Repeat();
          goto end;
        }
        if (op == Operator::Subtract && prevWasOp) {
          op = Operator::Negate;
          // Dummy left-hand side for negation
          valStack.push(0.0);
        }
        wasOp = true;

        while (!operStack.empty()) {
          Operator prevOp = operStack.top();

          bool rightAssoc = IsOperatorRightAssociative(op);
          int precedence = OperatorPrecedence(op);
          int prevPrecedence = OperatorPrecedence(prevOp);

          if ((!rightAssoc && precedence == prevPrecedence) ||
              precedence < prevPrecedence) {
            operStack.pop();
            if (valStack.size() < 2) {
              return wpi::unexpected("Missing operand");
            }
            ApplyOperator<V>(valStack, prevOp);
          } else {
            break;
          }
        }
        operStack.push(op);
        break;
    }
    prevType = token.type;
    prevWasOp = wasOp;
  }

// Reached the end of the expression
end:
  return EvalAll<V>(operStack, valStack);
}

// expr is null-terminated string, as ImGui::inputText() uses
template <typename V>
wpi::expected<V, std::string> TryParseExpr(const char* expr) {
  Lexer lexer(expr, std::is_integral_v<V>);
  return ParseExpr<V>(lexer, false);
}

template wpi::expected<double, std::string> TryParseExpr(const char*);
template wpi::expected<float, std::string> TryParseExpr(const char*);
template wpi::expected<int64_t, std::string> TryParseExpr(const char*);

}  // namespace glass::expression
