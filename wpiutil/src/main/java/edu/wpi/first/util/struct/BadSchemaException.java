// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.struct;

public class BadSchemaException extends Exception {
  private final String m_field;

  public BadSchemaException(String s) {
    super(s);
    m_field = "";
  }

  public BadSchemaException(String message, Throwable cause) {
    super(message, cause);
    m_field = "";
  }

  public BadSchemaException(Throwable cause) {
    super(cause);
    m_field = "";
  }

  public BadSchemaException(String field, String s) {
    super(s);
    m_field = field;
  }

  public BadSchemaException(String field, String message, Throwable cause) {
    super(message, cause);
    m_field = field;
  }

  public String getField() {
    return m_field;
  }

  @Override
  public String toString() {
    return m_field.isEmpty() ? getMessage() : "field " + m_field + ": " + getMessage();
  }
}
