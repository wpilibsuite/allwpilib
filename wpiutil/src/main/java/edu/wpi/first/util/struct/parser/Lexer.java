// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.struct.parser;

/** Raw struct schema lexer. */
public class Lexer {
  /**
   * Construct a raw struct schema lexer.
   *
   * @param in schema
   */
  public Lexer(String in) {
    m_in = in;
  }

  /**
   * Gets the next token.
   *
   * @return Token kind; the token text can be retrieved using getTokenText()
   */
  public TokenKind scan() {
    // skip whitespace
    do {
      get();
    } while (m_current == ' ' || m_current == '\t' || m_current == '\n' || m_current == '\r');
    m_tokenStart = m_pos - 1;

    return switch (m_current) {
      case '[' -> TokenKind.kLeftBracket;
      case ']' -> TokenKind.kRightBracket;
      case '{' -> TokenKind.kLeftBrace;
      case '}' -> TokenKind.kRightBrace;
      case ':' -> TokenKind.kColon;
      case ';' -> TokenKind.kSemicolon;
      case ',' -> TokenKind.kComma;
      case '=' -> TokenKind.kEquals;
      case '-', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' -> scanInteger();
      case '\0' -> TokenKind.kEndOfInput;
      default -> {
        if (Character.isLetter(m_current) || m_current == '_') {
          yield scanIdentifier();
        }
        yield TokenKind.kUnknown;
      }
    };
  }

  /**
   * Gets the text of the last lexed token.
   *
   * @return token text
   */
  public String getTokenText() {
    if (m_tokenStart >= m_in.length()) {
      return "";
    }
    return m_in.substring(m_tokenStart, m_pos);
  }

  /**
   * Gets the starting position of the last lexed token.
   *
   * @return position (0 = first character)
   */
  public int getPosition() {
    return m_tokenStart;
  }

  private TokenKind scanInteger() {
    do {
      get();
    } while (Character.isDigit(m_current));
    unget();
    return TokenKind.kInteger;
  }

  private TokenKind scanIdentifier() {
    do {
      get();
    } while (Character.isLetterOrDigit(m_current) || m_current == '_');
    unget();
    return TokenKind.kIdentifier;
  }

  private void get() {
    if (m_pos < m_in.length()) {
      m_current = m_in.charAt(m_pos);
    } else {
      m_current = '\0';
    }
    ++m_pos;
  }

  private void unget() {
    if (m_pos > 0) {
      m_pos--;
      if (m_pos < m_in.length()) {
        m_current = m_in.charAt(m_pos);
      } else {
        m_current = '\0';
      }
    } else {
      m_current = '\0';
    }
  }

  final String m_in;
  char m_current;
  int m_tokenStart;
  int m_pos;
}
