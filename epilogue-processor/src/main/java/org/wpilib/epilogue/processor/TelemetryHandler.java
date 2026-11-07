// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.epilogue.processor;

import javax.annotation.processing.ProcessingEnvironment;
import javax.lang.model.element.Element;
import javax.lang.model.element.ExecutableElement;
import javax.lang.model.element.TypeElement;
import javax.lang.model.element.VariableElement;
import javax.lang.model.type.TypeMirror;

public class TelemetryHandler extends ElementHandler {
  private final TypeMirror m_telemetryType;

  protected TelemetryHandler(ProcessingEnvironment processingEnv) {
    super(processingEnv);

    m_telemetryType =
        lookupTypeElement(processingEnv, "org.wpilib.telemetry.TelemetryLoggable").asType();
  }

  @Override
  public boolean isLoggable(Element element) {
    var dataType = dataType(element);

    return m_processingEnv.getTypeUtils().isAssignable(dataType, m_telemetryType);
  }

  @Override
  public String logInvocation(Element element, TypeElement loggedClass) {
    String varName = cacheVariableName(element);

    return "var %s = %s;%nif (%s != null) {%n  table.log(\"%s\", %s);%n}"
        .formatted(
            varName, elementAccess(element, loggedClass), varName, loggedName(element), varName);
  }

  private static String cacheVariableName(Element element) {
    return switch (element) {
      case VariableElement field -> "$$%s".formatted(field.getSimpleName().toString());
      case ExecutableElement method -> "__%s".formatted(method.getSimpleName().toString());
      default -> element.getSimpleName().toString();
    };
  }
}
