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

class CoroutineInLoopListenerTest {
  private static final String kCoroutineSource =
      """
      package org.wpilib.commands3;

      public interface Coroutine {
        void yield();
      }
      """;

  @Test
  void noYieldInLoopWithoutCoroutines() {
    String source =
        """
      package frc.robot;

      class Example {
        Runnable lambda = () -> {
          while (true) {
          }
        };
      }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void basicYieldInLoopInLambda() {
    String source =
        """
      package frc.robot;

      import java.util.function.Consumer;
      import org.wpilib.commands3.Coroutine;

      class Example {
        Consumer<Coroutine> lambda = coroutine -> {
          while (true) {
            coroutine.yield();
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

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void basicYieldInLoopInMethod() {
    String source =
        """
      package frc.robot;

      import java.util.function.Consumer;
      import org.wpilib.commands3.Coroutine;

      class Example {
        void useCoroutine(Coroutine coroutine) {
          while (true) {
            coroutine.yield();
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

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void noYieldInLoopInLambda() {
    String source =
        """
      package frc.robot;

      import java.util.function.Consumer;
      import org.wpilib.commands3.Coroutine;

      class Example {
        Consumer<Coroutine> lambda = coroutine -> {
          while (true) {
            // No yield
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
    assertEquals("Missing call to `coroutine.yield()` inside loop", error.getMessage(null));
  }

  @Test
  void yieldInLoopInRunnableInLambda() {
    String source =
        """
      package frc.robot;

      import java.util.function.Consumer;
      import org.wpilib.commands3.Coroutine;

      class Example {
        Consumer<Coroutine> lambda = coroutine -> {
          Runnable r = () -> {
            while (true) {
              coroutine.yield();
            }
          };
        };
      }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(
                JavaFileObjects.forSourceString("org.wpilib.commands3.Coroutine", kCoroutineSource),
                JavaFileObjects.forSourceString("frc.robot.Example", source));

    // TODO: Should we make it an error to yield (or invoke any methods on) a captured coroutine?
    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void noYieldInMethodWithManyCoroutineParams() {
    String source =
        """
      package frc.robot;

      import java.util.function.Consumer;
      import org.wpilib.commands3.Coroutine;

      class Example {
        void manyCoroutineParams(
            Coroutine firstCoroutine,
            Coroutine c1,
            Coroutine next,
            Coroutine thisMightBeTheLastOne) {
          while (true) {
            // No yield
          }
        }
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
    assertEquals(
        "Missing call to "
            + "`firstCoroutine.yield()`, "
            + "`c1.yield()`, "
            + "`next.yield()`, or "
            + "`thisMightBeTheLastOne.yield()` inside loop",
        error.getMessage(null));
  }

  @Test
  void noYieldsInNestedLambda() {
    String source =
        """
      package frc.robot;

      import java.util.function.Consumer;
      import org.wpilib.commands3.Coroutine;

      class Example {
        Consumer<Coroutine> lambda = coroutine -> {
          Consumer<Coroutine> lambda2 = innerCoroutine -> {
            while (true) {
              // No yield
            }
          };
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
    assertEquals("Missing call to `innerCoroutine.yield()` inside loop", error.getMessage(null));
  }

  @Test
  void nestedLambdaYieldsToOuter() {
    String source =
        """
      package frc.robot;

      import java.util.function.Consumer;
      import org.wpilib.commands3.Coroutine;

      class Example {
        Consumer<Coroutine> lambda = outerCoroutine -> {
          Consumer<Coroutine> lambda2 = innerCoroutine -> {
            while (true) {
              outerCoroutine.yield();
            }
          };
        };
      }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(
                JavaFileObjects.forSourceString("org.wpilib.commands3.Coroutine", kCoroutineSource),
                JavaFileObjects.forSourceString("frc.robot.Example", source));

    // TODO: Should we make it an error to yield (or invoke any methods on) a captured coroutine?
    assertThat(compilation).failed();
    assertEquals(1, compilation.errors().size());
    var error = compilation.errors().get(0);
    assertEquals("Missing call to `innerCoroutine.yield()` inside loop", error.getMessage(null));
  }

  @Test
  void noYieldsInNestedLoops() {
    String source =
        """
      package frc.robot;

      import java.util.function.Consumer;
      import org.wpilib.commands3.Coroutine;

      class Example {
        Consumer<Coroutine> lambda = coroutine -> {
          while (0 == 0) {
            while (1 == 1) {
              // inner - no yield
            }
            // outer - also no yield
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
    assertEquals(2, compilation.errors().size());

    var error1 = compilation.errors().get(0);
    assertEquals("Missing call to `coroutine.yield()` inside loop", error1.getMessage(null));
    assertEquals(8, error1.getLineNumber());

    var error2 = compilation.errors().get(1);
    assertEquals("Missing call to `coroutine.yield()` inside loop", error2.getMessage(null));
    assertEquals(9, error2.getLineNumber());
  }

  @Test
  void noYieldInOuterLoopButYieldInInnerLoop() {
    String source =
        """
      package frc.robot;

      import java.util.function.Consumer;
      import org.wpilib.commands3.Coroutine;

      class Example {
        Consumer<Coroutine> lambda = coroutine -> {
          while (true) {
            coroutine.yield();
            while (true) {
              // No yields
            }
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
    assertEquals("Missing call to `coroutine.yield()` inside loop", error.getMessage(null));
    assertEquals(10, error.getLineNumber());
  }

  @Test
  void noYieldInInnerLoopButYieldInOuterLoop() {
    String source =
        """
      package frc.robot;

      import java.util.function.Consumer;
      import org.wpilib.commands3.Coroutine;

      class Example {
        Consumer<Coroutine> lambda = coroutine -> {
          while (true) {
            while (true) {
              coroutine.yield();
            }
            // No yields
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
    assertEquals("Missing call to `coroutine.yield()` inside loop", error.getMessage(null));
    assertEquals(8, error.getLineNumber());
  }

  @Test
  void noYieldsInDeeplyNestedLoops() {
    String source =
        """
      package frc.robot;

      import java.util.function.Consumer;
      import org.wpilib.commands3.Coroutine;

      class Example {
        Consumer<Coroutine> lambda = coroutine -> {
          while (0 == 0) {
            while (1 == 1) {
              while (2 == 2) {
                while (3 == 3) {
                  while (4 == 4) {
                    while (5 == 5) {
                      if (true) {
                        // nested if statement shouldn't mess with error detection or reporting
                        while (6 == 6) {
                        }
                      }
                    }
                  }
                }
              }
            }
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
    assertEquals(7, compilation.errors().size());

    var error1 = compilation.errors().get(0);
    assertEquals("Missing call to `coroutine.yield()` inside loop", error1.getMessage(null));
    assertEquals(8, error1.getLineNumber());

    var error2 = compilation.errors().get(1);
    assertEquals("Missing call to `coroutine.yield()` inside loop", error2.getMessage(null));
    assertEquals(9, error2.getLineNumber());

    var error3 = compilation.errors().get(2);
    assertEquals("Missing call to `coroutine.yield()` inside loop", error3.getMessage(null));
    assertEquals(10, error3.getLineNumber());

    var error4 = compilation.errors().get(3);
    assertEquals("Missing call to `coroutine.yield()` inside loop", error4.getMessage(null));
    assertEquals(11, error4.getLineNumber());

    var error5 = compilation.errors().get(4);
    assertEquals("Missing call to `coroutine.yield()` inside loop", error5.getMessage(null));
    assertEquals(12, error5.getLineNumber());

    var error6 = compilation.errors().get(5);
    assertEquals("Missing call to `coroutine.yield()` inside loop", error6.getMessage(null));
    assertEquals(13, error6.getLineNumber());

    var error7 = compilation.errors().get(6);
    assertEquals("Missing call to `coroutine.yield()` inside loop", error7.getMessage(null));
    assertEquals(16, error7.getLineNumber());
  }
}
