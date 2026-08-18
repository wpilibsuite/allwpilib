// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.javacplugin;

import static com.google.testing.compile.CompilationSubject.assertThat;
import static com.google.testing.compile.Compiler.javac;
import static org.junit.jupiter.api.Assertions.assertEquals;

import com.google.testing.compile.JavaFileObjects;
import org.junit.jupiter.api.Test;

class IntegerDivisionDetectorTest {
  private static final String[] types = {"byte", "char", "short", "int", "long", "float", "double"};

  private static boolean isInteger(int t) {
    return t <= 4;
  }

  private static boolean isFloat(int t) {
    return t >= 5;
  }

  private static int promote(int t1, int t2) {
    if (t1 == 6 || t2 == 6) {
      return 6;
    }
    if (t1 == 5 || t2 == 5) {
      return 5;
    }
    if (t1 == 4 || t2 == 4) {
      return 4;
    }
    return 3; // int
  }

  private static boolean canAssign(int from, int to) {
    if (from == to) {
      return true;
    }
    // byte -> short, int, long, float, double
    if (from == 0) {
      return to == 2 || to == 3 || to == 4 || to == 5 || to == 6;
    }
    // char -> int, long, float, double
    if (from == 1) {
      return to == 3 || to == 4 || to == 5 || to == 6;
    }
    // short -> int, long, float, double
    if (from == 2) {
      return to == 3 || to == 4 || to == 5 || to == 6;
    }
    // int -> long, float, double
    if (from == 3) {
      return to == 4 || to == 5 || to == 6;
    }
    // long -> float, double
    if (from == 4) {
      return to == 5 || to == 6;
    }
    // float -> double
    return from == 5 && to == 6;
  }

  @Test
  void comprehensiveTest() {
    StringBuilder code = new StringBuilder(512);
    code.append("package wpilib.robot;\n\nclass Example {\n");

    // Define variables for each type
    for (int i = 0; i < types.length; i++) {
      code.append("  ").append(types[i]).append(" v").append(i).append(" = 1;\n");
    }

    int expectedErrors = 0;
    int testCount = 0;
    for (int t1 = 0; t1 < 7; t1++) {
      for (int t2 = 0; t2 < 7; t2++) {
        int res = promote(t1, t2);
        for (int t3 = 0; t3 < 7; t3++) {
          if (canAssign(res, t3)) {
            // Variation A: T3 v = (T1) a / (T2) b;
            code.append("  ")
                .append(types[t3])
                .append(" varA_")
                .append(testCount)
                .append(" = (")
                .append(types[t1])
                .append(") v")
                .append(t1)
                .append(" / (")
                .append(types[t2])
                .append(") v")
                .append(t2)
                .append(";\n  ");
            if (isInteger(t1) && isInteger(t2) && isFloat(t3)) {
              expectedErrors++;
            }
            testCount++;

            // Variation B: T3 v = (T3) ((T1) a / (T2) b);
            code.append(types[t3])
                .append(" varB_")
                .append(testCount)
                .append(" = (")
                .append(types[t3])
                .append(") ((")
                .append(types[t1])
                .append(") v")
                .append(t1)
                .append(" / (")
                .append(types[t2])
                .append(") v")
                .append(t2)
                .append(");\n  ");
            if (isInteger(t1) && isInteger(t2) && isFloat(t3)) {
              expectedErrors++;
            }
            testCount++;

            // Variation C: T3 v = ((T3) (T1) a) / (T2) b;
            code.append(types[t3])
                .append(" varC_")
                .append(testCount)
                .append(" = ((")
                .append(types[t3])
                .append(") (")
                .append(types[t1])
                .append(") v")
                .append(t1)
                .append(") / (")
                .append(types[t2])
                .append(") v")
                .append(t2)
                .append(";\n");
            // Variation C should never fail
            testCount++;
          }
        }
      }
    }
    code.append("}\n");

    final int finalExpectedErrors = expectedErrors;
    final String finalSource = code.toString();
    var compilation =
        javac()
            .withOptions(CompileTestUtils.kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("wpilib.robot.Example", finalSource));

    var errors = compilation.errors();
    assertEquals(
        finalExpectedErrors,
        errors.size(),
        () ->
            "Expected "
                + finalExpectedErrors
                + " errors, but got "
                + errors.size()
                + ". Source:\n"
                + finalSource);

    for (var error : errors) {
      assertEquals("integer division in a floating-point context", error.getMessage(null));
    }
  }

  @Test
  void integerDivisionForIntegerContext() {
    String source =
        """
        package wpilib.robot;

        class Example {
          int i = 1 / 2;
        }
        """;

    var compilation =
        javac()
            .withOptions(CompileTestUtils.kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void integerDivisionForFloatContext() {
    String source =
        """
        package wpilib.robot;

        class Example {
          float f = 1 / 2;
        }
        """;

    var compilation =
        javac()
            .withOptions(CompileTestUtils.kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).failed();
    var errors = compilation.errors();
    assertEquals(1, errors.size());
    var error = errors.get(0);
    assertEquals("integer division in a floating-point context", error.getMessage(null));
  }

  @Test
  void suppressWarnings() {
    String source =
        """
        package wpilib.robot;

        class Example {
          @SuppressWarnings("IntegerDivision")
          float f = 1 / 2;

          void method() {
            @SuppressWarnings("IntegerDivision")
            double d = 3 / 4;

            @SuppressWarnings("all")
            float f2 = 5 / 6;
          }

          @SuppressWarnings("IntegerDivision")
          double method2() {
            return 7 / 8;
          }
        }
        """;

    var compilation =
        javac()
            .withOptions(CompileTestUtils.kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }
}
