// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/struct/SchemaParser.h"

#include <string>
#include <utility>

#include <fmt/format.h>

#include "wpi/StringExtras.h"

using namespace wpi::structparser;

std::string_view wpi::structparser::ToString(Token::Kind kind) {
  switch (kind) {
    case Token::kInteger:
      return "integer";
    case Token::kIdentifier:
      return "identifier";
    case Token::kLeftBracket:
      return "'['";
    case Token::kRightBracket:
      return "']'";
    case Token::kLeftBrace:
      return "'{'";
    case Token::kRightBrace:
      return "'}'";
    case Token::kColon:
      return "':'";
    case Token::kSemicolon:
      return "';'";
    case Token::kComma:
      return "','";
    case Token::kEquals:
      return "'='";
    case Token::kEndOfInput:
      return "<EOF>";
    default:
      return "unknown";
  }
}

Token Lexer::Scan() {
  // skip whitespace
  do {
    Get();
  } while (m_current == ' ' || m_current == '\t' || m_current == '\n' ||
           m_current == '\r');
  m_tokenStart = m_pos - 1;

  switch (m_current) {
    case '[':
      return MakeToken(Token::kLeftBracket);
    case ']':
      return MakeToken(Token::kRightBracket);
    case '{':
      return MakeToken(Token::kLeftBrace);
    case '}':
      return MakeToken(Token::kRightBrace);
    case ':':
      return MakeToken(Token::kColon);
    case ';':
      return MakeToken(Token::kSemicolon);
    case ',':
      return MakeToken(Token::kComma);
    case '=':
      return MakeToken(Token::kEquals);
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return ScanInteger();
    case -1:
      return {Token::kEndOfInput, {}};
    default:
      if (isAlpha(m_current) || m_current == '_') {
        [[likely]] return ScanIdentifier();
      }
      return MakeToken(Token::kUnknown);
  }
}

Token Lexer::ScanInteger() {
  do {
    Get();
  } while (isDigit(m_current));
  Unget();
  return MakeToken(Token::kInteger);
}

Token Lexer::ScanIdentifier() {
  do {
    Get();
  } while (isAlnum(m_current) || m_current == '_');
  Unget();
  return MakeToken(Token::kIdentifier);
}

void Parser::FailExpect(Token::Kind desired) {
  Fail(fmt::format("expected {}, got '{}'", ToString(desired), m_token.text));
}

void Parser::Fail(std::string_view msg) {
  m_error = fmt::format("{}: {}", m_lexer.GetPosition(), msg);
}

bool Parser::Parse(ParsedSchema* out) {
  do {
    GetNextToken();
    if (m_token.Is(Token::kSemicolon)) {
      continue;
    }
    if (m_token.Is(Token::kEndOfInput)) {
      break;
    }
    if (!ParseDeclaration(&out->declarations.emplace_back())) {
      [[unlikely]] return false;
    }
  } while (m_token.kind != Token::kEndOfInput);
  return true;
}

bool Parser::ParseDeclaration(ParsedDeclaration* out) {
  // optional enum specification
  if (m_token.Is(Token::kIdentifier) && m_token.text == "enum") {
    GetNextToken();
    if (!Expect(Token::kLeftBrace)) {
      [[unlikely]] return false;
    }
    if (!ParseEnum(&out->enumValues)) {
      [[unlikely]] return false;
    }
    GetNextToken();
  } else if (m_token.Is(Token::kLeftBrace)) {
    if (!ParseEnum(&out->enumValues)) {
      [[unlikely]] return false;
    }
    GetNextToken();
  }

  // type name
  if (!Expect(Token::kIdentifier)) {
    [[unlikely]] return false;
  }
  out->typeString = m_token.text;
  GetNextToken();

  // identifier name
  if (!Expect(Token::kIdentifier)) {
    [[unlikely]] return false;
  }
  out->name = m_token.text;
  GetNextToken();

  // array or bit field
  if (m_token.Is(Token::kLeftBracket)) {
    GetNextToken();
    if (!Expect(Token::kInteger)) {
      [[unlikely]] return false;
    }
    auto val = parse_integer<uint64_t>(m_token.text, 10);
    if (val && *val > 0) {
      out->arraySize = *val;
    } else {
      Fail(fmt::format("array size '{}' is not a positive integer",
                       m_token.text));
      [[unlikely]] return false;
    }
    GetNextToken();
    if (!Expect(Token::kRightBracket)) {
      [[unlikely]] return false;
    }
    GetNextToken();
  } else if (m_token.Is(Token::kColon)) {
    GetNextToken();
    if (!Expect(Token::kInteger)) {
      [[unlikely]] return false;
    }
    auto val = parse_integer<unsigned int>(m_token.text, 10);
    if (val && *val > 0) {
      out->bitWidth = *val;
    } else {
      Fail(fmt::format("bitfield width '{}' is not a positive integer",
                       m_token.text));
      [[unlikely]] return false;
    }
    GetNextToken();
  }

  // declaration must end with EOF or semicolon
  if (m_token.Is(Token::kEndOfInput)) {
    return true;
  }
  return Expect(Token::kSemicolon);
}

bool Parser::ParseEnum(EnumValues* out) {
  // we start with current = '{'
  GetNextToken();
  while (!m_token.Is(Token::kRightBrace)) {
    if (!Expect(Token::kIdentifier)) {
      [[unlikely]] return false;
    }
    std::string name;
    name = m_token.text;
    GetNextToken();
    if (!Expect(Token::kEquals)) {
      [[unlikely]] return false;
    }
    GetNextToken();
    if (!Expect(Token::kInteger)) {
      [[unlikely]] return false;
    }
    int64_t value;
    if (auto val = parse_integer<int64_t>(m_token.text, 10)) {
      value = *val;
    } else {
      Fail(fmt::format("could not parse enum value '{}'", m_token.text));
      [[unlikely]] return false;
    }
    out->emplace_back(std::move(name), value);
    GetNextToken();
    if (m_token.Is(Token::kRightBrace)) {
      break;
    }
    if (!Expect(Token::kComma)) {
      [[unlikely]] return false;
    }
    GetNextToken();
  }
  return true;
}
