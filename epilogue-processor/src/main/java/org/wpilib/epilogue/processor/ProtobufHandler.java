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
import javax.lang.model.type.TypeMirror;
import javax.lang.model.util.Elements;
import javax.lang.model.util.Types;

/**
 * Supports protobuf serializable types. Protobuf-serializable types are loggable if they have a
 * public static final {@code proto} field of a type that inherits from {@code Protobuf}.
 */
public class ProtobufHandler extends ElementHandler {
  private final TypeMirror m_serializable;
  private final TypeElement m_protobufType;
  private final Types m_typeUtils;
  private final Elements m_elementUtils;

  protected ProtobufHandler(ProcessingEnvironment processingEnv) {
    super(processingEnv);

    m_serializable =
        processingEnv
            .getElementUtils()
            .getTypeElement("org.wpilib.util.protobuf.ProtobufSerializable")
            .asType();
    m_protobufType =
        processingEnv.getElementUtils().getTypeElement("org.wpilib.util.protobuf.Protobuf");
    m_typeUtils = processingEnv.getTypeUtils();
    m_elementUtils = processingEnv.getElementUtils();
  }

  @Override
  public boolean isLoggable(Element element) {
    return isLoggableType(dataType(element));
  }

  /**
   * Checks if a type is protobuf-serializable: implements the ProtobufSerializable marker interface
   * and has a `public static final proto` field of a type that inherits from Protobuf with a
   * compatible generic type bound.
   *
   * @param type The type to check
   * @return true if the type is protobuf-serializable, false otherwise
   */
  public boolean isLoggableType(TypeMirror type) {
    var serializableType = m_typeUtils.erasure(type);
    var typeElement = m_elementUtils.getTypeElement(serializableType.toString());
    if (typeElement == null) {
      return false;
    }

    // Build a type like `Protobuf<SerializableType, ? extends ProtoMessage<?>>` to match
    // implementations such as `SchedulerProto implements Protobuf<Scheduler, ProtobufScheduler>`.
    // Note: Using `ProtoMessage` raw as the upper bound causes assignability to fail; we must use
    // `ProtoMessage<?>` as the bound, then wrap that with an extends wildcard.
    var protoMessageElement = m_elementUtils.getTypeElement("us.hebi.quickbuf.ProtoMessage");
    var protoMessageWildcard =
        m_typeUtils.getDeclaredType(protoMessageElement, m_typeUtils.getWildcardType(null, null));
    var boundedProtoMessageWildcard = m_typeUtils.getWildcardType(protoMessageWildcard, null);

    var sharpProtobufType =
        m_typeUtils.getDeclaredType(
            m_protobufType,
            typeElement.asType(), // the serializable type
            boundedProtoMessageWildcard);

    boolean hasProto =
        typeElement.getEnclosedElements().stream()
            .filter(e -> e instanceof VariableElement)
            .map(e -> (VariableElement) e)
            .anyMatch(
                field -> {
                  var nameMatch = field.getSimpleName().contentEquals("proto");
                  var modifiersMatch =
                      field
                          .getModifiers()
                          .containsAll(Set.of(Modifier.PUBLIC, Modifier.STATIC, Modifier.FINAL));
                  var typeMatch = m_typeUtils.isAssignable(field.asType(), sharpProtobufType);
                  return nameMatch && modifiersMatch && typeMatch;
                });
    return m_typeUtils.isAssignable(type, m_serializable) && hasProto;
  }

  public String protoAccess(TypeMirror serializableType) {
    var className = m_typeUtils.erasure(serializableType).toString();
    return className + ".proto";
  }

  @Override
  public String logInvocation(Element element, TypeElement loggedClass) {
    return "backend.log(\"%s\", %s, %s)"
        .formatted(
            loggedName(element),
            elementAccess(element, loggedClass),
            protoAccess(dataType(element)));
  }
}
