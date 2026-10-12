// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.javacplugin;

import static com.google.testing.compile.CompilationSubject.assertThat;
import static com.google.testing.compile.Compiler.javac;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.wpilib.javacplugin.CompileTestUtils.JAVA_VERSION_OPTIONS;

import com.google.testing.compile.Compilation;
import com.google.testing.compile.JavaFileObjects;
import org.junit.jupiter.api.Test;

class CallSuperDetectorTest {
  @Test
  void overrideCallsSuper() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.annotation.CallSuper;

        class Example {
          @CallSuper
          public void foo() {
            System.out.println("foo");
          }
        }

        class Example2 extends Example {
          @Override
          public void foo() {
            super.foo();
            System.out.println("bar");
          }
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(JAVA_VERSION_OPTIONS)
            .compile(JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void overrideDoesntCallSuper() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.annotation.CallSuper;

        class Example {
          @CallSuper
          public void foo() {
            System.out.println("foo");
          }
        }

        class Example2 extends Example {
          @Override
          public void foo() {
            System.out.println("bar");
          }
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(JAVA_VERSION_OPTIONS)
            .compile(JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).failed();
    assertEquals(1, compilation.errors().size());
    var error = compilation.errors().get(0);
    assertEquals(
        "[WPILib] Override of @CallSuper method `Example.foo()` doesn't call `super.foo()`.",
        error.getMessage(null));
  }

  @Test
  void transitiveOverrideDoesntCallSuper() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.annotation.CallSuper;

        class Example {
          @CallSuper
          public void foo() {
            System.out.println("foo");
          }
        }

        class Example2 extends Example {
          @Override
          public void foo() {
            System.out.println("bar");
            super.foo();
          }
        }

        class Example3 extends Example2 {
          @Override
          public void foo() {
            System.out.println("baz");
          }
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(JAVA_VERSION_OPTIONS)
            .compile(JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).failed();
    assertEquals(1, compilation.errors().size());
    var error = compilation.errors().get(0);
    assertEquals(
        "[WPILib] Override of @CallSuper method `Example.foo()` doesn't call `super.foo()`.",
        error.getMessage(null));
  }

  @Test
  void subclassWithCallSuper() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.annotation.CallSuper;

        class Example {
          public void foo() {
            System.out.println("foo");
          }
        }

        class Example2 extends Example {
          @Override
          @CallSuper
          public void foo() {
            System.out.println("bar");
          }
        }

        class Example3 extends Example2 {
          @Override
          public void foo() {
            super.foo();
            System.out.println("baz");
          }
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(JAVA_VERSION_OPTIONS)
            .compile(JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void anonymousOverrideDoesntCallSuper() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.annotation.CallSuper;

        class Example {
          @CallSuper
          public void foo() {
            System.out.println("foo");
          }

          public static void bar() {
            var a = new Example() {
              @Override
              public void foo() {
                System.out.println("bar");
              }
            };
          }
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(JAVA_VERSION_OPTIONS)
            .compile(JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).failed();
    assertEquals(1, compilation.errors().size());
    var error = compilation.errors().get(0);
    assertEquals(
        "[WPILib] Override of @CallSuper method `Example.foo()` doesn't call `super.foo()`.",
        error.getMessage(null));
  }

  @Test
  void customLabel() {
    String source =
        """
        package wpilib.robot;

        import org.wpilib.annotation.CallSuper;

        class Example {
          @CallSuper("Overrides of `foo` must call `super.foo()`")
          public void foo() {
            System.out.println("foo");
          }
        }

        class Example2 extends Example {
          @Override
          public void foo() {
            System.out.println("bar");
          }
        }
        """;

    Compilation compilation =
        javac()
            .withOptions(JAVA_VERSION_OPTIONS)
            .compile(JavaFileObjects.forSourceString("wpilib.robot.Example", source));

    assertThat(compilation).failed();
    assertEquals(1, compilation.errors().size());
    var error = compilation.errors().get(0);
    assertEquals("[WPILib] Overrides of `foo` must call `super.foo()`.", error.getMessage(null));
  }
}
