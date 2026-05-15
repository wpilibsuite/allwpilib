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

@SuppressWarnings("LineLength") // Inline source code can have long lines
class OpModeAnnotationValidatorTest {
  private static final String AUTONOMOUS_ANNOTATION_SOURCE =
      """
      package org.wpilib.opmode;

      import java.lang.annotation.*;

      @Retention(RetentionPolicy.RUNTIME)
      @Target(ElementType.TYPE)
      public @interface Autonomous {
        String name() default "";
        String description() default "";
        String group() default "";
      }
      """;

  private static final String TELEOP_ANNOTATION_SOURCE =
      """
      package org.wpilib.opmode;

      import java.lang.annotation.*;

      @Retention(RetentionPolicy.RUNTIME)
      @Target(ElementType.TYPE)
      public @interface Teleop {
        String name() default "";
        String description() default "";
        String group() default "";
      }
      """;

  private static final String TEST_OPMODE_ANNOTATION_SOURCE =
      """
      package org.wpilib.opmode;

      import java.lang.annotation.*;

      @Retention(RetentionPolicy.RUNTIME)
      @Target(ElementType.TYPE)
      public @interface TestOpMode {
        String name() default "";
        String description() default "";
        String group() default "";
      }
      """;

  @Test
  void stringLiteralLessThanConfiguredMaxLength() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.opmode.*;

        @Autonomous(name = "Short Name", description = "Short Description", group = "Short Group")
        @Teleop(name = "Short Name", description = "Short Description", group = "Short Group")
        @TestOpMode(name = "Short Name", description = "Short Description", group = "Short Group")
        class Example {
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(
                JavaFileObjects.forSourceString(
                    "org.wpilib.opmode.Autonomous", AUTONOMOUS_ANNOTATION_SOURCE),
                JavaFileObjects.forSourceString(
                    "org.wpilib.opmode.Teleop", TELEOP_ANNOTATION_SOURCE),
                JavaFileObjects.forSourceString(
                    "org.wpilib.opmode.TestOpMode", TEST_OPMODE_ANNOTATION_SOURCE),
                JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void stringLiteralsGreaterThanConfiguredMaxLength() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.opmode.*;

        @Autonomous(
          name = "This is much longer than thirty six characters (I counted them all myself)",
          description = "This is significantly longer than sixty four characters (it's ninety nine, if you bother to count!)",
          group = "More than twelve characters long"
        )
        @Teleop(
          name = "This is much longer than thirty six characters (I counted them all myself)",
          description = "This is significantly longer than sixty four characters (it's ninety nine, if you bother to count!)",
          group = "More than twelve characters long"
        )
        @TestOpMode(
          name = "This is much longer than thirty six characters (I counted them all myself)",
          description = "This is significantly longer than sixty four characters (it's ninety nine, if you bother to count!)",
          group = "More than twelve characters long"
        )
        class Example {
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(
                JavaFileObjects.forSourceString(
                    "org.wpilib.opmode.Autonomous", AUTONOMOUS_ANNOTATION_SOURCE),
                JavaFileObjects.forSourceString(
                    "org.wpilib.opmode.Teleop", TELEOP_ANNOTATION_SOURCE),
                JavaFileObjects.forSourceString(
                    "org.wpilib.opmode.TestOpMode", TEST_OPMODE_ANNOTATION_SOURCE),
                JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).failed();
    var errors = compilation.errors();
    assertEquals(9, errors.size());

    // Autonomous
    assertEquals(
        "@Autonomous opmode name must be <= 32 characters (was 74)",
        errors.get(0).getMessage(null));
    assertEquals(
        "@Autonomous opmode group must be <= 12 characters (was 32)",
        errors.get(1).getMessage(null));
    assertEquals(
        "@Autonomous opmode description must be <= 64 characters (was 99)",
        errors.get(2).getMessage(null));

    // Teleop
    assertEquals(
        "@Teleop opmode name must be <= 32 characters (was 74)", errors.get(3).getMessage(null));
    assertEquals(
        "@Teleop opmode group must be <= 12 characters (was 32)", errors.get(4).getMessage(null));
    assertEquals(
        "@Teleop opmode description must be <= 64 characters (was 99)",
        errors.get(5).getMessage(null));

    // TestOpMode
    assertEquals(
        "@TestOpMode opmode name must be <= 32 characters (was 74)",
        errors.get(6).getMessage(null));
    assertEquals(
        "@TestOpMode opmode group must be <= 12 characters (was 32)",
        errors.get(7).getMessage(null));
    assertEquals(
        "@TestOpMode opmode description must be <= 64 characters (was 99)",
        errors.get(8).getMessage(null));
  }
}
