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
    case Token::INTEGER:
      return "integer";
    case Token::IDENTIFIER:
      return "identifier";
    case Token::LEFT_BRACKET:
      return "'['";
    case Token::RIGHT_BRACKET:
      return "']'";
    case Token::LEFT_BRACE:
      return "'{'";
    case Token::RIGHT_BRACE:
      return "'}'";
    case Token::COLON:
      return "':'";
    case Token::SEMICOLON:
      return "';'";
    case Token::COMMA:
      return "','";
    case Token::EQUALS:
      return "'='";
    case Token::END_OF_INPUT:
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
      return MakeToken(Token::LEFT_BRACKET);
    case ']':
      return MakeToken(Token::RIGHT_BRACKET);
    case '{':
      return MakeToken(Token::LEFT_BRACE);
    case '}':
      return MakeToken(Token::RIGHT_BRACE);
    case ':':
      return MakeToken(Token::COLON);
    case ';':
      return MakeToken(Token::SEMICOLON);
    case ',':
      return MakeToken(Token::COMMA);
    case '=':
      return MakeToken(Token::EQUALS);
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
      return {Token::END_OF_INPUT, {}};
    default:
      if (isAlpha(m_current) || m_current == '_') {
        [[likely]] return ScanIdentifier();
      }
      return MakeToken(Token::UNKNOWN);
  }
}

Token Lexer::ScanInteger() {
  do {
    Get();
  } while (isDigit(m_current));
  Unget();
  return MakeToken(Token::INTEGER);
}

Token Lexer::ScanIdentifier() {
  do {
    Get();
  } while (isAlnum(m_current) || m_current == '_');
  Unget();
  return MakeToken(Token::IDENTIFIER);
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
    if (m_token.Is(Token::SEMICOLON)) {
      continue;
    }
    if (m_token.Is(Token::END_OF_INPUT)) {
      break;
    }
    if (!ParseDeclaration(&out->declarations.emplace_back())) {
      [[unlikely]] return false;
    }
  } while (m_token.kind != Token::END_OF_INPUT);
  return true;
}

bool Parser::ParseDeclaration(ParsedDeclaration* out) {
  // optional enum specification
  if (m_token.Is(Token::IDENTIFIER) && m_token.text == "enum") {
    GetNextToken();
    if (!Expect(Token::LEFT_BRACE)) {
      [[unlikely]] return false;
    }
    if (!ParseEnum(&out->enumValues)) {
      [[unlikely]] return false;
    }
    GetNextToken();
  } else if (m_token.Is(Token::LEFT_BRACE)) {
    if (!ParseEnum(&out->enumValues)) {
      [[unlikely]] return false;
    }
    GetNextToken();
  }

  // type name
  if (!Expect(Token::IDENTIFIER)) {
    [[unlikely]] return false;
  }
  out->typeString = m_token.text;
  GetNextToken();

  // identifier name
  if (!Expect(Token::IDENTIFIER)) {
    [[unlikely]] return false;
  }
  out->name = m_token.text;
  GetNextToken();

  // array or bit field
  if (m_token.Is(Token::LEFT_BRACKET)) {
    GetNextToken();
    if (!Expect(Token::INTEGER)) {
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
    if (!Expect(Token::RIGHT_BRACKET)) {
      [[unlikely]] return false;
    }
    GetNextToken();
  } else if (m_token.Is(Token::COLON)) {
    GetNextToken();
    if (!Expect(Token::INTEGER)) {
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
  if (m_token.Is(Token::END_OF_INPUT)) {
    return true;
  }
  return Expect(Token::SEMICOLON);
}

bool Parser::ParseEnum(EnumValues* out) {
  // we start with current = '{'
  GetNextToken();
  while (!m_token.Is(Token::RIGHT_BRACE)) {
    if (!Expect(Token::IDENTIFIER)) {
      [[unlikely]] return false;
    }
    std::string name;
    name = m_token.text;
    GetNextToken();
    if (!Expect(Token::EQUALS)) {
      [[unlikely]] return false;
    }
    GetNextToken();
    if (!Expect(Token::INTEGER)) {
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
    if (m_token.Is(Token::RIGHT_BRACE)) {
      break;
    }
    if (!Expect(Token::COMMA)) {
      [[unlikely]] return false;
    }
    GetNextToken();
  }
  return true;
}
