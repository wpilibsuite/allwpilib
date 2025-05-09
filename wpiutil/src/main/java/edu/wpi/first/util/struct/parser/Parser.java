// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.struct.parser;

import java.util.HashMap;
import java.util.Map;

/** Raw struct schema parser. */
public class Parser {
  /**
   * Construct a raw struct schema parser.
   *
   * @param in schema
   */
  public Parser(String in) {
    m_lexer = new Lexer(in);
  }

  /**
   * Parses the schema.
   *
   * @return parsed schema object
   * @throws ParseException on parse error
   */
  public ParsedSchema parse() throws ParseException {
    ParsedSchema schema = new ParsedSchema();
    do {
      getNextToken();
      if (m_token == TokenKind.SEMICOLON) {
        continue;
      }
      if (m_token == TokenKind.END_OF_INPUT) {
        break;
      }
      schema.declarations.add(parseDeclaration());
    } while (m_token != TokenKind.END_OF_INPUT);
    return schema;
  }

  private ParsedDeclaration parseDeclaration() throws ParseException {
    ParsedDeclaration decl = new ParsedDeclaration();

    // optional enum specification
    if (m_token == TokenKind.IDENTIFIER && "enum".equals(m_lexer.getTokenText())) {
      getNextToken();
      expect(TokenKind.LEFT_BRACE);
      decl.enumValues = parseEnum();
      getNextToken();
    } else if (m_token == TokenKind.LEFT_BRACE) {
      decl.enumValues = parseEnum();
      getNextToken();
    }

    // type name
    expect(TokenKind.IDENTIFIER);
    decl.typeString = m_lexer.getTokenText();
    getNextToken();

    // identifier name
    expect(TokenKind.IDENTIFIER);
    decl.name = m_lexer.getTokenText();
    getNextToken();

    // array or bit field
    if (m_token == TokenKind.LEFT_BRACKET) {
      getNextToken();
      expect(TokenKind.INTEGER);
      String valueStr = m_lexer.getTokenText();
      int value;
      try {
        value = Integer.parseInt(valueStr);
      } catch (NumberFormatException e) {
        value = 0;
      }
      if (value > 0) {
        decl.arraySize = value;
      } else {
        throw new ParseException(
            m_lexer.m_pos, "array size '" + valueStr + "' is not a positive integer");
      }
      getNextToken();
      expect(TokenKind.RIGHT_BRACKET);
      getNextToken();
    } else if (m_token == TokenKind.COLON) {
      getNextToken();
      expect(TokenKind.INTEGER);
      String valueStr = m_lexer.getTokenText();
      int value;
      try {
        value = Integer.parseInt(valueStr);
      } catch (NumberFormatException e) {
        value = 0;
      }
      if (value > 0) {
        decl.bitWidth = value;
      } else {
        throw new ParseException(
            m_lexer.m_pos, "bitfield width '" + valueStr + "' is not a positive integer");
      }
      getNextToken();
    }

    // declaration must end with EOF or semicolon
    if (m_token != TokenKind.END_OF_INPUT) {
      expect(TokenKind.SEMICOLON);
    }

    return decl;
  }

  private Map<String, Long> parseEnum() throws ParseException {
    Map<String, Long> map = new HashMap<>();

    // we start with current = '{'
    getNextToken();
    while (m_token != TokenKind.RIGHT_BRACE) {
      expect(TokenKind.IDENTIFIER);
      final String name = m_lexer.getTokenText();
      getNextToken();
      expect(TokenKind.EQUALS);
      getNextToken();
      expect(TokenKind.INTEGER);
      String valueStr = m_lexer.getTokenText();
      long value;
      try {
        value = Long.parseLong(valueStr);
      } catch (NumberFormatException e) {
        throw new ParseException(m_lexer.m_pos, "could not parse enum value '" + valueStr + "'");
      }
      map.put(name, value);
      getNextToken();
      if (m_token == TokenKind.RIGHT_BRACE) {
        break;
      }
      expect(TokenKind.COMMA);
      getNextToken();
    }
    return map;
  }

  private TokenKind getNextToken() {
    m_token = m_lexer.scan();
    return m_token;
  }

  private void expect(TokenKind kind) throws ParseException {
    if (m_token != kind) {
      throw new ParseException(
          m_lexer.m_pos, "expected " + kind + ", got '" + m_lexer.getTokenText() + "'");
    }
  }

  final Lexer m_lexer;
  TokenKind m_token;
}
