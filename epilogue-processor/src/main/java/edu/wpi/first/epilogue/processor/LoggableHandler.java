// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.processor;

import edu.wpi.first.epilogue.Logged;
import javax.annotation.processing.ProcessingEnvironment;
import javax.lang.model.element.Element;
import javax.lang.model.type.DeclaredType;
import javax.lang.model.type.TypeMirror;

/** Handles logging for types annotated with the {@link Logged @Logged} annotation. */
public class LoggableHandler extends ElementHandler {
  protected LoggableHandler(ProcessingEnvironment processingEnv) {
    super(processingEnv);
  }

  @Override
  public boolean isLoggable(Element element) {
    var dataType = dataType(element);
    return dataType.getAnnotation(Logged.class) != null
        || dataType instanceof DeclaredType decl
            && decl.asElement().getAnnotation(Logged.class) != null;
  }

  @Override
  public String logInvocation(Element element) {
    TypeMirror dataType = dataType(element);
    var reflectedType =
        m_processingEnv
            .getElementUtils()
            .getTypeElement(m_processingEnv.getTypeUtils().erasure(dataType).toString());

    return "Epilogue."
        + StringUtils.loggerFieldName(reflectedType)
        + ".tryUpdate(dataLogger.getSubLogger(\""
        + loggedName(element)
        + "\"), "
        + elementAccess(element)
        + ", Epilogue.getConfig().errorHandler)";
  }
}
