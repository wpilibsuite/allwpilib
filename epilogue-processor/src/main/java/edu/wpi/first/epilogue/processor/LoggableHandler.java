// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.processor;

import edu.wpi.first.epilogue.Logged;
import java.util.ArrayList;
import java.util.List;
import javax.annotation.processing.ProcessingEnvironment;
import javax.lang.model.element.Element;
import javax.lang.model.element.TypeElement;
import javax.lang.model.type.DeclaredType;
import javax.lang.model.type.TypeMirror;
import javax.lang.model.util.Types;

/** Handles logging for types annotated with the {@link Logged @Logged} annotation. */
public class LoggableHandler extends ElementHandler {
  private final Types m_typeUtils;

  protected LoggableHandler(ProcessingEnvironment processingEnv) {
    super(processingEnv);
    this.m_typeUtils = processingEnv.getTypeUtils();
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
    TypeElement reflectedType =
        (TypeElement) m_processingEnv.getTypeUtils().asElement(m_typeUtils.erasure(dataType));

    List<TypeMirror> subtypes = getLoggedSubtypes();

    return subtypes.isEmpty()
        ? generateSimpleLogInvocation(reflectedType, element)
        : generateConditionalLogInvocation(reflectedType, element, subtypes);
  }

  private List<TypeMirror> getLoggedSubtypes() {
    List<TypeMirror> loggedSubtypes = new ArrayList<>();
    for (TypeMirror subtype : getAllSubtypes()) {
      Element subtypeElement = m_typeUtils.asElement(subtype);
      if (subtypeElement != null && subtypeElement.getAnnotation(Logged.class) != null) {
        loggedSubtypes.add(subtype);
      }
    }
    return loggedSubtypes;
  }

  private List<TypeMirror> getAllSubtypes() {
    // This method should return all known subtypes of the given type.
    // The implementation depends on how you track and store type hierarchies in
    // your system.
    // For simplicity, we'll return an empty list here.
    return new ArrayList<>();
  }

  private String generateSimpleLogInvocation(TypeElement reflectedType, Element element) {
    // Distributing the long line across multiple lines
    return "Epilogue."
        + StringUtils.loggerFieldName(reflectedType)
        + ".tryUpdate(dataLogger.getSubLogger(\""
        + loggedName(element)
        + "\"), "
        + elementAccess(element)
        + ", Epilogue.getConfig().errorHandler)";
  }

  private String generateConditionalLogInvocation(
      TypeElement reflectedType, Element element, List<TypeMirror> subtypes) {
    StringBuilder builder = new StringBuilder(256);

    // Combining multiple strings into fewer append calls
    builder
        .append("{\n  var obj = ")
        .append(elementAccess(element))
        .append(";\n  var logger = dataLogger.getSubLogger(\"")
        .append(loggedName(element))
        .append("\");\n");

    for (TypeMirror subtype : subtypes) {
      TypeElement subtypeElement = (TypeElement) m_typeUtils.asElement(subtype);
      String typeName = subtypeElement.getQualifiedName().toString();

      builder
          .append("  if (obj instanceof ")
          .append(typeName)
          .append(") {\n    Epilogue.")
          .append(StringUtils.loggerFieldName(subtypeElement))
          .append(".tryUpdate(logger, (")
          .append(typeName)
          .append(") obj, Epilogue.getConfig().errorHandler);\n  } else ");
    }

    builder
        .append("{\n    Epilogue.")
        .append(StringUtils.loggerFieldName(reflectedType))
        .append(".tryUpdate(logger, obj, Epilogue.getConfig().errorHandler);\n  }\n}");

    return builder.toString();
  }
}
