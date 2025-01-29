// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.processor;

import edu.wpi.first.epilogue.CustomLoggerFor;
import edu.wpi.first.epilogue.Logged;
import edu.wpi.first.epilogue.NotLogged;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.stream.Collectors;
import java.util.stream.Stream;
import javax.annotation.processing.AbstractProcessor;
import javax.annotation.processing.RoundEnvironment;
import javax.annotation.processing.SupportedAnnotationTypes;
import javax.annotation.processing.SupportedSourceVersion;
import javax.lang.model.SourceVersion;
import javax.lang.model.element.AnnotationMirror;
import javax.lang.model.element.AnnotationValue;
import javax.lang.model.element.Element;
import javax.lang.model.element.ElementKind;
import javax.lang.model.element.ExecutableElement;
import javax.lang.model.element.Modifier;
import javax.lang.model.element.TypeElement;
import javax.lang.model.element.VariableElement;
import javax.lang.model.type.DeclaredType;
import javax.lang.model.type.NoType;
import javax.lang.model.type.TypeKind;
import javax.lang.model.type.TypeMirror;
import javax.tools.Diagnostic;

@SupportedAnnotationTypes({
  "edu.wpi.first.epilogue.CustomLoggerFor",
  "edu.wpi.first.epilogue.Logged"
})
@SupportedSourceVersion(SourceVersion.RELEASE_17)
public class AnnotationProcessor extends AbstractProcessor {
  private static final String kCustomLoggerFqn = "edu.wpi.first.epilogue.CustomLoggerFor";
  private static final String kClassSpecificLoggerFqn =
      "edu.wpi.first.epilogue.logging.ClassSpecificLogger";
  private static final String kLoggedFqn = "edu.wpi.first.epilogue.Logged";

  private EpilogueGenerator m_epiloguerGenerator;
  private LoggerGenerator m_loggerGenerator;
  private List<ElementHandler> m_handlers;

  @Override
  public boolean process(Set<? extends TypeElement> annotations, RoundEnvironment roundEnv) {
    if (annotations.isEmpty()) {
      // Nothing to do, don't claim
      return false;
    }

    Map<TypeMirror, DeclaredType> customLoggers = new HashMap<>();

    annotations.stream()
        .filter(ann -> kCustomLoggerFqn.contentEquals(ann.getQualifiedName()))
        .findAny()
        .ifPresent(
            customLogger -> {
              customLoggers.putAll(processCustomLoggers(roundEnv, customLogger));
            });

    // Get all root types (classes and interfaces), excluding packages and modules
    roundEnv.getRootElements().stream()
        .filter(e -> e instanceof TypeElement)
        .filter(
            e ->
                processingEnv
                    .getTypeUtils()
                    .isAssignable(
                        e.asType(),
                        processingEnv
                            .getTypeUtils()
                            .erasure(
                                processingEnv
                                    .getElementUtils()
                                    .getTypeElement(kClassSpecificLoggerFqn)
                                    .asType())))
        .filter(e -> e.getAnnotation(CustomLoggerFor.class) == null)
        .forEach(
            e -> {
              processingEnv
                  .getMessager()
                  .printMessage(
                      Diagnostic.Kind.ERROR,
                      "[EPILOGUE] Custom logger classes should have a @CustomLoggerFor annotation",
                      e);
            });

    var loggedTypes = getLoggedTypes(roundEnv);

    // Handlers are declared in order of priority. If an element could be logged in more than one
    // way (eg a class implements both Sendable and StructSerializable), the order of the handlers
    // in this list will determine how it gets logged.
    m_handlers =
        List.of(
            new LoggableHandler(
                processingEnv, loggedTypes), // prioritize epilogue logging over Sendable
            new ConfiguredLoggerHandler(
                processingEnv, customLoggers), // then customized logging configs
            new ArrayHandler(processingEnv),
            new CollectionHandler(processingEnv),
            new EnumHandler(processingEnv),
            new MeasureHandler(processingEnv),
            new PrimitiveHandler(processingEnv),
            new SupplierHandler(processingEnv),
            new StructHandler(processingEnv), // prioritize struct over sendable
            new SendableHandler(processingEnv));

    m_epiloguerGenerator = new EpilogueGenerator(processingEnv, customLoggers);
    m_loggerGenerator = new LoggerGenerator(processingEnv, m_handlers);

    annotations.stream()
        .filter(ann -> kLoggedFqn.contentEquals(ann.getQualifiedName()))
        .findAny()
        .ifPresent(
            epilogue -> {
              processEpilogue(roundEnv, epilogue, loggedTypes);
            });

    return false;
  }

  /**
   * Gets the set of all loggable types in the compilation unit. A type is considered loggable if it
   * is directly annotated with {@code @Logged} or contains a field or method with a {@code @Logged}
   * annotation.
   *
   * @param roundEnv the compilation round environment
   * @return the set of all loggable types
   */
  private Set<TypeElement> getLoggedTypes(RoundEnvironment roundEnv) {
    // Fetches everything annotated with @Logged; classes, methods, values, etc.
    var annotatedElements = roundEnv.getElementsAnnotatedWith(Logged.class);
    return Stream.concat(
            // 1. All type elements (classes, interfaces, or enums) with the @Logged annotation
            annotatedElements.stream()
                .filter(e -> e instanceof TypeElement)
                .map(e -> (TypeElement) e),
            // 2. All type elements containing a field or method with the @Logged annotation
            annotatedElements.stream()
                .filter(e -> e instanceof VariableElement || e instanceof ExecutableElement)
                .map(Element::getEnclosingElement)
                .filter(e -> e instanceof TypeElement)
                .map(e -> (TypeElement) e))
        .sorted(Comparator.comparing(e -> e.getSimpleName().toString()))
        .collect(
            Collectors.toCollection(LinkedHashSet::new)); // Collect to a set to avoid duplicates
  }

  private boolean validateFields(Set<? extends Element> annotatedElements) {
    var fields =
        annotatedElements.stream()
            .filter(e -> e instanceof VariableElement)
            .map(e -> (VariableElement) e)
            .toList();

    boolean valid = true;

    for (VariableElement field : fields) {
      // Field is explicitly tagged
      // And is not opted out of
      if (field.getAnnotation(NotLogged.class) == null && isNotLoggable(field, field.asType())) {
        // And is not of a loggable type
        processingEnv
            .getMessager()
            .printMessage(
                Diagnostic.Kind.ERROR,
                "[EPILOGUE] You have opted in to logging on this field, "
                    + "but it is not a loggable data type!",
                field);
        valid = false;
      }
    }
    return valid;
  }

  private boolean validateMethods(Set<? extends Element> annotatedElements) {
    var methods =
        annotatedElements.stream()
            .filter(e -> e instanceof ExecutableElement)
            .map(e -> (ExecutableElement) e)
            .toList();

    boolean valid = true;

    for (ExecutableElement method : methods) {
      // Field is explicitly tagged
      if (method.getAnnotation(NotLogged.class) == null) {
        // And is not opted out of
        if (isNotLoggable(method, method.getReturnType())) {
          // And is not of a loggable type
          processingEnv
              .getMessager()
              .printMessage(
                  Diagnostic.Kind.ERROR,
                  "[EPILOGUE] You have opted in to logging on this method, "
                      + "but it does not return a loggable data type!",
                  method);
          valid = false;
        }

        if (!method.getModifiers().contains(Modifier.PUBLIC)) {
          // Only public methods can be logged

          processingEnv
              .getMessager()
              .printMessage(
                  Diagnostic.Kind.ERROR, "[EPILOGUE] Logged methods must be public", method);

          valid = false;
        }

        if (method.getModifiers().contains(Modifier.STATIC)) {
          processingEnv
              .getMessager()
              .printMessage(
                  Diagnostic.Kind.ERROR, "[EPILOGUE] Logged methods cannot be static", method);

          valid = false;
        }

        if (method.getReturnType().getKind() == TypeKind.NONE) {
          processingEnv
              .getMessager()
              .printMessage(
                  Diagnostic.Kind.ERROR, "[EPILOGUE] Logged methods cannot be void", method);

          valid = false;
        }

        if (!method.getParameters().isEmpty()) {
          processingEnv
              .getMessager()
              .printMessage(
                  Diagnostic.Kind.ERROR,
                  "[EPILOGUE] Logged methods cannot accept arguments",
                  method);

          valid = false;
        }
      }
    }
    return valid;
  }

  /**
   * Checks if a type is not loggable.
   *
   * @param type the type to check
   */
  private boolean isNotLoggable(Element element, TypeMirror type) {
    if (type instanceof NoType) {
      // e.g. void, cannot log
      return true;
    }

    boolean loggable = m_handlers.stream().anyMatch(h -> h.isLoggable(element));

    if (loggable) {
      return false;
    }

    processingEnv
        .getMessager()
        .printMessage(
            Diagnostic.Kind.NOTE,
            "[EPILOGUE] Excluded from logs because " + type + " is not a loggable data type",
            element);
    return true;
  }

  @SuppressWarnings("unchecked")
  private Map<DeclaredType, DeclaredType> processCustomLoggers(
      RoundEnvironment roundEnv, TypeElement customLoggerAnnotation) {
    // map logged type to its custom logger, eg
    // { Point.class => CustomPointLogger.class }
    var customLoggers = new HashMap<DeclaredType, DeclaredType>();

    var annotatedElements = roundEnv.getElementsAnnotatedWith(customLoggerAnnotation);

    var loggerSuperClass =
        processingEnv
            .getElementUtils()
            .getTypeElement("edu.wpi.first.epilogue.logging.ClassSpecificLogger");

    for (Element annotatedElement : annotatedElements) {
      List<AnnotationValue> targetTypes = List.of();
      for (AnnotationMirror annotationMirror : annotatedElement.getAnnotationMirrors()) {
        for (var entry : annotationMirror.getElementValues().entrySet()) {
          if ("value".equals(entry.getKey().getSimpleName().toString())) {
            targetTypes = (List<AnnotationValue>) entry.getValue().getValue();
          }
        }
      }

      boolean hasPublicNoArgConstructor =
          annotatedElement.getEnclosedElements().stream()
              .anyMatch(
                  enclosedElement ->
                      enclosedElement instanceof ExecutableElement exe
                          && exe.getKind() == ElementKind.CONSTRUCTOR
                          && exe.getModifiers().contains(Modifier.PUBLIC)
                          && exe.getParameters().isEmpty());

      if (!hasPublicNoArgConstructor) {
        processingEnv
            .getMessager()
            .printMessage(
                Diagnostic.Kind.ERROR,
                "[EPILOGUE] Logger classes must have a public no-argument constructor",
                annotatedElement);
        continue;
      }

      for (AnnotationValue value : targetTypes) {
        var targetType = (DeclaredType) value.getValue();
        var reflectedTarget = targetType.asElement();

        // eg ClassSpecificLogger<MyDataType>
        var requiredSuperClass =
            processingEnv
                .getTypeUtils()
                .getDeclaredType(
                    loggerSuperClass,
                    processingEnv.getTypeUtils().getWildcardType(null, reflectedTarget.asType()));

        if (customLoggers.containsKey(targetType)) {
          processingEnv
              .getMessager()
              .printMessage(
                  Diagnostic.Kind.ERROR,
                  "[EPILOGUE] Multiple custom loggers detected for type " + targetType,
                  annotatedElement);
          continue;
        }

        if (annotatedElement instanceof TypeElement t && !t.getTypeParameters().isEmpty()) {
          processingEnv
              .getMessager()
              .printMessage(
                  Diagnostic.Kind.ERROR,
                  "[EPILOGUE] Custom logger classes cannot take generic type arguments",
                  annotatedElement);
          continue;
        }

        if (!processingEnv
            .getTypeUtils()
            .isAssignable(annotatedElement.asType(), requiredSuperClass)) {
          processingEnv
              .getMessager()
              .printMessage(
                  Diagnostic.Kind.ERROR,
                  "[EPILOGUE] Not a subclass of ClassSpecificLogger<" + targetType + ">",
                  annotatedElement);
          continue;
        }

        customLoggers.put(targetType, (DeclaredType) annotatedElement.asType());
      }
    }

    return customLoggers;
  }

  private void processEpilogue(
      RoundEnvironment roundEnv, TypeElement epilogueAnnotation, Set<TypeElement> loggedTypes) {
    var annotatedElements = roundEnv.getElementsAnnotatedWith(epilogueAnnotation);

    List<String> loggerClassNames = new ArrayList<>();
    var mainRobotClasses = new ArrayList<TypeElement>();

    // Used to check for a main robot class
    var robotBaseClass =
        processingEnv.getElementUtils().getTypeElement("edu.wpi.first.wpilibj.TimedRobot").asType();

    boolean validFields = validateFields(annotatedElements);
    boolean validMethods = validateMethods(annotatedElements);

    if (!(validFields && validMethods)) {
      // Generate nothing and bail
      return;
    }

    for (TypeElement clazz : loggedTypes) {
      try {
        warnOfNonLoggableElements(clazz);
        m_loggerGenerator.writeLoggerFile(clazz);

        if (processingEnv.getTypeUtils().isAssignable(clazz.getSuperclass(), robotBaseClass)) {
          mainRobotClasses.add(clazz);
        }

        loggerClassNames.add(StringUtils.loggerClassName(clazz));
      } catch (IOException e) {
        processingEnv
            .getMessager()
            .printMessage(
                Diagnostic.Kind.ERROR,
                "[EPILOGUE] Could not write logger file for " + clazz.getQualifiedName(),
                clazz);
        e.printStackTrace(System.err);
      }
    }

    // Sort alphabetically
    mainRobotClasses.sort(Comparator.comparing(c -> c.getSimpleName().toString()));
    m_epiloguerGenerator.writeEpilogueFile(loggerClassNames, mainRobotClasses);
  }

  private void warnOfNonLoggableElements(TypeElement clazz) {
    var config = clazz.getAnnotation(Logged.class);
    if (config == null || config.strategy() == Logged.Strategy.OPT_IN) {
      // field and method validations will have already checked everything
      return;
    }

    for (Element element : clazz.getEnclosedElements()) {
      if (element.getAnnotation(NotLogged.class) != null) {
        // Explicitly opted out from, don't need to check
        continue;
      }

      if (element.getModifiers().contains(Modifier.STATIC)) {
        // static elements are never logged
        continue;
      }

      if (element instanceof VariableElement v) {
        // isNotLoggable will internally print a warning message
        isNotLoggable(v, v.asType());
      }

      if (element instanceof ExecutableElement exe
          && exe.getModifiers().contains(Modifier.PUBLIC)
          && exe.getParameters().isEmpty()) {
        // isNotLoggable will internally print a warning message
        isNotLoggable(exe, exe.getReturnType());
      }
    }
  }
}
