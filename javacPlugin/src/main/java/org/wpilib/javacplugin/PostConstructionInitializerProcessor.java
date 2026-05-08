// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.javacplugin;

import java.util.Set;
import javax.annotation.processing.AbstractProcessor;
import javax.annotation.processing.RoundEnvironment;
import javax.lang.model.SourceVersion;
import javax.lang.model.element.Element;
import javax.lang.model.element.ExecutableElement;
import javax.lang.model.element.Modifier;
import javax.lang.model.element.Name;
import javax.lang.model.element.TypeElement;
import javax.lang.model.element.VariableElement;
import javax.tools.Diagnostic;
import org.wpilib.annotation.PostConstructionInitializer;
import org.wpilib.annotation.PostConstructionInitializer.InitializedParam;

/**
 * Sanity checks for {@link PostConstructionInitializer}-annotated methods. This does not check for
 * usages of the annotated method; that is handled by the {@link
 * PostConstructionInitializerListener} compiler plugin.
 */
public class PostConstructionInitializerProcessor extends AbstractProcessor {
  @Override
  public SourceVersion getSupportedSourceVersion() {
    return SourceVersion.latestSupported();
  }

  @Override
  public Set<String> getSupportedAnnotationTypes() {
    return Set.of(
        "org.wpilib.annotation.PostConstructionInitializer",
        "org.wpilib.annotation.PostConstructionInitializer.InitializedParam");
  }

  @Override
  public boolean process(Set<? extends TypeElement> annotations, RoundEnvironment roundEnv) {
    var annotatedElements = roundEnv.getElementsAnnotatedWith(PostConstructionInitializer.class);

    for (Element element : annotatedElements) {
      // Check static initializer methods.
      // Static initializers must either take exactly one parameter of the type of the class they're
      // in, or have exactly one parameter of the type that's annotated with @InitializedParam
      if (element instanceof ExecutableElement exec
          && exec.getModifiers().contains(Modifier.STATIC)
          && exec.getEnclosingElement() instanceof TypeElement type) {
        Name typeName = type.getQualifiedName();

        var typedParameters =
            exec.getParameters().stream()
                .filter(
                    p ->
                        processingEnv
                            .getTypeUtils()
                            .isSameType(p.asType(), exec.getEnclosingElement().asType()))
                .toList();

        switch (typedParameters.size()) {
          case 0 -> printErrorForNoParams(exec, typeName);
          case 1 -> {
            // No ambiguity
          }
          default -> {
            // Multiple parameters.
            // Require exactly one with a @PostConstructionInitializer.InitializedParam annotation,
            // for disambiguation
            var taggedParameters =
                typedParameters.stream()
                    .filter(p -> p.getAnnotation(InitializedParam.class) != null)
                    .toList();
            if (taggedParameters.isEmpty()) {
              printTaggedParameterCountError(exec, typeName);
            } else if (taggedParameters.size() > 1) {
              for (VariableElement taggedParameter : taggedParameters) {
                printTaggedParameterCountError(taggedParameter, typeName);
              }
            }
          }
        }
      }
    }

    return false;
  }

  private void printErrorForNoParams(Element errorNode, Name typeName) {
    String message =
        "Static @PostConstructionInitializer method must take a parameter of type " + typeName;

    processingEnv.getMessager().printMessage(Diagnostic.Kind.ERROR, message, errorNode);
  }

  private void printTaggedParameterCountError(Element errorNode, Name typeName) {
    String message =
        "Static @PostConstructionInitializer method must take exactly one parameter of type "
            + typeName
            + " with a @PostConstructionInitializer.InitializedParam annotation";

    processingEnv.getMessager().printMessage(Diagnostic.Kind.ERROR, message, errorNode);
  }
}
