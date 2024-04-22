// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/support/ExpressionParser.h"

#include <cctype>
#include <cmath>
#include <stack>
#include <string>

#include <imgui.h>

namespace glass {
namespace expression {

ParseResult ParseResult::CreateSuccess(double value) {
  ParseResult result;
  result.kind = ParseResult::Success;
  result.successVal = value;
  return result;
}

ParseResult ParseResult::CreateError(const char* errorMessage) {
  ParseResult result;
  result.kind = ParseResult::Error;
  result.errorMessage = errorMessage;
  return result;
}

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
  explicit Lexer(const char* input)
      : input(input), startIdx(0), currentIdx(0) {}

  Token NextToken() {
    // Skip leading whitespace
    startIdx = currentIdx;
    while (std::isspace(input[startIdx]))
      startIdx++;
    if (input[startIdx] == 0)
      return Token(TokenType::End);
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
        if (std::isdigit(c) || c == '.')
          return nextNumber();
        return Token(TokenType::Error);
    }
  }

  // Makes NextToken() return the same token as previously
  void Repeat() { currentIdx = startIdx; }

 private:
  const char* input;
  int startIdx, currentIdx;

  Token nextNumber() {
    // Read whole part
    bool hasDigitsBeforeDecimal = false;
    while (std::isdigit(input[currentIdx])) {
      currentIdx++;
      hasDigitsBeforeDecimal = true;
    }

    // Read decimal part if it exists
    if (input[currentIdx] == '.') {
      currentIdx++;
      if (!hasDigitsBeforeDecimal && !std::isdigit(input[currentIdx]))
        return Token(TokenType::Error);

      while (std::isdigit(input[currentIdx]))
        currentIdx++;
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

void ApplyOperator(std::stack<double>& valStack, Operator op) {
  double right = valStack.top();
  valStack.pop();
  double left = valStack.top();
  valStack.pop();

  double val = 0;
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

ParseResult ParseExpr(Lexer& lexer, bool insideParen) {
  std::stack<Operator> operStack;
  std::stack<double> valStack;

  bool prevWasOp = true;
  TokenType prevType = TokenType::Add;

  while (true) {
    Token token = lexer.NextToken();

    bool wasOp = false;
    switch (token.type) {
      case TokenType::End:
        goto end;
      case TokenType::Number:
        // This happens if there's multiple decimal places in the number
        if (prevType == TokenType::Number)
          return ParseResult::CreateError("Invalid number");

        // Implicit multiplication. Ex: 2(4 + 5)
        if (!prevWasOp)
          operStack.push(Operator::Multiply);
        valStack.push(std::atof(std::string(token.str, token.strLen).c_str()));
        break;

      case TokenType::OpenParen: {
        // Implicit multiplication
        if (!prevWasOp)
          operStack.push(Operator::Multiply);

        ParseResult result = ParseExpr(lexer, true);
        if (result.kind == ParseResult::Error)
          return result;
        valStack.push(result.successVal);

        TokenType nextType = lexer.NextToken().type;
        if (nextType != TokenType::CloseParen) {
          if (nextType == TokenType::End)
            goto end;  // Act as if closed at end of expression
          return ParseResult::CreateError("Expected )");
        }
        break;
      }

      case TokenType::CloseParen:
        if (insideParen) {
          lexer.Repeat();
          goto end;
        }
        // Act as if there was open paren at start of expression
        break;

      case TokenType::Error:
        return ParseResult::CreateError("Unexpected character");

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
              return ParseResult::CreateError("Missing operand");
            }
            ApplyOperator(valStack, prevOp);
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
  while (!operStack.empty()) {
    if (valStack.size() < 2) {
      return ParseResult::CreateError("Missing operand");
    }
    ApplyOperator(valStack, operStack.top());
    operStack.pop();
  }
  if (valStack.empty()) {
    return ParseResult::CreateError("No value");
  }

  return ParseResult::CreateSuccess(valStack.top());
}

// expr is null-terminated string, as ImGui::inputText() uses
ParseResult TryParseExpr(const char* expr) {
  Lexer lexer(expr);
  return ParseExpr(lexer, false);
}

}  // namespace expression
}  // namespace glass
