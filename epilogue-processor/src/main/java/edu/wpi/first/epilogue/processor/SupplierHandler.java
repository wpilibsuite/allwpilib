// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.processor;

import javax.annotation.processing.ProcessingEnvironment;
import javax.lang.model.element.Element;
import javax.lang.model.type.TypeMirror;

public class SupplierHandler extends ElementHandler {
  private final TypeMirror m_booleanSupplier;
  private final TypeMirror m_intSupplier;
  private final TypeMirror m_longSupplier;
  private final TypeMirror m_doubleSupplier;

  protected SupplierHandler(ProcessingEnvironment processingEnv) {
    super(processingEnv);

    m_booleanSupplier =
        processingEnv
            .getElementUtils()
            .getTypeElement("java.util.function.BooleanSupplier")
            .asType();
    m_intSupplier =
        processingEnv.getElementUtils().getTypeElement("java.util.function.IntSupplier").asType();
    m_longSupplier =
        processingEnv.getElementUtils().getTypeElement("java.util.function.LongSupplier").asType();
    m_doubleSupplier =
        processingEnv
            .getElementUtils()
            .getTypeElement("java.util.function.DoubleSupplier")
            .asType();
  }

  @Override
  public boolean isLoggable(Element element) {
    return m_processingEnv.getTypeUtils().isAssignable(dataType(element), m_booleanSupplier)
        || m_processingEnv.getTypeUtils().isAssignable(dataType(element), m_intSupplier)
        || m_processingEnv.getTypeUtils().isAssignable(dataType(element), m_longSupplier)
        || m_processingEnv.getTypeUtils().isAssignable(dataType(element), m_doubleSupplier);
  }

  @Override
  public String logInvocation(Element element) {
    return "backend.log(\"" + loggedName(element) + "\", " + elementAccess(element) + ")";
  }

  @Override
  public String elementAccess(Element element) {
    var typeUtils = m_processingEnv.getTypeUtils();
    var dataType = dataType(element);
    String base = super.elementAccess(element);

    if (typeUtils.isAssignable(dataType, m_booleanSupplier)) {
      return base + ".getAsBoolean()";
    } else if (typeUtils.isAssignable(dataType, m_intSupplier)) {
      return base + ".getAsInt()";
    } else if (typeUtils.isAssignable(dataType, m_longSupplier)) {
      return base + ".getAsLong()";
    } else if (typeUtils.isAssignable(dataType, m_doubleSupplier)) {
      return base + ".getAsDouble()";
    } else {
      throw new IllegalArgumentException("Element type is unsupported: " + dataType);
    }
  }
}
