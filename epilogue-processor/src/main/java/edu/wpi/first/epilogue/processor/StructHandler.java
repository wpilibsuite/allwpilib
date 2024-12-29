// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.processor;

import javax.annotation.processing.ProcessingEnvironment;
import javax.lang.model.element.Element;
import javax.lang.model.type.TypeMirror;
import javax.lang.model.util.Types;

public class StructHandler extends ElementHandler {
  private final TypeMirror m_serializable;
  private final Types m_typeUtils;

  protected StructHandler(ProcessingEnvironment processingEnv) {
    super(processingEnv);
    m_serializable =
        processingEnv
            .getElementUtils()
            .getTypeElement("edu.wpi.first.util.struct.StructSerializable")
            .asType();
    m_typeUtils = processingEnv.getTypeUtils();
  }

  @Override
  public boolean isLoggable(Element element) {
    return m_typeUtils.isAssignable(dataType(element), m_serializable);
  }

  public boolean isLoggableType(TypeMirror type) {
    return m_typeUtils.isAssignable(type, m_serializable);
  }

  public String structAccess(TypeMirror serializableType) {
    var className = m_typeUtils.erasure(serializableType).toString();
    return className + ".struct";
  }

  @Override
  public String logInvocation(Element element) {
    return "backend.log(\""
        + loggedName(element)
        + "\", "
        + elementAccess(element)
        + ", "
        + structAccess(dataType(element))
        + ")";
  }
}
