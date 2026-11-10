// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.epilogue.processor;

import static com.google.testing.compile.CompilationSubject.assertThat;
import static com.google.testing.compile.Compiler.javac;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertInstanceOf;
import static org.junit.jupiter.api.Assertions.assertTrue;
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

@SuppressWarnings({
  "checkstyle:LineLength",
  "PMD"
}) // Source code templates exceed line length, test reflection
class EpilogueServiceGeneratorTest {
  @Test
  void noRobotClass() {
    String source =
        """
          package org.wpilib.epilogue;

          @Logged
          class Example {
          }
          """;

    var compilation =
        compile(List.of(JavaFileObjects.forSourceString("org.wpilib.epilogue.Example", source)));
    assertNoServiceGenerated(compilation, "Example_EpilogueService");

    var services = loadServicesAtRuntime(compilation);
    assertTrue(services.isEmpty(), "No services should be loaded for non-robot classes");
  }

  @Test
  void robotBase() throws Exception {
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

    String expectedService =
        """
        package org.wpilib.epilogue;

        import org.wpilib.epilogue.Epilogue;
        import org.wpilib.epilogue.EpilogueConfiguration;
        import org.wpilib.epilogue.EpilogueService;
        import org.wpilib.framework.RobotBase;

        public final class Example_EpilogueService implements EpilogueService<Example> {
          @Override
          public boolean supportsExactly(RobotBase root) {
            return root != null && root.getClass().equals(org.wpilib.epilogue.Example.class);
          }

          @Override
          public void update(Example root) {
            long start = System.nanoTime();
            EpilogueConfiguration config = Epilogue.getConfig();
            org.wpilib.epilogue.generated.EpilogueLoggers.org_wpilib_epilogue_ExampleLogger.tryUpdate(config.table.getTable(config.root), root, config.errorHandler);
            config.table.log("Epilogue/Stats/Last Run", (System.nanoTime() - start) / 1e6);
          }
        }
        """;

    var compilation =
        compile(List.of(JavaFileObjects.forSourceString("org.wpilib.epilogue.Example", source)));
    assertGeneratedService(compilation, "Example_EpilogueService", expectedService);

    var classLoader = new CompilationClassLoader(compilation);
    var services = loadServicesAtRuntime(classLoader);
    assertEquals(1, services.size());

    var service = services.get(0);
    assertFalse(
        service instanceof EpilogueService.Bindable, "RobotBase service should not be Bindable");

    var robotClass = classLoader.loadClass("org.wpilib.epilogue.Example");
    var robot = allocateInstance(robotClass);

    assertTrue(supportsExactlyUnchecked(service, robot));
    assertFalse(supportsExactlyUnchecked(service, null));
    assertFalse(supportsExactlyUnchecked(service, new Object()));
  }

  @Test
  void nestedRobotBase() throws Exception {
    String source =
        """
          package org.wpilib.epilogue;

          public class Example {
            @Logged
            public static class Robot extends org.wpilib.framework.RobotBase {
              @Override
              public void startCompetition() {}
              @Override
              public void endCompetition() {}
            }
          }
          """;

    String expectedService =
        """
        package org.wpilib.epilogue;

        import org.wpilib.epilogue.Epilogue;
        import org.wpilib.epilogue.EpilogueConfiguration;
        import org.wpilib.epilogue.EpilogueService;
        import org.wpilib.framework.RobotBase;

        public final class Example$Robot_EpilogueService implements EpilogueService<Example.Robot> {
          @Override
          public boolean supportsExactly(RobotBase root) {
            return root != null && root.getClass().equals(org.wpilib.epilogue.Example.Robot.class);
          }

          @Override
          public void update(Example.Robot root) {
            long start = System.nanoTime();
            EpilogueConfiguration config = Epilogue.getConfig();
            org.wpilib.epilogue.generated.EpilogueLoggers.org_wpilib_epilogue_Example$RobotLogger.tryUpdate(config.table.getTable(config.root), root, config.errorHandler);
            config.table.log("Epilogue/Stats/Last Run", (System.nanoTime() - start) / 1e6);
          }
        }
        """;
    var compilation =
        compile(List.of(JavaFileObjects.forSourceString("org.wpilib.epilogue.Example", source)));
    assertGeneratedService(compilation, "Example$Robot_EpilogueService", expectedService);

    var classLoader = new CompilationClassLoader(compilation);
    var services = loadServicesAtRuntime(classLoader);
    assertEquals(1, services.size());

    var service = services.get(0);
    assertFalse(
        service instanceof EpilogueService.Bindable, "RobotBase service should not be Bindable");

    var robotClass = classLoader.loadClass("org.wpilib.epilogue.Example$Robot");
    var robot = allocateInstance(robotClass);

    assertTrue(supportsExactlyUnchecked(service, robot));
    assertFalse(supportsExactlyUnchecked(service, null));
    assertFalse(supportsExactlyUnchecked(service, new Object()));
  }

  @Test
  void nestedTimedRobot() throws Exception {
    String source =
        """
          package org.wpilib.epilogue;

          public class Example {
            @Logged
            public static class Robot extends org.wpilib.framework.TimedRobot {
            }
          }
          """;

    String expectedService =
        """
        package org.wpilib.epilogue;

        import static org.wpilib.units.Units.Seconds;

        import org.wpilib.epilogue.Epilogue;
        import org.wpilib.epilogue.EpilogueConfiguration;
        import org.wpilib.epilogue.EpilogueService;
        import org.wpilib.framework.RobotBase;

        public final class Example$Robot_EpilogueService implements EpilogueService.Bindable<Example.Robot> {
          @Override
          public boolean supportsExactly(RobotBase root) {
            return root != null && root.getClass().equals(org.wpilib.epilogue.Example.Robot.class);
          }

          @Override
          public void update(Example.Robot root) {
            long start = System.nanoTime();
            EpilogueConfiguration config = Epilogue.getConfig();
            org.wpilib.epilogue.generated.EpilogueLoggers.org_wpilib_epilogue_Example$RobotLogger.tryUpdate(config.table.getTable(config.root), root, config.errorHandler);
            config.table.log("Epilogue/Stats/Last Run", (System.nanoTime() - start) / 1e6);
          }

          @Override
          public void bind(Example.Robot root) {
            EpilogueConfiguration config = Epilogue.getConfig();
            if (config.loggingPeriod == null) {
              config.loggingPeriod = Seconds.of(root.getPeriod());
            }
            if (config.loggingPeriodOffset == null) {
              config.loggingPeriodOffset = config.loggingPeriod.div(2);
            }

            root.addPeriodic(() -> {
              update(root);
            }, config.loggingPeriod.in(Seconds), config.loggingPeriodOffset.in(Seconds));
          }
        }
        """;

    var compilation =
        compile(List.of(JavaFileObjects.forSourceString("org.wpilib.epilogue.Example", source)));
    assertGeneratedService(compilation, "Example$Robot_EpilogueService", expectedService);

    var classLoader = new CompilationClassLoader(compilation);
    var services = loadServicesAtRuntime(classLoader);
    assertEquals(1, services.size());

    var service = services.get(0);
    assertInstanceOf(EpilogueService.Bindable.class, service);

    var robotClass = classLoader.loadClass("org.wpilib.epilogue.Example$Robot");
    var robot = allocateInstance(robotClass);

    assertTrue(supportsExactlyUnchecked(service, robot));
    assertFalse(supportsExactlyUnchecked(service, null));
    assertFalse(supportsExactlyUnchecked(service, "other"));
  }

  @Test
  void timedRobot() throws Exception {
    String source =
        """
          package org.wpilib.epilogue;

          @Logged
          public class Example extends org.wpilib.framework.TimedRobot {
          }
          """;

    String expectedService =
        """
        package org.wpilib.epilogue;

        import static org.wpilib.units.Units.Seconds;

        import org.wpilib.epilogue.Epilogue;
        import org.wpilib.epilogue.EpilogueConfiguration;
        import org.wpilib.epilogue.EpilogueService;
        import org.wpilib.framework.RobotBase;

        public final class Example_EpilogueService implements EpilogueService.Bindable<Example> {
          @Override
          public boolean supportsExactly(RobotBase root) {
            return root != null && root.getClass().equals(org.wpilib.epilogue.Example.class);
          }

          @Override
          public void update(Example root) {
            long start = System.nanoTime();
            EpilogueConfiguration config = Epilogue.getConfig();
            org.wpilib.epilogue.generated.EpilogueLoggers.org_wpilib_epilogue_ExampleLogger.tryUpdate(config.table.getTable(config.root), root, config.errorHandler);
            config.table.log("Epilogue/Stats/Last Run", (System.nanoTime() - start) / 1e6);
          }

          @Override
          public void bind(Example root) {
            EpilogueConfiguration config = Epilogue.getConfig();
            if (config.loggingPeriod == null) {
              config.loggingPeriod = Seconds.of(root.getPeriod());
            }
            if (config.loggingPeriodOffset == null) {
              config.loggingPeriodOffset = config.loggingPeriod.div(2);
            }

            root.addPeriodic(() -> {
              update(root);
            }, config.loggingPeriod.in(Seconds), config.loggingPeriodOffset.in(Seconds));
          }
        }
        """;

    var compilation =
        compile(List.of(JavaFileObjects.forSourceString("org.wpilib.epilogue.Example", source)));
    assertGeneratedService(compilation, "Example_EpilogueService", expectedService);

    var classLoader = new CompilationClassLoader(compilation);
    var services = loadServicesAtRuntime(classLoader);
    assertEquals(1, services.size());

    var service = services.get(0);
    assertInstanceOf(EpilogueService.Bindable.class, service);

    var robotClass = classLoader.loadClass("org.wpilib.epilogue.Example");
    var robot = allocateInstance(robotClass);

    assertTrue(supportsExactlyUnchecked(service, robot));
    assertFalse(supportsExactlyUnchecked(service, null));
    assertFalse(supportsExactlyUnchecked(service, "other"));
  }

  @Test
  void multipleRobots() throws Exception {
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

    String expectedAlphaService =
        """
        package org.wpilib.epilogue;

        import static org.wpilib.units.Units.Seconds;

        import org.wpilib.epilogue.Epilogue;
        import org.wpilib.epilogue.EpilogueConfiguration;
        import org.wpilib.epilogue.EpilogueService;
        import org.wpilib.framework.RobotBase;

        public final class AlphaBot_EpilogueService implements EpilogueService.Bindable<AlphaBot> {
          @Override
          public boolean supportsExactly(RobotBase root) {
            return root != null && root.getClass().equals(org.wpilib.epilogue.AlphaBot.class);
          }

          @Override
          public void update(AlphaBot root) {
            long start = System.nanoTime();
            EpilogueConfiguration config = Epilogue.getConfig();
            org.wpilib.epilogue.generated.EpilogueLoggers.org_wpilib_epilogue_AlphaBotLogger.tryUpdate(config.table.getTable(config.root), root, config.errorHandler);
            config.table.log("Epilogue/Stats/Last Run", (System.nanoTime() - start) / 1e6);
          }

          @Override
          public void bind(AlphaBot root) {
            EpilogueConfiguration config = Epilogue.getConfig();
            if (config.loggingPeriod == null) {
              config.loggingPeriod = Seconds.of(root.getPeriod());
            }
            if (config.loggingPeriodOffset == null) {
              config.loggingPeriodOffset = config.loggingPeriod.div(2);
            }

            root.addPeriodic(() -> {
              update(root);
            }, config.loggingPeriod.in(Seconds), config.loggingPeriodOffset.in(Seconds));
          }
        }
        """;

    String expectedBetaService =
        """
        package org.wpilib.epilogue;

        import static org.wpilib.units.Units.Seconds;

        import org.wpilib.epilogue.Epilogue;
        import org.wpilib.epilogue.EpilogueConfiguration;
        import org.wpilib.epilogue.EpilogueService;
        import org.wpilib.framework.RobotBase;

        public final class BetaBot_EpilogueService implements EpilogueService.Bindable<BetaBot> {
          @Override
          public boolean supportsExactly(RobotBase root) {
            return root != null && root.getClass().equals(org.wpilib.epilogue.BetaBot.class);
          }

          @Override
          public void update(BetaBot root) {
            long start = System.nanoTime();
            EpilogueConfiguration config = Epilogue.getConfig();
            org.wpilib.epilogue.generated.EpilogueLoggers.org_wpilib_epilogue_BetaBotLogger.tryUpdate(config.table.getTable(config.root), root, config.errorHandler);
            config.table.log("Epilogue/Stats/Last Run", (System.nanoTime() - start) / 1e6);
          }

          @Override
          public void bind(BetaBot root) {
            EpilogueConfiguration config = Epilogue.getConfig();
            if (config.loggingPeriod == null) {
              config.loggingPeriod = Seconds.of(root.getPeriod());
            }
            if (config.loggingPeriodOffset == null) {
              config.loggingPeriodOffset = config.loggingPeriod.div(2);
            }

            root.addPeriodic(() -> {
              update(root);
            }, config.loggingPeriod.in(Seconds), config.loggingPeriodOffset.in(Seconds));
          }
        }
        """;

    var compilation =
        compile(
            List.of(
                JavaFileObjects.forSourceString("org.wpilib.epilogue.AlphaBot", alphaSource),
                JavaFileObjects.forSourceString("org.wpilib.epilogue.BetaBot", betaSource)));

    assertGeneratedService(compilation, "AlphaBot_EpilogueService", expectedAlphaService);
    assertGeneratedService(compilation, "BetaBot_EpilogueService", expectedBetaService);

    var classLoader = new CompilationClassLoader(compilation);
    var services = loadServicesAtRuntime(classLoader);
    assertEquals(2, services.size());

    var alphaClass = classLoader.loadClass("org.wpilib.epilogue.AlphaBot");
    var betaClass = classLoader.loadClass("org.wpilib.epilogue.BetaBot");
    var alphaBot = allocateInstance(alphaClass);
    var betaBot = allocateInstance(betaClass);

    var alphaService =
        services.stream()
            .filter(s -> supportsExactlyUnchecked(s, alphaBot))
            .findFirst()
            .orElseThrow();
    var betaService =
        services.stream()
            .filter(s -> supportsExactlyUnchecked(s, betaBot))
            .findFirst()
            .orElseThrow();

    assertFalse(supportsExactlyUnchecked(alphaService, betaBot));
    assertFalse(supportsExactlyUnchecked(betaService, alphaBot));
  }

  @Test
  void customPackageRobot() throws Exception {
    String source =
        """
          package frc.robot;

          import org.wpilib.epilogue.Logged;

          @Logged
          public class MyBot extends org.wpilib.framework.TimedRobot {}
          """;

    String expectedService =
        """
        package frc.robot;

        import static org.wpilib.units.Units.Seconds;

        import org.wpilib.epilogue.Epilogue;
        import org.wpilib.epilogue.EpilogueConfiguration;
        import org.wpilib.epilogue.EpilogueService;
        import org.wpilib.framework.RobotBase;

        public final class MyBot_EpilogueService implements EpilogueService.Bindable<MyBot> {
          @Override
          public boolean supportsExactly(RobotBase root) {
            return root != null && root.getClass().equals(frc.robot.MyBot.class);
          }

          @Override
          public void update(MyBot root) {
            long start = System.nanoTime();
            EpilogueConfiguration config = Epilogue.getConfig();
            org.wpilib.epilogue.generated.EpilogueLoggers.frc_robot_MyBotLogger.tryUpdate(config.table.getTable(config.root), root, config.errorHandler);
            config.table.log("Epilogue/Stats/Last Run", (System.nanoTime() - start) / 1e6);
          }

          @Override
          public void bind(MyBot root) {
            EpilogueConfiguration config = Epilogue.getConfig();
            if (config.loggingPeriod == null) {
              config.loggingPeriod = Seconds.of(root.getPeriod());
            }
            if (config.loggingPeriodOffset == null) {
              config.loggingPeriodOffset = config.loggingPeriod.div(2);
            }

            root.addPeriodic(() -> {
              update(root);
            }, config.loggingPeriod.in(Seconds), config.loggingPeriodOffset.in(Seconds));
          }
        }
        """;

    var compilation = compile(List.of(JavaFileObjects.forSourceString("frc.robot.MyBot", source)));
    assertGeneratedService(compilation, "MyBot_EpilogueService", expectedService);

    var classLoader = new CompilationClassLoader(compilation);
    var services = loadServicesAtRuntime(classLoader);
    assertEquals(1, services.size());

    var service = services.get(0);
    var myBotClass = classLoader.loadClass("frc.robot.MyBot");
    var myBot = allocateInstance(myBotClass);

    assertTrue(supportsExactlyUnchecked(service, myBot));
  }

  @Test
  void mixedRobotTypes() throws Exception {
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

    assertGeneratedService(
        compilation,
        "BaseRobot_EpilogueService",
        """
        package org.wpilib.epilogue;

        import org.wpilib.epilogue.Epilogue;
        import org.wpilib.epilogue.EpilogueConfiguration;
        import org.wpilib.epilogue.EpilogueService;
        import org.wpilib.framework.RobotBase;

        public final class BaseRobot_EpilogueService implements EpilogueService<BaseRobot> {
          @Override
          public boolean supportsExactly(RobotBase root) {
            return root != null && root.getClass().equals(org.wpilib.epilogue.BaseRobot.class);
          }

          @Override
          public void update(BaseRobot root) {
            long start = System.nanoTime();
            EpilogueConfiguration config = Epilogue.getConfig();
            org.wpilib.epilogue.generated.EpilogueLoggers.org_wpilib_epilogue_BaseRobotLogger.tryUpdate(config.table.getTable(config.root), root, config.errorHandler);
            config.table.log("Epilogue/Stats/Last Run", (System.nanoTime() - start) / 1e6);
          }
        }
        """);

    assertGeneratedService(
        compilation,
        "TimedRobotBot_EpilogueService",
        """
        package org.wpilib.epilogue;

        import static org.wpilib.units.Units.Seconds;

        import org.wpilib.epilogue.Epilogue;
        import org.wpilib.epilogue.EpilogueConfiguration;
        import org.wpilib.epilogue.EpilogueService;
        import org.wpilib.framework.RobotBase;

        public final class TimedRobotBot_EpilogueService implements EpilogueService.Bindable<TimedRobotBot> {
          @Override
          public boolean supportsExactly(RobotBase root) {
            return root != null && root.getClass().equals(org.wpilib.epilogue.TimedRobotBot.class);
          }

          @Override
          public void update(TimedRobotBot root) {
            long start = System.nanoTime();
            EpilogueConfiguration config = Epilogue.getConfig();
            org.wpilib.epilogue.generated.EpilogueLoggers.org_wpilib_epilogue_TimedRobotBotLogger.tryUpdate(config.table.getTable(config.root), root, config.errorHandler);
            config.table.log("Epilogue/Stats/Last Run", (System.nanoTime() - start) / 1e6);
          }

          @Override
          public void bind(TimedRobotBot root) {
            EpilogueConfiguration config = Epilogue.getConfig();
            if (config.loggingPeriod == null) {
              config.loggingPeriod = Seconds.of(root.getPeriod());
            }
            if (config.loggingPeriodOffset == null) {
              config.loggingPeriodOffset = config.loggingPeriod.div(2);
            }

            root.addPeriodic(() -> {
              update(root);
            }, config.loggingPeriod.in(Seconds), config.loggingPeriodOffset.in(Seconds));
          }
        }
        """);

    var classLoader = new CompilationClassLoader(compilation);
    var services = loadServicesAtRuntime(classLoader);
    assertEquals(2, services.size());

    var baseClass = classLoader.loadClass("org.wpilib.epilogue.BaseRobot");
    var timedClass = classLoader.loadClass("org.wpilib.epilogue.TimedRobotBot");
    var baseBot = allocateInstance(baseClass);
    var timedBot = allocateInstance(timedClass);

    var baseService =
        services.stream()
            .filter(s -> supportsExactlyUnchecked(s, baseBot))
            .findFirst()
            .orElseThrow();
    var timedService =
        services.stream()
            .filter(s -> supportsExactlyUnchecked(s, timedBot))
            .findFirst()
            .orElseThrow();

    assertFalse(baseService instanceof EpilogueService.Bindable);
    assertInstanceOf(EpilogueService.Bindable.class, timedService);
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

  private void assertGeneratedService(
      Compilation compilation, String serviceSimpleClassName, String expectedContent) {
    var generatedFile =
        compilation.generatedSourceFiles().stream()
            .filter(jfo -> jfo.getName().contains(serviceSimpleClassName))
            .findFirst()
            .orElseThrow(
                () ->
                    new IllegalStateException(
                        serviceSimpleClassName + " service file was not generated!"));
    try {
      var content = generatedFile.getCharContent(false);
      assertEquals(
          expectedContent.replace("\r\n", "\n").strip(),
          content.toString().replace("\r\n", "\n").strip());
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
  }

  private void assertNoServiceGenerated(Compilation compilation, String serviceSimpleClassName) {
    boolean present =
        compilation.generatedSourceFiles().stream()
            .anyMatch(jfo -> jfo.getName().contains(serviceSimpleClassName));
    assertFalse(present, serviceSimpleClassName + " should not be generated");
  }

  @SuppressWarnings("rawtypes")
  private List<EpilogueService> loadServicesAtRuntime(Compilation compilation) {
    return loadServicesAtRuntime(new CompilationClassLoader(compilation));
  }

  @SuppressWarnings("rawtypes")
  private List<EpilogueService> loadServicesAtRuntime(ClassLoader classLoader) {
    var loader = ServiceLoader.load(EpilogueService.class, classLoader);
    List<EpilogueService> services = new ArrayList<>();
    for (EpilogueService service : loader) {
      services.add(service);
    }
    return services;
  }

  @SuppressWarnings({"unchecked", "rawtypes"})
  private static boolean supportsExactlyUnchecked(EpilogueService service, Object robot) {
    return robot instanceof org.wpilib.framework.RobotBase robotBase
        && service.supportsExactly(robotBase);
  }

  private static Object allocateInstance(Class<?> clazz) throws Exception {
    // Needed to avoid actually running the constructor and initializers, which will attempt to
    // load JNI libraries.
    Class<?> unsafeClass = Class.forName("sun.misc.Unsafe");
    var field = unsafeClass.getDeclaredField("theUnsafe");
    field.setAccessible(true);
    Object unsafe = field.get(null);
    var method = unsafeClass.getMethod("allocateInstance", Class.class);
    return method.invoke(unsafe, clazz);
  }
}
