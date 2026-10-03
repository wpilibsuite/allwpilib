// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.epilogue.processor;

import static com.google.testing.compile.CompilationSubject.assertThat;
import static com.google.testing.compile.Compiler.javac;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.wpilib.epilogue.processor.CompileTestOptions.JAVA_VERSION_OPTIONS;

import com.google.testing.compile.Compilation;
import com.google.testing.compile.JavaFileObjects;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.ServiceLoader;
import javax.tools.JavaFileObject;
import org.junit.jupiter.api.Test;
import org.wpilib.epilogue.EpilogueService;

@SuppressWarnings("checkstyle:LineLength") // Source code templates exceed line length
class EpilogueResourceGeneratorTest {
  @Test
  void noRobots() {
    String source =
        """
          package org.wpilib.epilogue;

          @Logged
          class Example {
          }
          """;

    var compilation =
        compile(List.of(JavaFileObjects.forSourceString("org.wpilib.epilogue.Example", source)));
    assertNoServicesResource(compilation);
  }

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

    var compilation =
        compile(List.of(JavaFileObjects.forSourceString("org.wpilib.epilogue.Example", source)));
    assertGeneratedServicesResource(
        compilation, List.of("org.wpilib.epilogue.Example_EpilogueService"));

    var services = loadServicesAtRuntime(compilation);
    assertEquals(1, services.size());
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

    var compilation =
        compile(List.of(JavaFileObjects.forSourceString("org.wpilib.epilogue.Example", source)));
    assertGeneratedServicesResource(
        compilation, List.of("org.wpilib.epilogue.Example_EpilogueService"));

    var services = loadServicesAtRuntime(compilation);
    assertEquals(1, services.size());
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

    var compilation =
        compile(
            List.of(
                JavaFileObjects.forSourceString("org.wpilib.epilogue.AlphaBot", alphaSource),
                JavaFileObjects.forSourceString("org.wpilib.epilogue.BetaBot", betaSource)));

    assertGeneratedServicesResource(
        compilation,
        List.of(
            "org.wpilib.epilogue.AlphaBot_EpilogueService",
            "org.wpilib.epilogue.BetaBot_EpilogueService"));

    var services = loadServicesAtRuntime(compilation);
    assertEquals(2, services.size());
  }

  @Test
  void customPackageRobot() {
    String source =
        """
          package frc.robot;

          import org.wpilib.epilogue.Logged;

          @Logged
          public class MyBot extends org.wpilib.framework.TimedRobot {}
          """;

    var compilation = compile(List.of(JavaFileObjects.forSourceString("frc.robot.MyBot", source)));
    assertGeneratedServicesResource(compilation, List.of("frc.robot.MyBot_EpilogueService"));

    var services = loadServicesAtRuntime(compilation);
    assertEquals(1, services.size());
  }

  @Test
  void mixedRobotTypes() {
    String baseSource =
        """
          package org.wpilib.epilogue;

          @Logged
          public class BaseRobot extends org.wpilib.framework.RobotBase {
            @Override
            public void startCompetition() {}
            @Override
            public void endCompetition() {}
          }
          """;

    String timedSource =
        """
          package org.wpilib.epilogue;

          @Logged
          public class TimedRobotBot extends org.wpilib.framework.TimedRobot {}
          """;

    var compilation =
        compile(
            List.of(
                JavaFileObjects.forSourceString("org.wpilib.epilogue.BaseRobot", baseSource),
                JavaFileObjects.forSourceString("org.wpilib.epilogue.TimedRobotBot", timedSource)));

    assertGeneratedServicesResource(
        compilation,
        List.of(
            "org.wpilib.epilogue.BaseRobot_EpilogueService",
            "org.wpilib.epilogue.TimedRobotBot_EpilogueService"));

    var services = loadServicesAtRuntime(compilation);
    assertEquals(2, services.size());
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

  private void assertGeneratedServicesResource(
      Compilation compilation, List<String> expectedServices) {
    var resourceFile =
        compilation.generatedFiles().stream()
            .filter(
                jfo ->
                    jfo.toUri()
                        .getPath()
                        .endsWith("/META-INF/services/org.wpilib.epilogue.EpilogueService"))
            .findFirst()
            .orElseThrow(
                () ->
                    new IllegalStateException(
                        "META-INF/services/org.wpilib.epilogue.EpilogueService resource was not generated!"));
    try {
      var content = resourceFile.getCharContent(false).toString().replace("\r\n", "\n");
      var lines = content.lines().filter(s -> !s.isBlank()).toList();
      assertEquals(expectedServices, lines);
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
  }

  private void assertNoServicesResource(Compilation compilation) {
    boolean present =
        compilation.generatedFiles().stream()
            .anyMatch(
                jfo ->
                    jfo.toUri()
                        .getPath()
                        .endsWith("/META-INF/services/org.wpilib.epilogue.EpilogueService"));
    assertFalse(
        present, "META-INF/services/org.wpilib.epilogue.EpilogueService should not be generated");
  }

  @SuppressWarnings("rawtypes")
  private List<EpilogueService> loadServicesAtRuntime(Compilation compilation) {
    var loader = ServiceLoader.load(EpilogueService.class, new CompilationClassLoader(compilation));
    List<EpilogueService> services = new ArrayList<>();
    for (EpilogueService service : loader) {
      services.add(service);
    }
    return services;
  }
}
