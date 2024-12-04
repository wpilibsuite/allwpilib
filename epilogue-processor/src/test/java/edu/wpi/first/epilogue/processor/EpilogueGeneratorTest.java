// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.processor;

import static com.google.testing.compile.CompilationSubject.assertThat;
import static com.google.testing.compile.Compiler.javac;
import static edu.wpi.first.epilogue.processor.CompileTestOptions.kJavaVersionOptions;
import static org.junit.jupiter.api.Assertions.assertEquals;

import com.google.testing.compile.Compilation;
import com.google.testing.compile.JavaFileObjects;
import java.io.IOException;
import org.junit.jupiter.api.Test;

@SuppressWarnings("checkstyle:LineLength") // Source code templates exceed the line length limit
class EpilogueGeneratorTest {
  @Test
  void noFields() {
    String source =
        """
          package edu.wpi.first.epilogue;

          @Logged
          class Example {
          }
          """;

    String expected =
        """
        package edu.wpi.first.epilogue;

        import static edu.wpi.first.units.Units.Seconds;

        import edu.wpi.first.hal.FRCNetComm;
        import edu.wpi.first.hal.HAL;

        import edu.wpi.first.epilogue.ExampleLogger;

        public final class Epilogue {
          static {
            HAL.report(
              FRCNetComm.tResourceType.kResourceType_LoggingFramework,
              FRCNetComm.tInstances.kLoggingFramework_Epilogue
            );
          }

          private static final EpilogueConfiguration config = new EpilogueConfiguration();

          public static final ExampleLogger exampleLogger = new ExampleLogger();

          public static void configure(java.util.function.Consumer<EpilogueConfiguration> configurator) {
            configurator.accept(config);
          }

          public static EpilogueConfiguration getConfig() {
            return config;
          }

          /**
           * Checks if data associated with a given importance level should be logged.
           */
          public static boolean shouldLog(Logged.Importance importance) {
            return importance.compareTo(config.minimumImportance) >= 0;
          }
        }
        """;

    assertGeneratedEpilogueContents(source, expected);
  }

  /** Subclassing RobotBase should not generate the bind() method because it lacks addPeriodic(). */
  @Test
  void robotBase() {
    String source =
        """
          package edu.wpi.first.epilogue;

          @Logged
          class Example extends edu.wpi.first.wpilibj.RobotBase {
            @Override
            public void startCompetition() {}
            @Override
            public void endCompetition() {}
          }
          """;

    String expected =
        """
        package edu.wpi.first.epilogue;

        import static edu.wpi.first.units.Units.Seconds;

        import edu.wpi.first.hal.FRCNetComm;
        import edu.wpi.first.hal.HAL;

        import edu.wpi.first.epilogue.ExampleLogger;

        public final class Epilogue {
          static {
            HAL.report(
              FRCNetComm.tResourceType.kResourceType_LoggingFramework,
              FRCNetComm.tInstances.kLoggingFramework_Epilogue
            );
          }

          private static final EpilogueConfiguration config = new EpilogueConfiguration();

          public static final ExampleLogger exampleLogger = new ExampleLogger();

          public static void configure(java.util.function.Consumer<EpilogueConfiguration> configurator) {
            configurator.accept(config);
          }

          public static EpilogueConfiguration getConfig() {
            return config;
          }

          /**
           * Checks if data associated with a given importance level should be logged.
           */
          public static boolean shouldLog(Logged.Importance importance) {
            return importance.compareTo(config.minimumImportance) >= 0;
          }
        }
        """;

    assertGeneratedEpilogueContents(source, expected);
  }

  @Test
  void timedRobot() {
    String source =
        """
          package edu.wpi.first.epilogue;

          @Logged
          class Example extends edu.wpi.first.wpilibj.TimedRobot {
          }
          """;

    String expected =
        """
        package edu.wpi.first.epilogue;

        import static edu.wpi.first.units.Units.Seconds;

        import edu.wpi.first.hal.FRCNetComm;
        import edu.wpi.first.hal.HAL;

        import edu.wpi.first.epilogue.ExampleLogger;

        public final class Epilogue {
          static {
            HAL.report(
              FRCNetComm.tResourceType.kResourceType_LoggingFramework,
              FRCNetComm.tInstances.kLoggingFramework_Epilogue
            );
          }

          private static final EpilogueConfiguration config = new EpilogueConfiguration();

          public static final ExampleLogger exampleLogger = new ExampleLogger();

          public static void configure(java.util.function.Consumer<EpilogueConfiguration> configurator) {
            configurator.accept(config);
          }

          public static EpilogueConfiguration getConfig() {
            return config;
          }

          /**
           * Checks if data associated with a given importance level should be logged.
           */
          public static boolean shouldLog(Logged.Importance importance) {
            return importance.compareTo(config.minimumImportance) >= 0;
          }

          /**
           * Updates Epilogue. This must be called periodically in order for Epilogue to record
           * new values. Alternatively, {@code bind()} can be used to update at an offset from
           * the main robot loop.
           */
          public static void update(edu.wpi.first.epilogue.Example robot) {
            long start = System.nanoTime();
            exampleLogger.tryUpdate(config.backend.getNested(config.root), robot, config.errorHandler);
            config.backend.log(\"Epilogue/Stats/Last Run\", (System.nanoTime() - start) / 1e6);
          }

          /**
           * Binds Epilogue updates to a timed robot's update period. Log calls will be made at the
           * same update rate as the robot's loop function, but will be offset by a full phase
           * (for example, a 20ms update rate but 10ms offset from the main loop invocation) to
           * help avoid high CPU loads. However, this does mean that any logged data that reads
           * directly from sensors will be slightly different from data used in the main robot
           * loop.
           */
          public static void bind(edu.wpi.first.epilogue.Example robot) {
            if (config.loggingPeriod == null) {
              config.loggingPeriod = Seconds.of(robot.getPeriod());
            }
            if (config.loggingPeriodOffset == null) {
              config.loggingPeriodOffset = config.loggingPeriod.div(2);
            }

            robot.addPeriodic(() -> {
              update(robot);
            }, config.loggingPeriod.in(Seconds), config.loggingPeriodOffset.in(Seconds));
          }
        }
        """;

    assertGeneratedEpilogueContents(source, expected);
  }

  @Test
  void multipleRobots() {
    String source =
        """
          package edu.wpi.first.epilogue;

          @Logged
          class AlphaBot extends edu.wpi.first.wpilibj.TimedRobot { }

          @Logged
          class BetaBot extends edu.wpi.first.wpilibj.TimedRobot { }
          """;

    String expected =
        """
        package edu.wpi.first.epilogue;

        import static edu.wpi.first.units.Units.Seconds;

        import edu.wpi.first.hal.FRCNetComm;
        import edu.wpi.first.hal.HAL;

        import edu.wpi.first.epilogue.AlphaBotLogger;
        import edu.wpi.first.epilogue.BetaBotLogger;

        public final class Epilogue {
          static {
            HAL.report(
              FRCNetComm.tResourceType.kResourceType_LoggingFramework,
              FRCNetComm.tInstances.kLoggingFramework_Epilogue
            );
          }

          private static final EpilogueConfiguration config = new EpilogueConfiguration();

          public static final AlphaBotLogger alphaBotLogger = new AlphaBotLogger();
          public static final BetaBotLogger betaBotLogger = new BetaBotLogger();

          public static void configure(java.util.function.Consumer<EpilogueConfiguration> configurator) {
            configurator.accept(config);
          }

          public static EpilogueConfiguration getConfig() {
            return config;
          }

          /**
           * Checks if data associated with a given importance level should be logged.
           */
          public static boolean shouldLog(Logged.Importance importance) {
            return importance.compareTo(config.minimumImportance) >= 0;
          }

          /**
           * Updates Epilogue. This must be called periodically in order for Epilogue to record
           * new values. Alternatively, {@code bind()} can be used to update at an offset from
           * the main robot loop.
           */
          public static void update(edu.wpi.first.epilogue.AlphaBot robot) {
            long start = System.nanoTime();
            alphaBotLogger.tryUpdate(config.backend.getNested(config.root), robot, config.errorHandler);
            config.backend.log(\"Epilogue/Stats/Last Run\", (System.nanoTime() - start) / 1e6);
          }

          /**
           * Binds Epilogue updates to a timed robot's update period. Log calls will be made at the
           * same update rate as the robot's loop function, but will be offset by a full phase
           * (for example, a 20ms update rate but 10ms offset from the main loop invocation) to
           * help avoid high CPU loads. However, this does mean that any logged data that reads
           * directly from sensors will be slightly different from data used in the main robot
           * loop.
           */
          public static void bind(edu.wpi.first.epilogue.AlphaBot robot) {
            if (config.loggingPeriod == null) {
              config.loggingPeriod = Seconds.of(robot.getPeriod());
            }
            if (config.loggingPeriodOffset == null) {
              config.loggingPeriodOffset = config.loggingPeriod.div(2);
            }

            robot.addPeriodic(() -> {
              update(robot);
            }, config.loggingPeriod.in(Seconds), config.loggingPeriodOffset.in(Seconds));
          }

          /**
           * Updates Epilogue. This must be called periodically in order for Epilogue to record
           * new values. Alternatively, {@code bind()} can be used to update at an offset from
           * the main robot loop.
           */
          public static void update(edu.wpi.first.epilogue.BetaBot robot) {
            long start = System.nanoTime();
            betaBotLogger.tryUpdate(config.backend.getNested(config.root), robot, config.errorHandler);
            config.backend.log(\"Epilogue/Stats/Last Run\", (System.nanoTime() - start) / 1e6);
          }

          /**
           * Binds Epilogue updates to a timed robot's update period. Log calls will be made at the
           * same update rate as the robot's loop function, but will be offset by a full phase
           * (for example, a 20ms update rate but 10ms offset from the main loop invocation) to
           * help avoid high CPU loads. However, this does mean that any logged data that reads
           * directly from sensors will be slightly different from data used in the main robot
           * loop.
           */
          public static void bind(edu.wpi.first.epilogue.BetaBot robot) {
            if (config.loggingPeriod == null) {
              config.loggingPeriod = Seconds.of(robot.getPeriod());
            }
            if (config.loggingPeriodOffset == null) {
              config.loggingPeriodOffset = config.loggingPeriod.div(2);
            }

            robot.addPeriodic(() -> {
              update(robot);
            }, config.loggingPeriod.in(Seconds), config.loggingPeriodOffset.in(Seconds));
          }
        }
        """;

    assertGeneratedEpilogueContents(source, expected);
  }

  @Test
  void genericCustomLogger() {
    String source =
        """
        package edu.wpi.first.epilogue;

        import edu.wpi.first.epilogue.logging.*;

        class A {}
        class B extends A {}
        class C extends A {}

        @CustomLoggerFor({A.class, B.class, C.class})
        class CustomLogger extends ClassSpecificLogger<A> {
          public CustomLogger() { super(A.class); }

          @Override
          public void update(EpilogueBackend backend, A object) {} // implementation is irrelevant
        }

        @Logged
        class Example {
          A a_b_or_c;
          B b;
          C c;
        }
        """;

    String expected =
        """
        package edu.wpi.first.epilogue;

        import static edu.wpi.first.units.Units.Seconds;

        import edu.wpi.first.hal.FRCNetComm;
        import edu.wpi.first.hal.HAL;

        import edu.wpi.first.epilogue.ExampleLogger;
        import edu.wpi.first.epilogue.CustomLogger;

        public final class Epilogue {
          static {
            HAL.report(
              FRCNetComm.tResourceType.kResourceType_LoggingFramework,
              FRCNetComm.tInstances.kLoggingFramework_Epilogue
            );
          }

          private static final EpilogueConfiguration config = new EpilogueConfiguration();

          public static final ExampleLogger exampleLogger = new ExampleLogger();
          public static final CustomLogger customLogger = new CustomLogger();

          public static void configure(java.util.function.Consumer<EpilogueConfiguration> configurator) {
            configurator.accept(config);
          }

          public static EpilogueConfiguration getConfig() {
            return config;
          }

          /**
           * Checks if data associated with a given importance level should be logged.
           */
          public static boolean shouldLog(Logged.Importance importance) {
            return importance.compareTo(config.minimumImportance) >= 0;
          }
        }
        """;

    assertGeneratedEpilogueContents(source, expected);
  }

  private void assertGeneratedEpilogueContents(
      String loggedClassContent, String loggerClassContent) {
    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .withProcessors(new AnnotationProcessor())
            .compile(JavaFileObjects.forSourceString("", loggedClassContent));

    assertThat(compilation).succeededWithoutWarnings();
    var generatedFiles = compilation.generatedSourceFiles();
    var generatedFile =
        generatedFiles.stream()
            .filter(jfo -> jfo.getName().contains("Epilogue"))
            .findFirst()
            .orElseThrow(() -> new IllegalStateException("Epilogue file was not generated!"));
    try {
      var content = generatedFile.getCharContent(false);
      assertEquals(
          loggerClassContent.replace("\r\n", "\n"), content.toString().replace("\r\n", "\n"));
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
  }
}
