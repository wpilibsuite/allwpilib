// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.processor;

import edu.wpi.first.epilogue.Logged;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import javax.annotation.processing.ProcessingEnvironment;
import javax.lang.model.element.Element;
import javax.lang.model.element.ElementKind;
import javax.lang.model.element.TypeElement;
import javax.lang.model.type.DeclaredType;
import javax.lang.model.type.TypeMirror;
import javax.lang.model.util.ElementScanner9;
import javax.lang.model.util.Elements;
import javax.lang.model.util.Types;

/** Handles logging for types annotated with the {@link Logged @Logged} annotation. */
public class LoggableHandler extends ElementHandler {
  private final Types m_typeUtils;
  private final Elements m_elementUtils;

  protected LoggableHandler(ProcessingEnvironment processingEnv) {
    super(processingEnv);
    this.m_typeUtils = processingEnv.getTypeUtils();
    this.m_elementUtils = processingEnv.getElementUtils();
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

    List<TypeMirror> subtypes = getLoggedSubtypes(dataType);

    return subtypes.isEmpty()
        ? generateSimpleLogInvocation(reflectedType, element)
        : generateConditionalLogInvocation(reflectedType, element, subtypes);
  }

  private List<TypeMirror> getLoggedSubtypes(TypeMirror type) {
    List<TypeMirror> loggedSubtypes = new ArrayList<>();
    for (TypeMirror subtype : getAllSubtypes(type)) {
      Element subtypeElement = m_typeUtils.asElement(subtype);
      if (subtypeElement != null && subtypeElement.getAnnotation(Logged.class) != null) {
        loggedSubtypes.add(subtype);
      }
    }
    return loggedSubtypes;
  }

  private List<TypeMirror> getAllSubtypes(TypeMirror type) {
    List<TypeMirror> allSubtypes = new ArrayList<>();
    Set<TypeElement> allTypes = findAllTypes();

    for (TypeElement potentialSubtype : allTypes) {
      TypeMirror potentialSubtypeMirror = potentialSubtype.asType();
      if (m_typeUtils.isSubtype(potentialSubtypeMirror, type)
          && !potentialSubtypeMirror.equals(type)) {
        allSubtypes.add(potentialSubtypeMirror);
      }
    }

    return allSubtypes;
  }

  private Set<TypeElement> findAllTypes() {
    Set<TypeElement> allTypes = new HashSet<>();

    // This scanner will collect all TypeElements from the root elements in the compilation unit
    ElementScanner9<Void, Void> scanner =
        new ElementScanner9<>() {
          @Override
          public Void visitType(TypeElement e, Void p) {
            allTypes.add(e);
            return super.visitType(e, p);
          }
        };

    // Iterate through all elements and scan
    for (Element rootElement : m_elementUtils.getAllModuleElements()) {
      if (rootElement.getKind() == ElementKind.PACKAGE) {
        rootElement.accept(scanner, null);
      }
    }

    return allTypes;
  }

  private String generateSimpleLogInvocation(TypeElement reflectedType, Element element) {
    return String.format(
        "Epilogue.%s.tryUpdate(dataLogger.getSubLogger(\"%s\"), "
            + "%s, Epilogue.getConfig().errorHandler);",
        StringUtils.loggerFieldName(reflectedType), loggedName(element), elementAccess(element));
  }

  private String generateConditionalLogInvocation(
      TypeElement reflectedType, Element element, List<TypeMirror> subtypes) {
    StringBuilder builder = new StringBuilder(256);

    // Combining multiple strings into fewer append calls for better readability
    builder
        .append("{\n  var obj = ")
        .append(elementAccess(element))
        .append(";\n")
        .append("  var logger = dataLogger.getSubLogger(\"")
        .append(loggedName(element))
        .append("\");\n");

    for (TypeMirror subtype : subtypes) {
      TypeElement subtypeElement = (TypeElement) m_typeUtils.asElement(subtype);
      String typeName = subtypeElement.getQualifiedName().toString();

      builder
          .append("  if (obj instanceof ")
          .append(typeName)
          .append(") {\n")
          .append("    Epilogue.")
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
