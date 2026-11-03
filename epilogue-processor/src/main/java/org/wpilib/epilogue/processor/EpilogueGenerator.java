// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.epilogue.processor;

import java.io.IOException;
import java.io.PrintWriter;
import java.nio.charset.StandardCharsets;
import java.util.List;
import java.util.Map;
import javax.annotation.processing.ProcessingEnvironment;
import javax.lang.model.type.DeclaredType;
import javax.lang.model.type.TypeMirror;

/**
 * Generates the {@code EpilogueLoggers} file containing static instances of every generated logger
 * and custom logger class.
 */
public class EpilogueGenerator {
  private final ProcessingEnvironment m_processingEnv;
  private final Map<TypeMirror, DeclaredType> m_customLoggers;

  public EpilogueGenerator(
      ProcessingEnvironment processingEnv, Map<TypeMirror, DeclaredType> customLoggers) {
    this.m_processingEnv = processingEnv;
    this.m_customLoggers = customLoggers;
  }

  /**
   * Creates the EpilogueLoggers file containing static instances of generated loggers.
   *
   * @param loggerClassNames the names of the generated logger classes. Each of these will be
   *     instantiated in a public static field on the EpilogueLoggers class.
   */
  @SuppressWarnings("checkstyle:LineLength") // Source code templates exceed the line length limit
  public void writeEpilogueFile(List<String> loggerClassNames) {
    try {
      var centralStore =
          m_processingEnv
              .getFiler()
              .createSourceFile("org.wpilib.epilogue.generated.EpilogueLoggers");

      try (var out =
          new PrintWriter(centralStore.openOutputStream(), false, StandardCharsets.UTF_8)) {
        out.println("package org.wpilib.epilogue.generated;");
        out.println();

        out.println("public final class EpilogueLoggers {");

        loggerClassNames.forEach(
            clazz -> {
              // public static final com.example.FooLogger com_example_fooLogger =
              //   new com.example.FooLogger();
              String field = clazz.replace('.', '_');
              out.printf("  public static final %s %s = new %s();%n", clazz, field, clazz);
            });
        m_customLoggers.values().stream()
            .distinct()
            .forEach(
                loggerType -> {
                  var loggerTypeName = loggerType.toString();
                  out.printf(
                      "  public static final %s %s = new %s();%n",
                      loggerTypeName, loggerTypeName.replace('.', '_'), loggerTypeName);
                });

        out.println("}");
      }
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
  }
}
