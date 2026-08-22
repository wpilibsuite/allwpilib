// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.javacplugin;

import java.util.Set;
import javax.annotation.processing.AbstractProcessor;
import javax.annotation.processing.Messager;
import javax.annotation.processing.RoundEnvironment;
import javax.lang.model.SourceVersion;
import javax.lang.model.element.Element;
import javax.lang.model.element.ElementKind;
import javax.lang.model.element.Modifier;
import javax.lang.model.element.TypeElement;
import javax.tools.Diagnostic;

/**
 * Detects usages of OpMode annotations on classes that do not implement the OpMode interface and
 * non-concrete types (abstract classes, enums, or interfaces) with an annotation.
 */
public class OpModeAnnotationProcessor extends AbstractProcessor {
  @Override
  public Set<String> getSupportedAnnotationTypes() {
    return Set.of(
        "org.wpilib.opmode.Autonomous", "org.wpilib.opmode.Teleop", "org.wpilib.opmode.Utility");
  }

  @Override
  public SourceVersion getSupportedSourceVersion() {
    return SourceVersion.latestSupported();
  }

  @Override
  public boolean process(Set<? extends TypeElement> annotations, RoundEnvironment roundEnv) {
    final Messager messager = processingEnv.getMessager();

    TypeElement opmodeType =
        processingEnv.getElementUtils().getTypeElement("org.wpilib.opmode.OpMode");

    for (TypeElement annotationType : annotations) {
      for (Element element : roundEnv.getElementsAnnotatedWith(annotationType)) {
        if (!(element instanceof TypeElement type)) {
          // Can't get here; the annotation types only declare ElementType.TYPE as the target.
          continue;
        }

        if (isNonConcreteType(type)) {
          // Not an instantiable type
          messager.printMessage(
              Diagnostic.Kind.ERROR,
              "The @"
                  + annotationType.getSimpleName()
                  + " OpMode annotation can only be used on non-abstract classes",
              type);
        }

        if (!processingEnv.getTypeUtils().isAssignable(type.asType(), opmodeType.asType())) {
          // Doesn't implement `OpMode` or extend a class that does
          messager.printMessage(
              Diagnostic.Kind.ERROR,
              "The @"
                  + annotationType.getSimpleName()
                  + " OpMode annotation can only be used on classes that implement the "
                  + "org.wpilib.opmode.OpMode interface or extend a class that does",
              type);
        }
      }
    }

    return false;
  }

  private static boolean isNonConcreteType(TypeElement type) {
    return type.getModifiers().contains(Modifier.ABSTRACT) || type.getKind() == ElementKind.ENUM;
  }
}
