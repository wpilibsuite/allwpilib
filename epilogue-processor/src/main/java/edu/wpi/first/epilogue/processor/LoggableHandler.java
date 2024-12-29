// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.processor;

import edu.wpi.first.epilogue.Logged;
import java.util.Collection;
import java.util.Comparator;
import java.util.Set;
import java.util.stream.Collectors;
import javax.annotation.processing.ProcessingEnvironment;
import javax.lang.model.element.Element;
import javax.lang.model.element.ElementKind;
import javax.lang.model.element.ExecutableElement;
import javax.lang.model.element.TypeElement;
import javax.lang.model.element.VariableElement;
import javax.lang.model.type.TypeKind;
import javax.lang.model.type.TypeMirror;

/** Handles logging for types annotated with the {@link Logged @Logged} annotation. */
public class LoggableHandler extends ElementHandler {
  private final Set<TypeElement> m_loggedTypes;

  protected LoggableHandler(
      ProcessingEnvironment processingEnv, Collection<? extends Element> loggedTypes) {
    super(processingEnv);
    m_loggedTypes =
        loggedTypes.stream()
            .filter(e -> e instanceof TypeElement)
            .map(e -> (TypeElement) e)
            .collect(Collectors.toSet());
  }

  @Override
  public boolean isLoggable(Element element) {
    return m_processingEnv.getTypeUtils().asElement(dataType(element)) instanceof TypeElement t
        && m_loggedTypes.contains(t);
  }

  @Override
  public String logInvocation(Element element) {
    TypeMirror dataType = dataType(element);
    var declaredType =
        m_processingEnv
            .getElementUtils()
            .getTypeElement(m_processingEnv.getTypeUtils().erasure(dataType).toString());

    // Get the list of known loggable subtypes of the input type. This will include the input type.
    // These are sorted by their distance from the declared type such that "more concrete" types are
    // checked first so the instanceof chain doesn't check a really generic type first, even if a
    // more specific loggable type could be used instead.
    var loggableSubtypes =
        m_loggedTypes.stream()
            .filter(
                l -> m_processingEnv.getTypeUtils().isAssignable(l.asType(), declaredType.asType()))
            .sorted(inheritanceComparatorFor(declaredType))
            .toList();

    int size = loggableSubtypes.size();

    // If there are no known loggable subtypes, return just the single logger call
    if (size == 1) {
      return generateLoggerCall(element, declaredType, elementAccess(element));
    }

    // Otherwise, generate an if-else chain to compare the element with its known loggable subtypes
    // and implementations. A subclass without a @Logged annotation will be logged at runtime using
    // the generic logger for whatever the field or method's declared type is.

    String varName = cacheVariableName(element);

    StringBuilder builder = new StringBuilder();

    // Cache the value in a variable so it's only read once
    builder.append("var %s = %s;\n".formatted(varName, elementAccess(element)));

    for (int i = 0; i < size; i++) {
      TypeElement type = loggableSubtypes.get(i);
      String part;

      if (i == 0) {
        // First invocation, generate an "if" statement
        part = generateIf(type, element, "if", varName);
      } else if (i == size - 1) {
        // Final invocation, generate an "else" statement
        String loggerCall = generateLoggerCall(element, type, varName);
        part =
            " else {\n  // Base type %s\n  %s;\n}"
                .formatted(declaredType.getQualifiedName(), loggerCall);
      } else {
        // Somewhere in the middle, generate an "else if" statement
        part = generateIf(type, element, " else if", varName);
      }

      builder.append(part);
    }

    return builder.toString();
  }

  /**
   * Generates the name of the cache variable to use for a logged element.
   *
   * @param element the logged element
   * @return the cache variable name
   */
  private static String cacheVariableName(Element element) {
    // Generate unique names in case a field and a method share the same name
    if (element instanceof VariableElement) {
      return "$$%s".formatted(element.getSimpleName().toString());
    } else if (element instanceof ExecutableElement) {
      return "__%s".formatted(element.getSimpleName().toString());
    } else {
      // Generic fallback (shouldn't get here, since only fields and methods are logged)
      return element.getSimpleName().toString();
    }
  }

  /**
   * Creates a comparator for sorting inheritors of a given type by their distance (farthest first)
   * for use in generating if-else instanceof chains. Inheritors at the same distance from the base
   * type will be further compared so classes come before interfaces, any any further ties are
   * broken alphabetically by fully-qualified type names.
   *
   * @param declaredType the declared type
   * @return the comparator
   */
  private Comparator<TypeElement> inheritanceComparatorFor(TypeElement declaredType) {
    Comparator<TypeElement> byDistance =
        Comparator.comparingInt(
            inheritor -> {
              return inheritanceDistance(inheritor.asType(), declaredType.asType());
            });

    return byDistance
        .reversed()
        .thenComparing(type -> type.getKind() == ElementKind.INTERFACE ? 1 : 0)
        .thenComparing(type -> type.getQualifiedName().toString());
  }

  /**
   * Generates an instanceof if or if-else statement that checks the type and logs the element using
   * the logger for the given type, if they're compatible.
   *
   * @param type the type to generate the check for
   * @param element the element to be logged
   * @param keyword either "if" or " else if"
   * @param varName the name of the variable in the "instanceof" check
   * @return the if or else-if statement
   */
  private String generateIf(TypeElement type, Element element, String keyword, String varName) {
    String ref = type.getQualifiedName().toString().replace('.', '_');
    String loggerCall = generateLoggerCall(element, type, ref);

    return "%s (%s instanceof %s %s) {\n  %s;\n}"
        .formatted(keyword, varName, type.getQualifiedName(), ref, loggerCall);
  }

  private String generateLoggerCall(Element element, TypeElement type, String elementReference) {
    return ("Epilogue.%s.tryUpdate(backend.getNested(\"%s\"), %s, "
            + "Epilogue.getConfig().errorHandler)")
        .formatted(StringUtils.loggerFieldName(type), loggedName(element), elementReference);
  }

  /**
   * Computes the minimum inheritance distance between two types; that is, how many "extends" or
   * "implements" clauses are required to get from one to the other.
   *
   * @param toCheck the type to check
   * @param base the base type to check against
   * @return the inheritance distance
   */
  private int inheritanceDistance(TypeMirror toCheck, TypeMirror base) {
    var types = m_processingEnv.getTypeUtils();

    if (types.isSameType(toCheck, base)) {
      return 0;
    }

    int distance = 1;
    var parent = toCheck;
    TypeElement element = m_processingEnv.getElementUtils().getTypeElement(parent.toString());
    while (!types.isSameType(parent, base)
        && element.getInterfaces().stream().noneMatch(i -> types.isSameType(i, base))) {
      element = m_processingEnv.getElementUtils().getTypeElement(parent.toString());
      if (parent.getKind() == TypeKind.NONE) {
        // Interface inheritance, there is no superclass
        break;
      }

      parent = element.getSuperclass();

      // Handle cases of interface inheritance
      distance =
          1
              + element.getInterfaces().stream()
                  .mapToInt(iface -> inheritanceDistance(iface, base))
                  .min()
                  .orElse(distance);
    }

    return distance;
  }
}
