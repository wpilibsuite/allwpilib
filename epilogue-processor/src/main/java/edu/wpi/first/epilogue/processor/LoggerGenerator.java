// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.processor;

import static java.util.stream.Collectors.groupingBy;
import static java.util.stream.Collectors.toList;

import com.sun.source.tree.IdentifierTree;
import com.sun.source.tree.ReturnTree;
import com.sun.source.util.SimpleTreeVisitor;
import com.sun.source.util.Trees;
import edu.wpi.first.epilogue.Logged;
import edu.wpi.first.epilogue.NotLogged;
import java.io.IOException;
import java.io.PrintWriter;
import java.lang.annotation.Annotation;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Deque;
import java.util.EnumMap;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.function.Predicate;
import java.util.stream.Collectors;
import java.util.stream.Stream;
import javax.annotation.processing.ProcessingEnvironment;
import javax.lang.model.element.Element;
import javax.lang.model.element.ExecutableElement;
import javax.lang.model.element.Modifier;
import javax.lang.model.element.Name;
import javax.lang.model.element.PackageElement;
import javax.lang.model.element.TypeElement;
import javax.lang.model.element.VariableElement;
import javax.tools.Diagnostic;

/** Generates logger class files for {@link Logged @Logged}-annotated classes. */
public class LoggerGenerator {
  public static final Predicate<ExecutableElement> kIsBuiltInJavaMethod =
      LoggerGenerator::isBuiltInJavaMethod;
  private final ProcessingEnvironment m_processingEnv;
  private final List<ElementHandler> m_handlers;

  @SuppressWarnings("BadAnnotationImplementation")
  private final Logged m_defaultConfig =
      new Logged() {
        @Override
        public Class<? extends Annotation> annotationType() {
          return Logged.class;
        }

        @Override
        public String name() {
          return "";
        }

        @Override
        public Strategy strategy() {
          return Strategy.OPT_IN;
        }

        @Override
        public Importance importance() {
          return Importance.DEBUG;
        }

        @Override
        public Naming defaultNaming() {
          return Naming.USE_CODE_NAME;
        }
      };

  public LoggerGenerator(ProcessingEnvironment processingEnv, List<ElementHandler> handlers) {
    this.m_processingEnv = processingEnv;
    this.m_handlers = handlers;
  }

  private static boolean isNotSkipped(Element e) {
    return e.getAnnotation(NotLogged.class) == null;
  }

  /**
   * Checks if a method is a method declared in java.lang.Object that should not be logged.
   *
   * @param e the method to check
   * @return true if the method is toString, hashCode, or clone; false otherwise
   */
  private static boolean isBuiltInJavaMethod(ExecutableElement e) {
    Name name = e.getSimpleName();
    return name.contentEquals("toString")
        || name.contentEquals("hashCode")
        || name.contentEquals("clone");
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
    if (config == null) {
      config = m_defaultConfig;
    }
    boolean requireExplicitOptIn = config.strategy() == Logged.Strategy.OPT_IN;

    Predicate<Element> notSkipped = LoggerGenerator::isNotSkipped;
    Predicate<Element> optedIn =
        e -> !requireExplicitOptIn || e.getAnnotation(Logged.class) != null;

    List<VariableElement> fieldsToLog;
    if (Objects.equals(clazz.getSuperclass().toString(), "java.lang.Record")) {
      // Do not log record members - just use the accessor methods
      fieldsToLog = List.of();
    } else {
      fieldsToLog =
          clazz.getEnclosedElements().stream()
              .filter(e -> e instanceof VariableElement)
              .map(e -> (VariableElement) e)
              .filter(notSkipped)
              .filter(optedIn)
              .filter(e -> !e.getModifiers().contains(Modifier.STATIC))
              .filter(this::isLoggable)
              .toList();
    }

    List<ExecutableElement> methodsToLog =
        clazz.getEnclosedElements().stream()
            .filter(e -> e instanceof ExecutableElement)
            .map(e -> (ExecutableElement) e)
            .filter(notSkipped)
            .filter(optedIn)
            .filter(e -> !e.getModifiers().contains(Modifier.STATIC))
            .filter(e -> e.getModifiers().contains(Modifier.PUBLIC))
            .filter(e -> e.getParameters().isEmpty())
            .filter(e -> e.getReceiverType() != null)
            .filter(kIsBuiltInJavaMethod.negate())
            .filter(this::isLoggable)
            .filter(e -> !isSimpleGetterMethodForLoggedField(e, fieldsToLog))
            .toList();

    // Validate no name collisions
    Map<String, List<Element>> usedNames =
        Stream.concat(fieldsToLog.stream(), methodsToLog.stream())
            .reduce(
                new HashMap<>(),
                (map, element) -> {
                  String name = ElementHandler.loggedName(element);
                  map.computeIfAbsent(name, _k -> new ArrayList<>()).add(element);

                  return map;
                },
                (left, right) -> {
                  left.putAll(right);
                  return left;
                });

    usedNames.forEach(
        (name, elements) -> {
          if (elements.size() > 1) {
            // Collisions!
            for (Element conflictingElement : elements) {
              String conflicts =
                  elements.stream()
                      .filter(e -> !e.equals(conflictingElement))
                      .map(e -> e.getEnclosingElement().getSimpleName() + "." + e)
                      .collect(Collectors.joining(", "));

              m_processingEnv
                  .getMessager()
                  .printMessage(
                      Diagnostic.Kind.ERROR,
                      "[EPILOGUE] Conflicting name detected: \""
                          + name
                          + "\" is also used by "
                          + conflicts,
                      conflictingElement);
            }
          }
        });

    writeLoggerFile(clazz, config, fieldsToLog, methodsToLog);
  }

  private void writeLoggerFile(
      TypeElement clazz,
      Logged classConfig,
      List<VariableElement> loggableFields,
      List<ExecutableElement> loggableMethods)
      throws IOException {
    // Walk nesting levels, to support inner classes
    Deque<String> nesting = new ArrayDeque<>();
    Element enclosing = clazz.getEnclosingElement();
    while (!(enclosing instanceof PackageElement p)) {
      nesting.addFirst(enclosing.getSimpleName().toString());
      enclosing = enclosing.getEnclosingElement();
    }
    String packageName = p.getQualifiedName().toString();
    nesting.addLast(clazz.getSimpleName().toString());
    String simpleClassName = String.join(".", nesting);

    String loggerClassName = StringUtils.loggerClassName(clazz);
    String loggerSimpleClassName = StringUtils.simpleName(loggerClassName);

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
      out.println("import edu.wpi.first.epilogue.logging.EpilogueBackend;");
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

        var classReference = simpleClassName + ".class";

        out.println("  static {");
        out.println("    try {");
        out.println(
            "      var lookup = MethodHandles.privateLookupIn("
                + classReference
                + ", MethodHandles.lookup());");

        for (var privateField : privateFields) {
          var fieldName = privateField.getSimpleName();
          out.println(
              "      $"
                  + fieldName
                  + " = lookup.findVarHandle("
                  + classReference
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
      // public void update(EpilogueBackend backend, Foo object) {
      out.println("  @Override");
      out.println(
          "  public void update(EpilogueBackend backend, " + simpleClassName + " object) {");

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

  /**
   * Checks if a method is a simple "getter" method for a field in the given list. Here, we define
   * "getter" as a method with a single return statement that references the name of a field, with
   * no other expressions. `getX() { return x; }` would be considered a "getter" method, while
   * `getX() { return x.clone(); }` would not be. Note that the method name is irrelevant; only the
   * method body is checked.
   *
   * @param ex the method to check
   * @param fieldsToLog the fields that will already be logged
   * @return true if the method is a simple "getter" method, false otherwise
   */
  private boolean isSimpleGetterMethodForLoggedField(
      ExecutableElement ex, List<VariableElement> fieldsToLog) {
    var trees = Trees.instance(m_processingEnv);
    var methodTree = trees.getTree(ex);

    if (methodTree == null) {
      // probably a record's synthetic reader method
      return false;
    }

    if (methodTree.getBody() == null) {
      // Abstract or native method, can't be determined to be a getter
      return false;
    }

    var statements = methodTree.getBody().getStatements();
    if (statements.size() != 1) {
      // More complex than a simple `return m_field` statement
      return false;
    }

    var statement = statements.get(0);
    if (!(statement instanceof ReturnTree ret)) {
      // Shouldn't get here, since we've already filtered for methods that return a value
      // and with a single statement - that one statement should be the return
      return false;
    }

    var returnExpression = ret.getExpression();
    return returnExpression.accept(
        new SimpleTreeVisitor<Boolean, Void>(false) {
          @Override
          public Boolean visitIdentifier(IdentifierTree identifier, Void unused) {
            return fieldsToLog.stream()
                .anyMatch(v -> v.getSimpleName().contentEquals(identifier.getName()));
          }
        },
        null);
  }
}
