// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.epilogue.processor;

import javax.annotation.processing.ProcessingEnvironment;
import javax.lang.model.element.Element;
import javax.lang.model.element.TypeElement;
import javax.lang.model.type.DeclaredType;
import javax.lang.model.type.TypeMirror;

/** Collections of strings, structs, boxed booleans, and boxed numbers are loggable. */
public class CollectionHandler extends ElementHandler {
  private final ArrayHandler m_arrayHandler;
  private final TypeMirror m_collectionType;
  private final TypeMirror m_javaLangBoolean;
  private final TypeMirror m_javaLangNumber;
  private final StructHandler m_structHandler;

  protected CollectionHandler(ProcessingEnvironment processingEnv) {
    super(processingEnv);
    m_arrayHandler = new ArrayHandler(processingEnv);
    m_collectionType =
        processingEnv.getElementUtils().getTypeElement("java.util.Collection").asType();
    m_javaLangBoolean = lookupTypeElement(processingEnv, "java.lang.Boolean").asType();
    m_javaLangNumber = lookupTypeElement(processingEnv, "java.lang.Number").asType();
    m_structHandler = new StructHandler(processingEnv);
  }

  @Override
  public boolean isLoggable(Element element) {
    var dataType = dataType(element);

    return m_processingEnv
            .getTypeUtils()
            .isAssignable(dataType, m_processingEnv.getTypeUtils().erasure(m_collectionType))
        && dataType instanceof DeclaredType decl
        && decl.getTypeArguments().size() == 1
        && isLoggableComponentType(decl.getTypeArguments().get(0));
  }

  private boolean isLoggableComponentType(TypeMirror type) {
    return m_arrayHandler.isLoggableComponentType(type)
        || m_processingEnv.getTypeUtils().isAssignable(type, m_javaLangBoolean)
        || m_processingEnv.getTypeUtils().isAssignable(type, m_javaLangNumber);
  }

  @Override
  public String logInvocation(Element element, TypeElement loggedClass) {
    var dataType = dataType(element);
    var componentType = ((DeclaredType) dataType).getTypeArguments().get(0);

    if (m_structHandler.isLoggableType(componentType)) {
      return "table.log(\""
          + loggedName(element)
          + "\", "
          + elementAccess(element, loggedClass)
          + ", "
          + m_structHandler.structAccess(componentType)
          + ")";
    } else {
      return "table.log(\""
          + loggedName(element)
          + "\", "
          + elementAccess(element, loggedClass)
          + ", "
          + m_processingEnv.getTypeUtils().erasure(componentType)
          + ".class"
          + ")";
    }
  }
}
