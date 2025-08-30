// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.javacplugin;

import static com.google.testing.compile.CompilationSubject.assertThat;
import static com.google.testing.compile.Compiler.javac;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.wpilib.javacplugin.CompileTestOptions.getErrorSource;
import static org.wpilib.javacplugin.CompileTestOptions.kJavaVersionOptions;

import com.google.testing.compile.Compilation;
import com.google.testing.compile.JavaFileObjects;
import org.junit.jupiter.api.Test;

class CodeAfterCoroutineParkDetectorTest {
  private static final String kCoroutineSource =
      """
      package org.wpilib.commands3;

      public interface Coroutine {
        void park();
      }
      """;

  @Test
  void soloPark() {
    String source =
        """
      package frc.robot;

      import java.util.function.Consumer;
      import org.wpilib.commands3.Coroutine;

      class Example {
        Consumer<Coroutine> lambda = coroutine -> {
          coroutine.park();
        };
      }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(
                JavaFileObjects.forSourceString("org.wpilib.commands3.Coroutine", kCoroutineSource),
                JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void printAfterPark() {
    String source =
        """
      package frc.robot;

      import java.util.function.Consumer;
      import org.wpilib.commands3.Coroutine;

      class Example {
        Consumer<Coroutine> lambda = coroutine -> {
          coroutine.park();
          System.out.println("Unreachable 1"); // this line should get an error
          System.out.println("Unreachable 2"); // but not this one
        };
      }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(
                JavaFileObjects.forSourceString("org.wpilib.commands3.Coroutine", kCoroutineSource),
                JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).failed();
    assertEquals(1, compilation.errors().size());
    var error = compilation.errors().get(0);
    assertEquals("Unreachable statement: coroutine.park() will never exit", error.getMessage(null));
    assertEquals("System.out.println(\"Unreachable 1\"); ", getErrorSource(error));
  }

  @Test
  void printAfterParkInBlock() {
    String source =
        """
      package frc.robot;

      import java.util.function.Consumer;
      import org.wpilib.commands3.Coroutine;

      class Example {
        Consumer<Coroutine> lambda = coroutine -> {
          {
            // Only .park() calls at the top level are detected
            coroutine.park();
          }
          System.out.println("Unreachable 1");
          System.out.println("Unreachable 2");
        };
      }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(
                JavaFileObjects.forSourceString("org.wpilib.commands3.Coroutine", kCoroutineSource),
                JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void loopAfterPark() {
    String source =
        """
      package frc.robot;

      import java.util.function.Consumer;
      import org.wpilib.commands3.Coroutine;

      class Example {
        Consumer<Coroutine> lambda = coroutine -> {
          coroutine.park();
          for (;;) {
          }
        };
      }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(
                JavaFileObjects.forSourceString("org.wpilib.commands3.Coroutine", kCoroutineSource),
                JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).failed();
    assertEquals(1, compilation.errors().size());
    var error = compilation.errors().get(0);
    assertEquals("Unreachable statement: coroutine.park() will never exit", error.getMessage(null));
    assertEquals("for (;;) {\n    }\n", getErrorSource(error));
  }

  @Test
  void blockAfterPark() {
    String source =
        """
      package frc.robot;

      import java.util.function.Consumer;
      import org.wpilib.commands3.Coroutine;

      class Example {
        Consumer<Coroutine> lambda = coroutine -> {
          coroutine.park();
          {
          }
        };
      }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(
                JavaFileObjects.forSourceString("org.wpilib.commands3.Coroutine", kCoroutineSource),
                JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).failed();
    assertEquals(1, compilation.errors().size());
    var error = compilation.errors().get(0);
    assertEquals("Unreachable statement: coroutine.park() will never exit", error.getMessage(null));
    assertEquals("{\n    }\n", getErrorSource(error));
  }

  @Test
  void emptyStatementAfterPark() {
    String source =
        """
      package frc.robot;

      import java.util.function.Consumer;
      import org.wpilib.commands3.Coroutine;

      class Example {
        Consumer<Coroutine> lambda = coroutine -> {
          coroutine.park();;
        };
      }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(
                JavaFileObjects.forSourceString("org.wpilib.commands3.Coroutine", kCoroutineSource),
                JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void printAfterEmptyStatementAfterPark() {
    String source =
        """
      package frc.robot;

      import java.util.function.Consumer;
      import org.wpilib.commands3.Coroutine;

      class Example {
        Consumer<Coroutine> lambda = coroutine -> {
          coroutine.park();;
          System.out.println("Unreachable");
        };
      }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(
                JavaFileObjects.forSourceString("org.wpilib.commands3.Coroutine", kCoroutineSource),
                JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).failed();
    assertEquals(1, compilation.errors().size());
    var error = compilation.errors().get(0);
    assertEquals("Unreachable statement: coroutine.park() will never exit", error.getMessage(null));
    assertEquals("System.out.println(\"Unreachable\");\n", getErrorSource(error));
  }
}
