// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.processor;

import java.util.Optional;
import javax.annotation.processing.ProcessingEnvironment;
import javax.lang.model.element.Element;
import javax.lang.model.element.TypeElement;
import javax.lang.model.type.TypeMirror;

public class SendableHandler extends ElementHandler {
  private final Optional<TypeMirror> m_sendableType;
  private final Optional<TypeMirror> m_commandType;
  private final Optional<TypeMirror> m_subsystemType;

  protected SendableHandler(ProcessingEnvironment processingEnv) {
    super(processingEnv);

    m_sendableType =
        Optional.ofNullable(
                lookupTypeElement(processingEnv, "edu.wpi.first.util.sendable.Sendable"))
            .map(TypeElement::asType);
    m_commandType =
        Optional.ofNullable(
                lookupTypeElement(processingEnv, "edu.wpi.first.wpilibj2.command.Command"))
            .map(TypeElement::asType);
    m_subsystemType =
        Optional.ofNullable(
                lookupTypeElement(processingEnv, "edu.wpi.first.wpilibj2.command.SubsystemBase"))
            .map(TypeElement::asType);
  }

  @Override
  public boolean isLoggable(Element element) {
    var dataType = dataType(element);

    // Accept any sendable type. However, the log invocation will return null
    // for sendable types that should not be logged (commands, subsystems)
    return m_sendableType
        .map(t -> m_processingEnv.getTypeUtils().isAssignable(dataType, t))
        .orElse(false);
  }

  @Override
  public String logInvocation(Element element) {
    var dataType = dataType(element);

    // Do not log commands or subsystems via their sendable implementations
    // We accept all sendable objects to prevent them from being reported as not loggable,
    // but their sendable implementations do not include helpful information.
    // Users are free to provide custom logging implementations for commands, and tag their
    // subsystems with @Logged to log their contents automatically
    if (m_commandType
        .map(t -> m_processingEnv.getTypeUtils().isAssignable(dataType, t))
        .orElse(false)) {
      return null;
    }
    if (m_subsystemType
        .map(t -> m_processingEnv.getTypeUtils().isAssignable(dataType, t))
        .orElse(false)) {
      return null;
    }

    return "logSendable(backend.getNested(\""
        + loggedName(element)
        + "\"), "
        + elementAccess(element)
        + ")";
  }
}
