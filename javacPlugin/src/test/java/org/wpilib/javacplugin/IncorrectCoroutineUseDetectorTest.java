package org.wpilib.javacplugin;

import static com.google.testing.compile.CompilationSubject.assertThat;
import static com.google.testing.compile.Compiler.javac;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.wpilib.javacplugin.CompileTestUtils.getErrorSource;
import static org.wpilib.javacplugin.CompileTestUtils.kJavaVersionOptions;

import com.google.testing.compile.Compilation;
import com.google.testing.compile.JavaFileObjects;
import org.junit.jupiter.api.Test;

class IncorrectCoroutineUseDetectorTest {
  private static final String kCoroutineSource =
      """
      package org.wpilib.commands3;

      public interface Coroutine {
        void yield();
      }
      """;

  @Test
  void methodCalledOnNonlocalCoroutine() {
    String source =
        """
        package frc.robot;

        import org.wpilib.commands3.Coroutine;
        import java.util.function.Consumer;

        class Example {
          Consumer<Coroutine> lambda = outerCoroutine -> {
            Consumer<Coroutine> lambda2 = innerCoroutine -> {
              outerCoroutine.yield();
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
    assertEquals(
        "Coroutine `outerCoroutine` may not be in scope. Consider using `innerCoroutine`",
        error.getMessage(null));
  }

  @Test
  void nonlocalCoroutinePassedToMethod() {
    String source =
        """
        package frc.robot;

        import org.wpilib.commands3.Coroutine;
        import java.util.function.Consumer;

        class Example {
          Consumer<Coroutine> lambda = outerCoroutine -> {
            Consumer<Coroutine> lambda2 = innerCoroutine -> {
              method(outerCoroutine);
            };
          };

          void method(Coroutine foo) { }
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
        "Coroutine `outerCoroutine` may not be in scope. Consider using `innerCoroutine`",
        error.getMessage(null));
  }

  @Test
  void twoLocalCoroutines() {
    String source =
        """
        package frc.robot;

        import org.wpilib.commands3.Coroutine;
        import java.util.function.BiConsumer;
        import java.util.function.Consumer;

        class Example {
          Consumer<Coroutine> lambda = outerCoroutine -> {
            BiConsumer<Coroutine, Coroutine> lambda2 = (a, b) -> {
              method(outerCoroutine);
            };
          };

          void method(Coroutine foo) { }
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
        "Coroutine `outerCoroutine` may not be in scope. Consider using `a` or `b`",
        error.getMessage(null));
  }

  @Test
  void threeLocalCoroutines() {
    String source =
        """
        package frc.robot;

        import org.wpilib.commands3.Coroutine;
        import java.util.function.Consumer;

        @FunctionalInterface
        interface TriConsumer<T, U, V> {
          void accept(T t, U u, V v);
        }

        class Example {
          Consumer<Coroutine> lambda = outerCoroutine -> {
            TriConsumer<Coroutine, Coroutine, Coroutine> lambda2 = (a, b, c) -> {
              method(outerCoroutine);
            };
          };

          void method(Coroutine foo) { }
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
        "Coroutine `outerCoroutine` may not be in scope. Consider using `a`, `b`, or `c`",
        error.getMessage(null));
  }

  @Test
  void coroutineSavedToFieldErrors() {
    String source =
        """
        package frc.robot;

        import org.wpilib.commands3.Coroutine;
        import java.util.function.Consumer;

        class Example {
          Coroutine coroutineField;

          Consumer<Coroutine> lambda = coroutine -> {
            coroutineField = coroutine;
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
    assertEquals("Captured coroutines may not be stored in fields", error.getMessage(null));
  }

  @Test
  void outerCoroutineSavedToFieldErrors() {
    String source =
        """
        package frc.robot;

        import org.wpilib.commands3.Coroutine;
        import java.util.function.Consumer;

        class Example {
          Coroutine coroutineField;

          Consumer<Coroutine> lambda = outerCoroutine -> {
            Consumer<Coroutine> lambda2 = innerCoroutine -> {
              coroutineField = outerCoroutine;
              coroutineField = innerCoroutine;
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
    assertEquals(2, compilation.errors().size());

    var error1 = compilation.errors().get(0);
    assertEquals("Captured coroutines may not be stored in fields", error1.getMessage(null));
    assertEquals(11, error1.getLineNumber());
    assertEquals("coroutineField = outerCoroutine;", getErrorSource(error1));

    var error2 = compilation.errors().get(1);
    assertEquals("Captured coroutines may not be stored in fields", error2.getMessage(null));
    assertEquals("coroutineField = innerCoroutine;", getErrorSource(error2));
    assertEquals(12, error2.getLineNumber());
  }

  @Test
  void coroutineSavedToVariableIsAllowed() {
    String source =
        """
        package frc.robot;

        import org.wpilib.commands3.Coroutine;
        import java.util.function.Consumer;

        class Example {
          Consumer<Coroutine> lambda = coroutine -> {
            Coroutine local = coroutine;
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

  // This test is really to make it clear that the compiler plugin can't feasibly check for
  // everything, rather than this being a usecase we actually want to support.
  @Test
  void coroutineSavedToFieldViaMethodCannotBeDetected() {
    String source =
        """
        package frc.robot;

        import org.wpilib.commands3.Coroutine;
        import java.util.function.Consumer;

        class Example {
          private Coroutine coroutineField;

          Consumer<Coroutine> lambda = coroutine -> {
            saveCoroutine(coroutine);
          };

          void saveCoroutine(Coroutine coroutine) {
            coroutineField = coroutine;
          }
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
}
