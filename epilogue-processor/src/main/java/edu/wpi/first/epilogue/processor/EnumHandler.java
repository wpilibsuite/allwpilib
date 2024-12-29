// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.processor;

import javax.annotation.processing.ProcessingEnvironment;
import javax.lang.model.element.Element;
import javax.lang.model.type.TypeMirror;

public class EnumHandler extends ElementHandler {
  private final TypeMirror m_javaLangEnum;

  protected EnumHandler(ProcessingEnvironment processingEnv) {
    super(processingEnv);

    // raw type java.lang.Enum
    m_javaLangEnum =
        processingEnv
            .getTypeUtils()
            .erasure(processingEnv.getElementUtils().getTypeElement("java.lang.Enum").asType());
  }

  @Override
  public boolean isLoggable(Element element) {
    return m_processingEnv.getTypeUtils().isAssignable(dataType(element), m_javaLangEnum);
  }

  @Override
  public String logInvocation(Element element) {
    return "backend.log(\"" + loggedName(element) + "\", " + elementAccess(element) + ")";
  }
}
