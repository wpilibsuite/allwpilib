// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.processor;

import static javax.lang.model.type.TypeKind.BOOLEAN;
import static javax.lang.model.type.TypeKind.BYTE;
import static javax.lang.model.type.TypeKind.CHAR;
import static javax.lang.model.type.TypeKind.DOUBLE;
import static javax.lang.model.type.TypeKind.FLOAT;
import static javax.lang.model.type.TypeKind.INT;
import static javax.lang.model.type.TypeKind.LONG;
import static javax.lang.model.type.TypeKind.SHORT;

import java.util.Set;
import javax.annotation.processing.ProcessingEnvironment;
import javax.lang.model.element.Element;
import javax.lang.model.type.TypeMirror;

public class PrimitiveHandler extends ElementHandler {
  private final TypeMirror m_javaLangString;

  protected PrimitiveHandler(ProcessingEnvironment processingEnv) {
    super(processingEnv);

    m_javaLangString = processingEnv.getElementUtils().getTypeElement("java.lang.String").asType();
  }

  @Override
  public boolean isLoggable(Element element) {
    return m_processingEnv.getTypeUtils().isAssignable(dataType(element), m_javaLangString)
        || Set.of(BYTE, CHAR, SHORT, INT, LONG, FLOAT, DOUBLE, BOOLEAN)
            .contains(dataType(element).getKind());
  }

  @Override
  public String logInvocation(Element element) {
    return "backend.log(\"" + loggedName(element) + "\", " + elementAccess(element) + ")";
  }
}
