// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.epilogue.processor;

import static com.google.testing.compile.CompilationSubject.assertThat;
import static com.google.testing.compile.Compiler.javac;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.wpilib.epilogue.processor.CompileTestOptions.kJavaVersionOptions;

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
          package org.wpilib.epilogue;

          @Logged
          class Example {
          }
          """;

    String expected =
        """
        package org.wpilib.epilogue;

        import static org.wpilib.units.Units.Seconds;

        import org.wpilib.hardware.hal.HAL;

        import org.wpilib.epilogue.ExampleLogger;

        public final class Epilogue {
          static {
            HAL.reportUsage("Epilogue", "");
          }

          private static final EpilogueConfiguration config = new EpilogueConfiguration();

          public static final org.wpilib.epilogue.ExampleLogger org_wpilib_epilogue_ExampleLogger = new org.wpilib.epilogue.ExampleLogger();

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
          package org.wpilib.epilogue;

          @Logged
          class Example extends org.wpilib.framework.RobotBase {
            @Override
            public void startCompetition() {}
            @Override
            public void endCompetition() {}
          }
          """;

    String expected =
        """
        package org.wpilib.epilogue;

        import static org.wpilib.units.Units.Seconds;

        import org.wpilib.hardware.hal.HAL;

        import org.wpilib.epilogue.ExampleLogger;

        public final class Epilogue {
          static {
            HAL.reportUsage("Epilogue", "");
          }

          private static final EpilogueConfiguration config = new EpilogueConfiguration();

          public static final org.wpilib.epilogue.ExampleLogger org_wpilib_epilogue_ExampleLogger = new org.wpilib.epilogue.ExampleLogger();

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
          package org.wpilib.epilogue;

          @Logged
          class Example extends org.wpilib.framework.TimedRobot {
          }
          """;

    String expected =
        """
        package org.wpilib.epilogue;

        import static org.wpilib.units.Units.Seconds;

        import org.wpilib.hardware.hal.HAL;

        import org.wpilib.epilogue.ExampleLogger;

        public final class Epilogue {
          static {
            HAL.reportUsage("Epilogue", "");
          }

          private static final EpilogueConfiguration config = new EpilogueConfiguration();

          public static final org.wpilib.epilogue.ExampleLogger org_wpilib_epilogue_ExampleLogger = new org.wpilib.epilogue.ExampleLogger();

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
          public static void update(org.wpilib.epilogue.Example robot) {
            long start = System.nanoTime();
            org_wpilib_epilogue_ExampleLogger.tryUpdate(config.backend.getNested(config.root), robot, config.errorHandler);
            config.backend.log("Epilogue/Stats/Last Run", (System.nanoTime() - start) / 1e6);
          }

          /**
           * Binds Epilogue updates to a timed robot's update period. Log calls will be made at the
           * same update rate as the robot's loop function, but will be offset by a full phase
           * (for example, a 20ms update rate but 10ms offset from the main loop invocation) to
           * help avoid high CPU loads. However, this does mean that any logged data that reads
           * directly from sensors will be slightly different from data used in the main robot
           * loop.
           */
          public static void bind(org.wpilib.epilogue.Example robot) {
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
          package org.wpilib.epilogue;

          @Logged
          class AlphaBot extends org.wpilib.framework.TimedRobot { }

          @Logged
          class BetaBot extends org.wpilib.framework.TimedRobot { }
          """;

    String expected =
        """
        package org.wpilib.epilogue;

        import static org.wpilib.units.Units.Seconds;

        import org.wpilib.hardware.hal.HAL;

        import org.wpilib.epilogue.AlphaBotLogger;
        import org.wpilib.epilogue.BetaBotLogger;

        public final class Epilogue {
          static {
            HAL.reportUsage("Epilogue", "");
          }

          private static final EpilogueConfiguration config = new EpilogueConfiguration();

          public static final org.wpilib.epilogue.AlphaBotLogger org_wpilib_epilogue_AlphaBotLogger = new org.wpilib.epilogue.AlphaBotLogger();
          public static final org.wpilib.epilogue.BetaBotLogger org_wpilib_epilogue_BetaBotLogger = new org.wpilib.epilogue.BetaBotLogger();

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
          public static void update(org.wpilib.epilogue.AlphaBot robot) {
            long start = System.nanoTime();
            org_wpilib_epilogue_AlphaBotLogger.tryUpdate(config.backend.getNested(config.root), robot, config.errorHandler);
            config.backend.log("Epilogue/Stats/Last Run", (System.nanoTime() - start) / 1e6);
          }

          /**
           * Binds Epilogue updates to a timed robot's update period. Log calls will be made at the
           * same update rate as the robot's loop function, but will be offset by a full phase
           * (for example, a 20ms update rate but 10ms offset from the main loop invocation) to
           * help avoid high CPU loads. However, this does mean that any logged data that reads
           * directly from sensors will be slightly different from data used in the main robot
           * loop.
           */
          public static void bind(org.wpilib.epilogue.AlphaBot robot) {
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
          public static void update(org.wpilib.epilogue.BetaBot robot) {
            long start = System.nanoTime();
            org_wpilib_epilogue_BetaBotLogger.tryUpdate(config.backend.getNested(config.root), robot, config.errorHandler);
            config.backend.log("Epilogue/Stats/Last Run", (System.nanoTime() - start) / 1e6);
          }

          /**
           * Binds Epilogue updates to a timed robot's update period. Log calls will be made at the
           * same update rate as the robot's loop function, but will be offset by a full phase
           * (for example, a 20ms update rate but 10ms offset from the main loop invocation) to
           * help avoid high CPU loads. However, this does mean that any logged data that reads
           * directly from sensors will be slightly different from data used in the main robot
           * loop.
           */
          public static void bind(org.wpilib.epilogue.BetaBot robot) {
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
        package org.wpilib.epilogue;

        import org.wpilib.epilogue.logging.*;

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
        package org.wpilib.epilogue;

        import static org.wpilib.units.Units.Seconds;

        import org.wpilib.hardware.hal.HAL;

        import org.wpilib.epilogue.ExampleLogger;
        import org.wpilib.epilogue.CustomLogger;

        public final class Epilogue {
          static {
            HAL.reportUsage("Epilogue", "");
          }

          private static final EpilogueConfiguration config = new EpilogueConfiguration();

          public static final org.wpilib.epilogue.ExampleLogger org_wpilib_epilogue_ExampleLogger = new org.wpilib.epilogue.ExampleLogger();
          public static final org.wpilib.epilogue.CustomLogger org_wpilib_epilogue_CustomLogger = new org.wpilib.epilogue.CustomLogger();

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
  void commandsv3Scheduler() {
    String schedulerSource =
        """
        package org.wpilib.command3;

        import org.wpilib.util.protobuf.*;
        import us.hebi.quickbuf.*;

        // Stub the scheduler and its protobuf logging so the shape is correct at compile time.
        // We don't care about runtime behavior because we are only testing the contents of the
        // generated code.
        public interface Scheduler extends ProtobufSerializable {
          static class ProtoScheduler extends ProtoMessage<ProtoScheduler> implements Cloneable {
            @Override public ProtoScheduler clone() { return null; }
            @Override public boolean equals(Object other) { return false; }
            @Override public ProtoScheduler copyFrom(ProtoScheduler other) { return null; }
            @Override public ProtoScheduler mergeFrom(ProtoSource other) { return null; }
            @Override public ProtoScheduler clear() { return null; }
            @Override public int computeSerializedSize() { return 0; }
            @Override public void writeTo(ProtoSink output) {}
          }

          static Protobuf<Scheduler, ProtoScheduler> proto = null;

          static Scheduler getDefault() {
            return null;
          }
        }
        """;

    String robotSource =
        """
        package org.wpilib.epilogue;

        @Logged
        public class Robot extends org.wpilib.framework.TimedRobot {}
        """;

    Compilation compilation =
        javac()
            .withOptions(kJavaVersionOptions)
            .withProcessors(new AnnotationProcessor())
            .compile(
                JavaFileObjects.forSourceString("org.wpilib.epilogue.Robot", robotSource),
                JavaFileObjects.forSourceString("org.wpilib.command3.Scheduler", schedulerSource));

    assertThat(compilation).succeededWithoutWarnings();
    var generatedFiles = compilation.generatedSourceFiles();
    var epilogueFile =
        generatedFiles.stream()
            .filter(jfo -> jfo.getName().contains("Epilogue"))
            .findFirst()
            .orElseThrow(() -> new IllegalStateException("Epilogue file was not generated!"));

    try {
      var content = epilogueFile.getCharContent(false);

      assertTrue(
          content
              .toString()
              .contains(
                  """
              if (config.automaticallyLogCommandScheduler) {
                config.backend.getNested(config.root).log("Command Scheduler", org.wpilib.command3.Scheduler.getDefault(), org.wpilib.command3.Scheduler.proto);
              }
          """),
          "Generated file did not contain the expected scheduler logging code:\n\n" + content);
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
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
