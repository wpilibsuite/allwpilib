// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.javacplugin;

import static com.google.testing.compile.CompilationSubject.assertThat;
import static com.google.testing.compile.Compiler.javac;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.wpilib.javacplugin.CompileTestUtils.kJavaVersionOptions;

import com.google.testing.compile.Compilation;
import com.google.testing.compile.JavaFileObjects;
import org.junit.jupiter.api.Test;

class MaxLengthDetectorTest {
  @Test
  void stringLiteralLessThanConfiguredMaxLength() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.annotation.MaxLength;

        class Example {
          Example(@MaxLength(10) String arg) {
          }

          Example() {
            this("short");
          }
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void stringLiteralLongerThanConfiguredMaxLength() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.annotation.MaxLength;

        class Example {
          Example(@MaxLength(1) String arg) {
          }

          Example() {
            this("abcdefghijklmnopqrstuvwxyz1234567890");
          }
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).failed();
    var errors = compilation.errors();
    assertEquals(1, errors.size());
    var error = errors.get(0);
    assertEquals(
        "String literal exceeds maximum length: \"abcdefghijklmnopqrstuvwxyz1234567890\""
            + " (36 characters) is longer than 1 character",
        error.getMessage(null));
  }

  @Test
  void stringLiteralConcatenationLongerThanConfiguredMaxLength() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.annotation.MaxLength;

        class Example {
          Example(@MaxLength(1) String arg) {
          }

          Example() {
            this("1" + "2");
          }
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).failed();
    var errors = compilation.errors();
    assertEquals(1, errors.size());
    var error = errors.get(0);
    assertEquals(
        "String literal exceeds maximum length: \"12\" (2 characters) is longer than 1 character",
        error.getMessage(null));
  }

  @Test
  void stringGenerationLongerThanConfiguredMaxLength() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.annotation.MaxLength;

        class Example {
          Example(@MaxLength(1) String arg) {
          }

          Example() {
            this("x".repeat(2));
          }
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    // Can't detect this
    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void zeroMaxLength() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.annotation.MaxLength;

        class Example {
          Example(@MaxLength(0) String arg) {
          }
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).failed();
    var errors = compilation.errors();
    assertEquals(1, errors.size());
    var error = errors.get(0);
    assertEquals("@MaxLength value must be >= 1 (was 0)", error.getMessage(null));
  }

  @Test
  void negativeMaxLength() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.annotation.MaxLength;

        class Example {
          Example(@MaxLength(-123) String arg) {
          }
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).failed();
    var errors = compilation.errors();
    assertEquals(1, errors.size());
    var error = errors.get(0);
    assertEquals("@MaxLength value must be >= 1 (was -123)", error.getMessage(null));
  }

  @Test
  void constantZeroMaxLength() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.annotation.MaxLength;

        class Example {
          public static final int CONFIGURED_MAX_LENGTH = 0;

          Example(@MaxLength(CONFIGURED_MAX_LENGTH) String arg) {
          }
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).failed();
    var errors = compilation.errors();
    assertEquals(1, errors.size());
    var error = errors.get(0);
    assertEquals("@MaxLength value must be >= 1 (was 0)", error.getMessage(null));
  }

  @Test
  void constantNegativeMaxLength() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.annotation.MaxLength;

        class Example {
          public static final int CONFIGURED_MAX_LENGTH = -3;

          Example(@MaxLength(CONFIGURED_MAX_LENGTH) String arg) {
          }
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).failed();
    var errors = compilation.errors();
    assertEquals(1, errors.size());
    var error = errors.get(0);
    assertEquals("@MaxLength value must be >= 1 (was -3)", error.getMessage(null));
  }
}
