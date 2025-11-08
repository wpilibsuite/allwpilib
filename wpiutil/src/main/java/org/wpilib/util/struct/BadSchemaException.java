// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.struct;

/** Exception thrown when encountering a bad schema. */
public class BadSchemaException extends Exception {
  /** The bad schema field. */
  private final String m_field;

  /**
   * Constructs a BadSchemaException.
   *
   * @param message the detail message.
   */
  public BadSchemaException(String message) {
    super(message);
    m_field = "";
  }

  /**
   * Constructs a BadSchemaException.
   *
   * @param message the detail message.
   * @param cause The cause (which is saved for later retrieval by the {@link #getCause()} method).
   */
  public BadSchemaException(String message, Throwable cause) {
    super(message, cause);
    m_field = "";
  }

  /**
   * Constructs a BadSchemaException.
   *
   * @param cause The cause (which is saved for later retrieval by the {@link #getCause()} method).
   */
  public BadSchemaException(Throwable cause) {
    super(cause);
    m_field = "";
  }

  /**
   * Constructs a BadSchemaException.
   *
   * @param field The bad schema field.
   * @param message the detail message.
   */
  public BadSchemaException(String field, String message) {
    super(message);
    m_field = field;
  }

  /**
   * Constructs a BadSchemaException.
   *
   * @param field The bad schema field.
   * @param message the detail message.
   * @param cause The cause (which is saved for later retrieval by the {@link #getCause()} method).
   */
  public BadSchemaException(String field, String message, Throwable cause) {
    super(message, cause);
    m_field = field;
  }

  /**
   * Gets the name of the bad schema field.
   *
   * @return The name of the bad schema field, or an empty string if not applicable.
   */
  public String getField() {
    return m_field;
  }

  @Override
  public String getMessage() {
    return m_field.isEmpty() ? super.getMessage() : "field " + m_field + ": " + super.getMessage();
  }
}
