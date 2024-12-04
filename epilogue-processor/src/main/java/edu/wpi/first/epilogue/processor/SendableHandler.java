// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.processor;

import javax.annotation.processing.ProcessingEnvironment;
import javax.lang.model.element.Element;
import javax.lang.model.type.TypeMirror;

public class SendableHandler extends ElementHandler {
  private final TypeMirror m_sendableType;
  private final TypeMirror m_commandType;
  private final TypeMirror m_subsystemType;

  protected SendableHandler(ProcessingEnvironment processingEnv) {
    super(processingEnv);

    m_sendableType =
        lookupTypeElement(processingEnv, "edu.wpi.first.util.sendable.Sendable").asType();
    m_commandType =
        lookupTypeElement(processingEnv, "edu.wpi.first.wpilibj2.command.Command").asType();
    m_subsystemType =
        lookupTypeElement(processingEnv, "edu.wpi.first.wpilibj2.command.SubsystemBase").asType();
  }

  @Override
  public boolean isLoggable(Element element) {
    var dataType = dataType(element);

    // Accept any sendable type. However, the log invocation will return null
    // for sendable types that should not be logged (commands, subsystems)
    return m_processingEnv.getTypeUtils().isAssignable(dataType, m_sendableType);
  }

  @Override
  public String logInvocation(Element element) {
    var dataType = dataType(element);

    if (m_processingEnv.getTypeUtils().isAssignable(dataType, m_commandType)
        || m_processingEnv.getTypeUtils().isAssignable(dataType, m_subsystemType)) {
      // Do not log commands or subsystems via their sendable implementations
      // We accept all sendable objects to prevent them from being reported as not loggable,
      // but their sendable implementations do not include helpful information.
      // Users are free to provide custom logging implementations for commands, and tag their
      // subsystems with @Logged to log their contents automatically
      return null;
    }

    return "logSendable(backend.getNested(\""
        + loggedName(element)
        + "\"), "
        + elementAccess(element)
        + ")";
  }
}
