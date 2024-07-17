// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.processor;

import static java.util.stream.Collectors.groupingBy;
import static java.util.stream.Collectors.toList;

import edu.wpi.first.epilogue.Logged;
import edu.wpi.first.epilogue.NotLogged;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.EnumMap;
import java.util.List;
import java.util.function.Predicate;
import java.util.stream.Stream;
import javax.annotation.processing.ProcessingEnvironment;
import javax.lang.model.element.Element;
import javax.lang.model.element.ExecutableElement;
import javax.lang.model.element.Modifier;
import javax.lang.model.element.TypeElement;
import javax.lang.model.element.VariableElement;

/** Generates logger class files for {@link Logged @Logged}-annotated classes. */
public class LoggerGenerator {
  private final ProcessingEnvironment m_processingEnv;
  private final List<ElementHandler> m_handlers;

  public LoggerGenerator(ProcessingEnvironment processingEnv, List<ElementHandler> handlers) {
    this.m_processingEnv = processingEnv;
    this.m_handlers = handlers;
  }

  private static boolean isNotSkipped(Element e) {
    return e.getAnnotation(NotLogged.class) == null;
  }

  /**
   * Generates the logger class used to handle data objects of the given type. The generated logger
   * class will subclass from {@link edu.wpi.first.epilogue.logging.ClassSpecificLogger} and
   * implement the {@code update()} method to populate a data log with information from an instance
   * of the data type.
   *
   * @param clazz the data type that the logger should support.
   * @throws IOException if the file could not be written
   */
  public void writeLoggerFile(TypeElement clazz) throws IOException {
    var config = clazz.getAnnotation(Logged.class);
    boolean requireExplicitOptIn = config.strategy() == Logged.Strategy.OPT_IN;

    Predicate<Element> notSkipped = LoggerGenerator::isNotSkipped;
    Predicate<Element> optedIn =
        e -> !requireExplicitOptIn || e.getAnnotation(Logged.class) != null;

    var fieldsToLog =
        clazz.getEnclosedElements().stream()
            .filter(e -> e instanceof VariableElement)
            .map(e -> (VariableElement) e)
            .filter(notSkipped)
            .filter(optedIn)
            .filter(e -> !e.getModifiers().contains(Modifier.STATIC))
            .filter(this::isLoggable)
            .toList();

    var methodsToLog =
        clazz.getEnclosedElements().stream()
            .filter(e -> e instanceof ExecutableElement)
            .map(e -> (ExecutableElement) e)
            .filter(notSkipped)
            .filter(optedIn)
            .filter(e -> !e.getModifiers().contains(Modifier.STATIC))
            .filter(e -> e.getModifiers().contains(Modifier.PUBLIC))
            .filter(e -> e.getParameters().isEmpty())
            .filter(e -> e.getReceiverType() != null)
            .filter(this::isLoggable)
            .toList();

    writeLoggerFile(clazz.getQualifiedName().toString(), config, fieldsToLog, methodsToLog);
  }

  private void writeLoggerFile(
      String className,
      Logged classConfig,
      List<VariableElement> loggableFields,
      List<ExecutableElement> loggableMethods)
      throws IOException {
    String packageName = null;
    int lastDot = className.lastIndexOf('.');
    if (lastDot > 0) {
      packageName = className.substring(0, lastDot);
    }

    String simpleClassName = StringUtils.simpleName(className);
    String loggerClassName = className + "Logger";
    String loggerSimpleClassName = loggerClassName.substring(lastDot + 1);

    // Use the name on the class config to set the generated logger names
    // This helps to avoid naming conflicts
    if (!classConfig.name().isBlank()) {
      loggerSimpleClassName =
          StringUtils.capitalize(classConfig.name().replaceAll(" ", "")) + "Logger";
      if (lastDot > 0) {
        loggerClassName = packageName + "." + loggerSimpleClassName;
      } else {
        loggerClassName = loggerSimpleClassName;
      }
    }

    var loggerFile = m_processingEnv.getFiler().createSourceFile(loggerClassName);

    var privateFields =
        loggableFields.stream().filter(e -> e.getModifiers().contains(Modifier.PRIVATE)).toList();
    boolean requiresVarHandles = !privateFields.isEmpty();

    try (var out = new PrintWriter(loggerFile.openWriter())) {
      if (packageName != null) {
        // package com.example;
        out.println("package " + packageName + ";");
        out.println();
      }

      out.println("import edu.wpi.first.epilogue.Logged;");
      out.println("import edu.wpi.first.epilogue.Epilogue;");
      out.println("import edu.wpi.first.epilogue.logging.ClassSpecificLogger;");
      out.println("import edu.wpi.first.epilogue.logging.DataLogger;");
      if (requiresVarHandles) {
        out.println("import java.lang.invoke.MethodHandles;");
        out.println("import java.lang.invoke.VarHandle;");
      }
      out.println();

      // public class FooLogger implements ClassSpecificLogger<Foo> {
      out.println(
          "public class "
              + loggerSimpleClassName
              + " extends ClassSpecificLogger<"
              + simpleClassName
              + "> {");

      if (requiresVarHandles) {
        for (var privateField : privateFields) {
          // This field needs a VarHandle to access.
          // Cache it in the class to avoid lookups
          out.println("  private static final VarHandle $" + privateField.getSimpleName() + ";");
        }
        out.println();

        var clazz = simpleClassName + ".class";

        out.println("  static {");
        out.println("    try {");
        out.println(
            "      var lookup = MethodHandles.privateLookupIn("
                + clazz
                + ", MethodHandles.lookup());");

        for (var privateField : privateFields) {
          var fieldName = privateField.getSimpleName();
          out.println(
              "      $"
                  + fieldName
                  + " = lookup.findVarHandle("
                  + clazz
                  + ", \""
                  + fieldName
                  + "\", "
                  + m_processingEnv.getTypeUtils().erasure(privateField.asType())
                  + ".class);");
        }

        out.println("    } catch (ReflectiveOperationException e) {");
        out.println(
            "      throw new RuntimeException("
                + "\"[EPILOGUE] Could not load private fields for logging!\", e);");
        out.println("    }");
        out.println("  }");
        out.println();
      }

      out.println("  public " + loggerSimpleClassName + "() {");
      out.println("    super(" + simpleClassName + ".class);");
      out.println("  }");
      out.println();

      // @Override
      // public void update(DataLogger dataLogger, Foo object) {
      out.println("  @Override");
      out.println("  public void update(DataLogger dataLogger, " + simpleClassName + " object) {");

      // Build a map of importance levels to the fields logged at those levels
      // e.g. { DEBUG: [fieldA, fieldB], INFO: [fieldC], CRITICAL: [fieldD, fieldE, fieldF] }
      var loggedElementsByImportance =
          Stream.concat(loggableFields.stream(), loggableMethods.stream())
              .collect(
                  groupingBy(
                      element -> {
                        var config = element.getAnnotation(Logged.class);
                        if (config == null) {
                          // No configuration on this element, fall back to the class-level
                          // configuration
                          return classConfig.importance();
                        } else {
                          return config.importance();
                        }
                      },
                      () ->
                          new EnumMap<>(Logged.Importance.class), // EnumMap for consistent ordering
                      toList()));

      loggedElementsByImportance.forEach(
          (importance, elements) -> {
            out.println(
                "    if (Epilogue.shouldLog(Logged.Importance." + importance.name() + ")) {");

            for (var loggableElement : elements) {
              // findFirst for prioritization
              var handler =
                  m_handlers.stream().filter(h -> h.isLoggable(loggableElement)).findFirst();

              handler.ifPresent(
                  h -> {
                    // May be null if the handler consumes the element but does not actually want it
                    // to be logged. For example, the sendable handler consumes all sendable types
                    // but does not log commands or subsystems, to prevent excessive warnings about
                    // unloggable commands.
                    var logInvocation = h.logInvocation(loggableElement);
                    if (logInvocation != null) {
                      out.println(logInvocation.indent(6).stripTrailing() + ";");
                    }
                  });
            }

            out.println("    }");
          });

      out.println("  }");
      out.println("}");
    }
  }

  private boolean isLoggable(Element element) {
    return m_handlers.stream().anyMatch(h -> h.isLoggable(element));
  }
}
