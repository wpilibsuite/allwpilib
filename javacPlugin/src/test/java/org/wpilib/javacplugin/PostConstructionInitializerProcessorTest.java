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

class PostConstructionInitializerProcessorTest {
  @Test
  void staticInitializerWithNoParameters() {
    String source =
        """
        package frc.robot;

        import org.wpilib.annotation.PostConstructionInitializer;

        class Example {
          @PostConstructionInitializer
          static void init() { }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .withProcessors(new PostConstructionInitializerProcessor())
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).failed();
    assertEquals(1, compilation.errors().size());
    var error = compilation.errors().get(0);
    assertEquals(
        "Static @PostConstructionInitializer method must take a parameter of type "
            + "frc.robot.Example",
        error.getMessage(null));
  }

  @Test
  void staticInitializerWithOneParameter() {
    String source =
        """
        package frc.robot;

        import org.wpilib.annotation.PostConstructionInitializer;

        class Example {
          @PostConstructionInitializer
          static void init(Example e) { }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .withProcessors(new PostConstructionInitializerProcessor())
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void staticInitializerWithOneAnnotatedParameter() {
    String source =
        """
        package frc.robot;

        import org.wpilib.annotation.PostConstructionInitializer;
        import org.wpilib.annotation.PostConstructionInitializer.InitializedParam;

        class Example {
          @PostConstructionInitializer
          static void init(@InitializedParam Example e) { }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .withProcessors(new PostConstructionInitializerProcessor())
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void staticInitializerWithOneAnnotatedParameterWithUnannotatedParameter() {
    String source =
        """
        package frc.robot;

        import org.wpilib.annotation.PostConstructionInitializer;
        import org.wpilib.annotation.PostConstructionInitializer.InitializedParam;

        class Example {
          @PostConstructionInitializer
          static void init(@InitializedParam Example dst, Example src) { }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .withProcessors(new PostConstructionInitializerProcessor())
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).succeededWithoutWarnings();
  }

  @Test
  void staticInitializerWithMultipleAnnotatedParameters() {
    String source =
        """
        package frc.robot;

        import org.wpilib.annotation.PostConstructionInitializer;
        import org.wpilib.annotation.PostConstructionInitializer.InitializedParam;

        class Example {
          @PostConstructionInitializer
          static void init(@InitializedParam Example a, @InitializedParam Example b) { }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .withProcessors(new PostConstructionInitializerProcessor())
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).failed();
    assertEquals(2, compilation.errors().size());
    var error1 = compilation.errors().get(0);
    assertEquals(
        "Static @PostConstructionInitializer method must take exactly one parameter of type "
            + "frc.robot.Example with a @PostConstructionInitializer.InitializedParam annotation",
        error1.getMessage(null));

    var error2 = compilation.errors().get(1);
    assertEquals(
        "Static @PostConstructionInitializer method must take exactly one parameter of type "
            + "frc.robot.Example with a @PostConstructionInitializer.InitializedParam annotation",
        error2.getMessage(null));
  }

  @Test
  void staticInitializerAcceptingBaseType() {
    String source =
        """
        package frc.robot;

        import org.wpilib.annotation.PostConstructionInitializer;
        import org.wpilib.annotation.PostConstructionInitializer.InitializedParam;

        class Base {}

        class Example extends Base {
          @PostConstructionInitializer
          static void init(@InitializedParam Base e) { }
        }
      """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .withProcessors(new PostConstructionInitializerProcessor())
            .compile(JavaFileObjects.forSourceString("frc.robot.Example", source));

    assertThat(compilation).failed();
    assertEquals(1, compilation.errors().size());
    var error = compilation.errors().get(0);
    assertEquals(
        "Static @PostConstructionInitializer method must take a parameter of type "
            + "frc.robot.Example",
        error.getMessage(null));
  }
}
