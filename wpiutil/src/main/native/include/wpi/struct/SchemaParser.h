// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace wpi::structparser {

/**
 * A lexed raw struct schema token.
 */
struct Token {
  /** A lexed raw struct schema token kind. */
  enum Kind {
    /// Unknown.
    kUnknown,
    /// Integer.
    kInteger,
    /// Identifier.
    kIdentifier,
    /// Left square bracket.
    kLeftBracket,
    /// Right square bracket.
    kRightBracket,
    /// Left curly brace.
    kLeftBrace,
    /// Right curly brace.
    kRightBrace,
    /// Colon.
    kColon,
    /// Semicolon.
    kSemicolon,
    /// Comma.
    kComma,
    /// Equals.
    kEquals,
    /// End of input.
    kEndOfInput,
  };

  Token() = default;
  Token(Kind kind, std::string_view text) : kind{kind}, text{text} {}

  bool Is(Kind k) const { return kind == k; }

  Kind kind = kUnknown;
  std::string_view text;
};

std::string_view ToString(Token::Kind kind);

/**
 * Raw struct schema lexer.
 */
class Lexer {
 public:
  /**
   * Construct a raw struct schema lexer.
   *
   * @param in schema
   */
  explicit Lexer(std::string_view in) : m_in{in} {}

  /**
   * Gets the next token.
   *
   * @return Token
   */
  [[nodiscard]]
  Token Scan();

  /**
   * Gets the starting position of the last lexed token.
   *
   * @return position (0 = first character)
   */
  size_t GetPosition() const { return m_tokenStart; }

 private:
  Token ScanInteger();
  Token ScanIdentifier();

  void Get() {
    if (m_pos < m_in.size()) {
      [[likely]] m_current = m_in[m_pos];
    } else {
      m_current = -1;
    }
    ++m_pos;
  }

  void Unget() {
    if (m_pos > 0) {
      [[likely]] m_pos--;
      if (m_pos < m_in.size()) {
        [[likely]] m_current = m_in[m_pos];
      } else {
        m_current = -1;
      }
    } else {
      m_current = -1;
    }
  }

  Token MakeToken(Token::Kind kind) {
    return {kind, m_in.substr(m_tokenStart, m_pos - m_tokenStart)};
  }

  std::string_view m_in;
  int m_current = -1;
  size_t m_tokenStart = 0;
  size_t m_pos = 0;
};

/**
 * Raw struct set of enumerated values.
 */
using EnumValues = std::vector<std::pair<std::string, int64_t>>;

/**
 * Raw struct schema declaration.
 */
struct ParsedDeclaration {
  std::string typeString;
  std::string name;
  EnumValues enumValues;
  size_t arraySize = 1;
  unsigned int bitWidth = 0;
};

/**
 * Raw struct schema.
 */
struct ParsedSchema {
  std::vector<ParsedDeclaration> declarations;
};

/**
 * Raw struct schema parser.
 */
class Parser {
 public:
  /**
   * Construct a raw struct schema parser.
   *
   * @param in schema
   */
  explicit Parser(std::string_view in) : m_lexer{in} {}

  /**
   * Parses the schema.
   *
   * @param[out] out parsed schema object
   * @return true on success, false on failure (use GetError() to get error)
   */
  [[nodiscard]]
  bool Parse(ParsedSchema* out);

  /**
   * Gets the parser error if one occurred.
   *
   * @return parser error; blank if no error occurred
   */
  const std::string& GetError() const { return m_error; }

 private:
  [[nodiscard]]
  bool ParseDeclaration(ParsedDeclaration* out);
  [[nodiscard]]
  bool ParseEnum(EnumValues* out);

  Token::Kind GetNextToken() {
    m_token = m_lexer.Scan();
    return m_token.kind;
  }
  [[nodiscard]]
  bool Expect(Token::Kind kind) {
    if (m_token.Is(kind)) {
      [[likely]] return true;
    }
    FailExpect(kind);
    return false;
  }
  void FailExpect(Token::Kind desired);
  void Fail(std::string_view msg);

  Lexer m_lexer;
  Token m_token;
  std::string m_error;
};

}  // namespace wpi::structparser
