// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.processor;

import javax.annotation.processing.ProcessingEnvironment;
import javax.lang.model.element.Element;
import javax.lang.model.type.DeclaredType;
import javax.lang.model.type.TypeMirror;

/**
 * Collections of strings and structs are loggable. Collections of boxed primitive types are not.
 */
public class CollectionHandler extends ElementHandler {
  private final ArrayHandler m_arrayHandler;
  private final TypeMirror m_collectionType;
  private final StructHandler m_structHandler;

  protected CollectionHandler(ProcessingEnvironment processingEnv) {
    super(processingEnv);
    m_arrayHandler = new ArrayHandler(processingEnv);
    m_collectionType =
        processingEnv.getElementUtils().getTypeElement("java.util.Collection").asType();
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
        && m_arrayHandler.isLoggableComponentType(decl.getTypeArguments().get(0));
  }

  @Override
  public String logInvocation(Element element) {
    var dataType = dataType(element);
    var componentType = ((DeclaredType) dataType).getTypeArguments().get(0);

    if (m_structHandler.isLoggableType(componentType)) {
      return "backend.log(\""
          + loggedName(element)
          + "\", "
          + elementAccess(element)
          + ", "
          + m_structHandler.structAccess(componentType)
          + ")";
    } else {
      return "backend.log(\"" + loggedName(element) + "\", " + elementAccess(element) + ")";
    }
  }
}
