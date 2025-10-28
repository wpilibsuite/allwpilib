// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.javacplugin;

import static com.google.testing.compile.CompilationSubject.assertThat;
import static com.google.testing.compile.Compiler.javac;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.wpilib.javacplugin.CompileTestOptions.kJavaVersionOptions;

import com.google.testing.compile.Compilation;
import com.google.testing.compile.JavaFileObjects;
import org.junit.jupiter.api.Test;

class PostConstructionInitializerListenerTest {
  @Test
  void instanceInitializerIsUsed() {
    String source =
        """
        package frc.robot;

        import org.wpilib.annotation.PostConstructionInitializer;

        class Example {
          @PostConstructionInitializer
          void init() { }

          static void usage() {
            var example = new Example();
            example.init();
          }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void instanceInitializerIsNotUsed() {
    String source =
        """
        package frc.robot;

        import org.wpilib.annotation.PostConstructionInitializer;

        class Example {
          @PostConstructionInitializer
          void init() { }

          static void usage() {
            Example example = new Example();
            // example.init();
          }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).failed();
    assertEquals(1, compilation.errors().size());
    var error = compilation.errors().get(0);
    assertEquals(
        "Partially-initialized object `example` is missing a call to initializer method `init()`",
        error.getMessage(null));
  }

  @Test
  void instanceInitializerIsUsedInFactory() {
    String source =
        """
        package frc.robot;

        import org.wpilib.annotation.PostConstructionInitializer;

        class Example {
          @PostConstructionInitializer
          void init() { }

          static Example makeExample() {
            var example = new Example();
            example.init();
            return example;
          }

          static void usage() {
            var example = makeExample();
          }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void instanceInitializerCalledInInnerBlock() {
    String source =
        """
        package frc.robot;

        import org.wpilib.annotation.PostConstructionInitializer;

        class Example {
          @PostConstructionInitializer
          void init() { }

          static void usage() {
            var example = new Example();
            if (false) {
              // Will never actually run, but the plugin doesn't ignore dead branches
              example.init();
            }
          }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void instanceInitializerInConstructorDoesNotCount() {
    String source =
        """
        package frc.robot;

        import org.wpilib.annotation.PostConstructionInitializer;

        class Example {
          public Example() {
            init();
          }

          @PostConstructionInitializer
          void init() { }

          static void usage() {
            var example = new Example();
          }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).failed();
    assertEquals(1, compilation.errors().size());
    var error = compilation.errors().get(0);
    assertEquals(
        "Partially-initialized object `example` is missing a call to initializer method `init()`",
        error.getMessage(null));
  }

  @Test
  void staticInitializerIsUsed() {
    String source =
        """
        package frc.robot;

        import org.wpilib.annotation.PostConstructionInitializer;

        class Example {
          @PostConstructionInitializer
          static void init(Example e) { }

          static void usage() {
            var example = new Example();
            Example.init(example);
          }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void staticInitializerIsNotUsed() {
    String source =
        """
        package frc.robot;

        import org.wpilib.annotation.PostConstructionInitializer;

        class Example {
          @PostConstructionInitializer
          static void init(Example e) { }

          static void usage() {
            var example = new Example();
            // Example.init(example);
          }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).failed();
    assertEquals(1, compilation.errors().size());
    var error = compilation.errors().get(0);
    assertEquals(
        "Partially-initialized object `example` is missing a call to initializer method `init()`",
        error.getMessage(null));
  }

  @Test
  void checksForInitializersFromInterfaces() {
    String source =
        """
        package frc.robot;

        import org.wpilib.annotation.PostConstructionInitializer;

        interface I1 {
          @PostConstructionInitializer
          default void i1Init() {}
        }

        interface I2 extends I1 {
          @PostConstructionInitializer
          default void i2Init() {}
        }

        class Example implements I2 {
          @PostConstructionInitializer
          void init() { }

          static void usage() {
            var example = new Example();
            example.init();
          }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).failed();
    assertEquals(1, compilation.errors().size());
    var error = compilation.errors().get(0);
    assertEquals(
        "Partially-initialized object `example` is missing calls to 2 initializer methods: "
            + "`i1Init()`, `i2Init()`",
        error.getMessage(null));
  }

  @Test
  void initializerCalledInOtherContext() {
    String source =
        """
        package frc.robot;

        import org.wpilib.annotation.PostConstructionInitializer;

        class Example {
          @PostConstructionInitializer
          void init() { }
        }

        class User {
          Example example = new Example();

          void later() {
            example.init();
          }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void initializerCalledAfterConstructor() {
    String source =
        """
        package frc.robot;

        import org.wpilib.annotation.PostConstructionInitializer;

        class Example {
          @PostConstructionInitializer
          void init() { }
        }

        class User {
          Example example;

          User() {
            example = new Example();
          }

          void later() {
            example.init();
          }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void initializerNotCalledAfterConstructor() {
    String source =
        """
        package frc.robot;

        import org.wpilib.annotation.PostConstructionInitializer;

        class Example {
          @PostConstructionInitializer
          void init() { }
        }

        class User {
          Example example;

          User() {
            example = new Example();
          }

          void later() {
            // example.init();
          }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).failed();
    assertEquals(1, compilation.errors().size());
    var error = compilation.errors().get(0);
    assertEquals(
        "Partially-initialized object `example` is missing a call to initializer method `init()`",
        error.getMessage(null));
  }

  @Test
  void initializerCalledOnAccessorAfterConstructor() {
    String source =
        """
        package frc.robot;

        import org.wpilib.annotation.PostConstructionInitializer;

        class Example {
          @PostConstructionInitializer
          void init() { }
        }

        class User {
          Example example;

          User() {
            example = new Example();
          }

          Example getExample() {
            return example;
          }

          void later() {
            // The plugin can't detect calls from accessor methods.
            // Initializers MUST be called on the variable directly.
            getExample().init();
          }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).failed();
    assertEquals(1, compilation.errors().size());
    var error = compilation.errors().get(0);
    assertEquals(
        "Partially-initialized object `example` is missing a call to initializer method `init()`",
        error.getMessage(null));
  }

  @Test
  void suppressWarningsOnConstructorCall() {
    String source =
        """
        package frc.robot;

        import org.wpilib.annotation.PostConstructionInitializer;

        class Example {
          @PostConstructionInitializer
          void init() { }

          static void usage() {
            @SuppressWarnings("PostConstructionInitializer")
            var example = new Example();
            // example.init();
          }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void suppressWarningsOnCallerMethod() {
    String source =
        """
        package frc.robot;

        import org.wpilib.annotation.PostConstructionInitializer;

        class Example {
          @PostConstructionInitializer
          void init() { }

          @SuppressWarnings("PostConstructionInitializer")
          static void usage() {
            var example = new Example();
            // example.init();
          }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void suppressWarningsOnClass() {
    String source =
        """
        package frc.robot;

        import org.wpilib.annotation.PostConstructionInitializer;

        @SuppressWarnings("PostConstructionInitializer")
        class Example {
          @PostConstructionInitializer
          void init() { }

          static void usage() {
            var example = new Example();
            // example.init();
          }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }
}
