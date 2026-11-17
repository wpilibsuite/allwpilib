// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.epilogue.processor;

import static com.google.testing.compile.CompilationSubject.assertThat;
import static com.google.testing.compile.Compiler.javac;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.wpilib.epilogue.processor.CompileTestOptions.JAVA_VERSION_OPTIONS;

import com.google.testing.compile.Compilation;
import com.google.testing.compile.JavaFileObjects;
import java.io.IOException;
import java.util.Collection;
import java.util.List;
import javax.tools.JavaFileObject;
import org.junit.jupiter.api.Test;

@SuppressWarnings("checkstyle:LineLength") // Source code templates exceed the line length limit
class EpilogueGeneratorTest {
  @Test
  void noFields() {
    String source =
        """
          package org.wpilib.epilogue;

          @Logged
          class Example {
          }
          """;

    String expected =
        """
        package org.wpilib.epilogue.generated;

        public final class EpilogueLoggers {
          public static final org.wpilib.epilogue.ExampleLogger org_wpilib_epilogue_ExampleLogger = new org.wpilib.epilogue.ExampleLogger();
        }
        """;

    var compilation =
        compile(List.of(JavaFileObjects.forSourceString("org.wpilib.epilogue.Example", source)));
    assertGeneratedEpilogueLoggers(compilation, expected);
  }

  /** Subclassing RobotBase should not generate the bind() method because it lacks addPeriodic(). */
  @Test
  void robotBase() {
    String source =
        """
          package org.wpilib.epilogue;

          @Logged
          public class Example extends org.wpilib.framework.RobotBase {
            @Override
            public void startCompetition() {}
            @Override
            public void endCompetition() {}
          }
          """;

    String expected =
        """
        package org.wpilib.epilogue.generated;

        public final class EpilogueLoggers {
          public static final org.wpilib.epilogue.ExampleLogger org_wpilib_epilogue_ExampleLogger = new org.wpilib.epilogue.ExampleLogger();
        }
        """;

    var compilation =
        compile(List.of(JavaFileObjects.forSourceString("org.wpilib.epilogue.Example", source)));
    assertGeneratedEpilogueLoggers(compilation, expected);
  }

  @Test
  void timedRobot() {
    String source =
        """
          package org.wpilib.epilogue;

          @Logged
          public class Example extends org.wpilib.framework.TimedRobot {
          }
          """;

    String expected =
        """
        package org.wpilib.epilogue.generated;

        public final class EpilogueLoggers {
          public static final org.wpilib.epilogue.ExampleLogger org_wpilib_epilogue_ExampleLogger = new org.wpilib.epilogue.ExampleLogger();
        }
        """;

    var compilation =
        compile(List.of(JavaFileObjects.forSourceString("org.wpilib.epilogue.Example", source)));
    assertGeneratedEpilogueLoggers(compilation, expected);
  }

  @Test
  void multipleRobots() {
    String alphaSource =
        """
          package org.wpilib.epilogue;

          @Logged
          public class AlphaBot extends org.wpilib.framework.TimedRobot { }
          """;
    String betaSource =
        """
          package org.wpilib.epilogue;

          @Logged
          public class BetaBot extends org.wpilib.framework.TimedRobot { }
          """;

    String expected =
        """
        package org.wpilib.epilogue.generated;

        public final class EpilogueLoggers {
          public static final org.wpilib.epilogue.AlphaBotLogger org_wpilib_epilogue_AlphaBotLogger = new org.wpilib.epilogue.AlphaBotLogger();
          public static final org.wpilib.epilogue.BetaBotLogger org_wpilib_epilogue_BetaBotLogger = new org.wpilib.epilogue.BetaBotLogger();
        }
        """;

    var compilation =
        compile(
            List.of(
                JavaFileObjects.forSourceString("org.wpilib.epilogue.AlphaBot", alphaSource),
                JavaFileObjects.forSourceString("org.wpilib.epilogue.BetaBot", betaSource)));

    assertGeneratedEpilogueLoggers(compilation, expected);
  }

  @Test
  void genericCustomLogger() {
    JavaFileObject classA =
        JavaFileObjects.forSourceString(
            "org.wpilib.epilogue.A", "package org.wpilib.epilogue; class A {}");

    JavaFileObject classB =
        JavaFileObjects.forSourceString(
            "org.wpilib.epilogue.B", "package org.wpilib.epilogue; class B extends A {}");

    JavaFileObject classC =
        JavaFileObjects.forSourceString(
            "org.wpilib.epilogue.C", "package org.wpilib.epilogue; class C extends A {}");

    JavaFileObject loggedClass =
        JavaFileObjects.forSourceString(
            "org.wpilib.epilogue.Example",
            """
        package org.wpilib.epilogue;

        @Logged
        public class Example {
          A a_b_or_c;
          B b;
          C c;
        }
        """);

    JavaFileObject logger =
        JavaFileObjects.forSourceString(
            "org.wpilib.epilogue.CustomLogger",
            """
            package org.wpilib.epilogue;

            import org.wpilib.epilogue.logging.*;
            import org.wpilib.telemetry.TelemetryTable;

            @CustomLoggerFor({A.class, B.class, C.class})
            public class CustomLogger extends ClassSpecificLogger<A> {
              public CustomLogger() { super(A.class); }

              @Override
              public void update(TelemetryTable table, A object) {} // implementation is irrelevant
            }
            """);

    String expected =
        """
        package org.wpilib.epilogue.generated;

        public final class EpilogueLoggers {
          public static final org.wpilib.epilogue.ExampleLogger org_wpilib_epilogue_ExampleLogger = new org.wpilib.epilogue.ExampleLogger();
          public static final org.wpilib.epilogue.CustomLogger org_wpilib_epilogue_CustomLogger = new org.wpilib.epilogue.CustomLogger();
        }
        """;

    var compilation = compile(List.of(classA, classB, classC, loggedClass, logger));
    assertGeneratedEpilogueLoggers(compilation, expected);
  }

  private static Compilation compile(Collection<JavaFileObject> jfos) {
    Compilation compilation =
        javac()
            .withOptions(JAVA_VERSION_OPTIONS)
            .withProcessors(new AnnotationProcessor())
            .compile(jfos);

    assertThat(compilation).succeededWithoutWarnings();
    return compilation;
  }

  private void assertGeneratedEpilogueLoggers(Compilation compilation, String expectedContent) {
    var generatedFile =
        compilation.generatedSourceFiles().stream()
            .filter(jfo -> jfo.getName().contains("EpilogueLoggers"))
            .findFirst()
            .orElseThrow(
                () -> new IllegalStateException("EpilogueLoggers file was not generated!"));
    try {
      var content = generatedFile.getCharContent(false);
      assertEquals(
          expectedContent.replace("\r\n", "\n").strip(),
          content.toString().replace("\r\n", "\n").strip());
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
  }
}
