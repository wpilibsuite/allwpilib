// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.epilogue.processor;

import static java.util.stream.Collectors.groupingBy;
import static java.util.stream.Collectors.toList;

import com.sun.source.tree.IdentifierTree;
import com.sun.source.tree.ReturnTree;
import com.sun.source.util.SimpleTreeVisitor;
import com.sun.source.util.Trees;
import org.wpilib.epilogue.Logged;
import org.wpilib.epilogue.NotLogged;
import java.io.IOException;
import java.io.PrintWriter;
import java.lang.annotation.Annotation;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.Deque;
import java.util.EnumMap;
import java.util.HashMap;
import java.util.LinkedHashMap;
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

        @Override
        public boolean warnForNonLoggableTypes() {
          return false;
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
   * class will subclass from {@link org.wpilib.epilogue.logging.ClassSpecificLogger} and
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

    List<VariableElement> fieldsToLog = new ArrayList<>();
    List<ExecutableElement> methodsToLog = new ArrayList<>();
    collectLoggables(clazz, fieldsToLog, methodsToLog);

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

    var varHandleFields =
        loggableFields.stream()
            .filter(
                e -> {
                  if (e.getEnclosingElement().equals(clazz)) {
                    // The generated logger is in the same package as the logged class, so the
                    // only fields it can't read are private ones.
                    return e.getModifiers().contains(Modifier.PRIVATE);
                  } else {
                    // Logging from a superclass. Can only read public fields, unless the superclass
                    // is in the same package, in which case protected and package-private fields
                    // are also readable.
                    return !e.getModifiers().contains(Modifier.PUBLIC);
                  }
                })
            .toList();
    boolean requiresVarHandles = !varHandleFields.isEmpty();

    try (var out = new PrintWriter(loggerFile.openWriter())) {
      if (packageName != null) {
        // package com.example;
        out.println("package " + packageName + ";");
        out.println();
      }

      out.println("import org.wpilib.epilogue.Logged;");
      out.println("import org.wpilib.epilogue.Epilogue;");
      out.println("import org.wpilib.epilogue.logging.ClassSpecificLogger;");
      out.println("import org.wpilib.epilogue.logging.EpilogueBackend;");
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
        for (var privateField : varHandleFields) {
          // This field needs a VarHandle to access.
          // Cache it in the class to avoid lookups
          out.printf(
              "  // Accesses private or superclass field %s.%s%n",
              privateField.getEnclosingElement(), privateField.getSimpleName());
          out.printf("  private static final VarHandle %s;%n", varHandleName(privateField));
        }
        out.println();
      }

      // Static initializer block to load VarHandles and reflection fields
      if (requiresVarHandles) {
        out.println("  static {");

        out.println("    try {");

        out.println("      var rootLookup = MethodHandles.lookup();");

        // Group private fields by class, then generate a private lookup for each class
        // and a VarHandle for each field using that lookup. Sorting and then collecting into a
        // LinkedHashMap gives deterministic output ordering (mostly for tests, which check exact
        // file contents, but also results in less churn when regenerating files for users who like
        // to read the generated logger classes).
        //
        // This lets us read private fields from superclasses.
        Map<Element, List<VariableElement>> privateFieldsByClass =
            varHandleFields.stream()
                .sorted(Comparator.comparing(e -> e.getSimpleName().toString()))
                .collect(
                    Collectors.groupingBy(
                        VariableElement::getEnclosingElement,
                        LinkedHashMap::new,
                        Collectors.toList()));

        privateFieldsByClass.forEach(
            (enclosingClass, fields) -> {
              String className = enclosingClass.toString();
              String lookupName = "lookup$$" + className.replace(".", "_");
              out.printf(
                  "      var %s = MethodHandles.privateLookupIn(%s.class, rootLookup);%n",
                  lookupName, className);

              for (var field : fields) {
                var fieldname = field.getSimpleName();
                out.printf(
                    "      %s = %s.findVarHandle(%s.class, \"%s\", %s.class);%n",
                    varHandleName(field),
                    lookupName,
                    className,
                    fieldname,
                    m_processingEnv.getTypeUtils().erasure(field.asType()));
              }
            });

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
                    var logInvocation = h.logInvocation(loggableElement, clazz);
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

  /**
   * Generates the name of a VarHandle for access to the given field. The VarHandle variable's name
   * is guaranteed to be unique.
   *
   * @param field The field to generate a VarHandle for
   * @return The name of the generated VarHandle variable
   */
  public static String varHandleName(VariableElement field) {
    return "$%s_%s"
        .formatted(field.getEnclosingElement().toString().replace(".", "_"), field.getSimpleName());
  }

  private void collectLoggables(
      TypeElement clazz, List<VariableElement> fields, List<ExecutableElement> methods) {
    var config = clazz.getAnnotation(Logged.class);
    if (config == null) {
      config = m_defaultConfig;
    }
    boolean requireExplicitOptIn = config.strategy() == Logged.Strategy.OPT_IN;

    Predicate<Element> notSkipped = LoggerGenerator::isNotSkipped;
    Predicate<Element> optedIn =
        e -> !requireExplicitOptIn || e.getAnnotation(Logged.class) != null;

    List<VariableElement> classFields;
    if (Objects.equals(clazz.getSuperclass().toString(), "java.lang.Record")) {
      // Do not log record members - just use the accessor methods
      classFields = List.of();
    } else {
      classFields =
          clazz.getEnclosedElements().stream()
              .filter(e -> e instanceof VariableElement)
              .map(e -> (VariableElement) e)
              .filter(notSkipped)
              .filter(optedIn)
              .filter(e -> !e.getModifiers().contains(Modifier.STATIC))
              .filter(this::isLoggable)
              .toList();
    }
    fields.addAll(classFields);

    methods.addAll(
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
            .filter(e -> !isSimpleGetterMethodForLoggedField(e, classFields))
            .toList());

    TypeElement superclass =
        (TypeElement) m_processingEnv.getTypeUtils().asElement(clazz.getSuperclass());
    if (superclass != null) {
      collectLoggables(superclass, fields, methods);
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
