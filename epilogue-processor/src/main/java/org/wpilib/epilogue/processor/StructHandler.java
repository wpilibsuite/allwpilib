// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.epilogue.processor;

import java.util.Set;
import javax.annotation.processing.ProcessingEnvironment;
import javax.lang.model.element.Element;
import javax.lang.model.element.Modifier;
import javax.lang.model.element.TypeElement;
import javax.lang.model.element.VariableElement;
import javax.lang.model.type.ArrayType;
import javax.lang.model.type.TypeMirror;
import javax.lang.model.util.Elements;
import javax.lang.model.util.Types;

/**
 * Supports struct serializable types. Struct-serializable types are loggable if they have a public
 * static final {@code struct} field of a type that inherits from {@code Struct}.
 */
public class StructHandler extends ElementHandler {
  private final TypeMirror m_serializable;
  private final TypeElement m_structType;
  private final Types m_typeUtils;
  private final Elements m_elementUtils;

  protected StructHandler(ProcessingEnvironment processingEnv) {
    super(processingEnv);
    m_serializable =
        processingEnv
            .getElementUtils()
            .getTypeElement("org.wpilib.util.struct.StructSerializable")
            .asType();
    m_structType = processingEnv.getElementUtils().getTypeElement("org.wpilib.util.struct.Struct");
    m_typeUtils = processingEnv.getTypeUtils();
    m_elementUtils = processingEnv.getElementUtils();
  }

  @Override
  public boolean isLoggable(Element element) {
    return isLoggableType(dataType(element));
  }

  /**
   * Checks if a type is struct-serializable: implements the StructSerializable marker interface and
   * has a `public static final struct` field of a type that inherits from Struct with a compatible
   * generic type bound.
   *
   * @param type The type to check
   * @return true if the type is struct-serializable, false otherwise
   */
  public boolean isLoggableType(TypeMirror type) {
    TypeMirror serializableType;
    if (type instanceof ArrayType arr) {
      serializableType = arr.getComponentType();
    } else {
      serializableType = m_typeUtils.erasure(type);
    }
    var typeElement = m_elementUtils.getTypeElement(serializableType.toString());
    if (typeElement == null) {
      return false;
    }

    // eg `Struct<Rotation2d>` instead of the raw `Struct` type
    var sharpStructType = m_typeUtils.getDeclaredType(m_structType, typeElement.asType());

    boolean hasStruct =
        typeElement.getEnclosedElements().stream()
            .filter(e -> e instanceof VariableElement)
            .map(e -> (VariableElement) e)
            .anyMatch(
                field -> {
                  var nameMatch = field.getSimpleName().contentEquals("struct");
                  var modifiersMatch =
                      field
                          .getModifiers()
                          .containsAll(Set.of(Modifier.PUBLIC, Modifier.STATIC, Modifier.FINAL));
                  var typeMatch =
                      m_typeUtils.isAssignable(
                          m_typeUtils.erasure(field.asType()), sharpStructType);
                  return nameMatch && modifiersMatch && typeMatch;
                });
    return m_typeUtils.isAssignable(type, m_serializable) && hasStruct;
  }

  public String structAccess(TypeMirror serializableType) {
    var className = m_typeUtils.erasure(serializableType).toString();
    return className + ".struct";
  }

  @Override
  public String logInvocation(Element element, TypeElement loggedClass) {
    return "backend.log(\""
        + loggedName(element)
        + "\", "
        + elementAccess(element, loggedClass)
        + ", "
        + structAccess(dataType(element))
        + ")";
  }
}
