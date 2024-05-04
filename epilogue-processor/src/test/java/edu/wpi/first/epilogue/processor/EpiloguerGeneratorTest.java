package edu.wpi.first.epilogue.processor;

import static com.google.testing.compile.CompilationSubject.assertThat;
import static com.google.testing.compile.Compiler.javac;
import static org.junit.jupiter.api.Assertions.assertEquals;

import com.google.testing.compile.Compilation;
import com.google.testing.compile.JavaFileObjects;
import java.io.IOException;
import org.junit.jupiter.api.Test;

@SuppressWarnings("checkstyle:LineLength") // Source code templates exceed the line length limit
class EpiloguerGeneratorTest {
  @Test
  void noFields() {
    String source =
        """
          package edu.wpi.first.epilogue;

          @Epilogue
          class HelloWorld {
          }
          """;

    String expected =
        """
        package edu.wpi.first.epilogue;

        import edu.wpi.first.epilogue.HelloWorldLogger;

        public final class Epiloguer {
          private static final EpilogueConfiguration config = new EpilogueConfiguration();

          public static final HelloWorldLogger helloWorldLogger = new HelloWorldLogger();

          public static void configure(java.util.function.Consumer<EpilogueConfiguration> configurator) {
            configurator.accept(config);
          }

          public static EpilogueConfiguration getConfig() {
            return config;
          }

          /**
           * Checks if data associated with a given importance level should be logged.
           */
          public static boolean shouldLog(Epilogue.Importance importance) {
            return importance.compareTo(config.minimumImportance) >= 0;
          }
        }
        """;

    assertGeneratedEpiloguerContents(source, expected);
  }

  @Test
  void timedRobot() {
    String source =
        """
          package edu.wpi.first.epilogue;

          @Epilogue
          class HelloWorld extends edu.wpi.first.wpilibj.TimedRobot {
          }
          """;

    String expected =
        """
        package edu.wpi.first.epilogue;

        import edu.wpi.first.epilogue.HelloWorldLogger;

        public final class Epiloguer {
          private static final EpilogueConfiguration config = new EpilogueConfiguration();

          public static final HelloWorldLogger helloWorldLogger = new HelloWorldLogger();

          public static void configure(java.util.function.Consumer<EpilogueConfiguration> configurator) {
            configurator.accept(config);
          }

          public static EpilogueConfiguration getConfig() {
            return config;
          }

          /**
           * Checks if data associated with a given importance level should be logged.
           */
          public static boolean shouldLog(Epilogue.Importance importance) {
            return importance.compareTo(config.minimumImportance) >= 0;
          }

          /**
           * Binds Epilogue updates to a timed robot's update period. Log calls will be made at the
           * same update rate as the robot's loop function, but will be offset by a full phase
           * (for example, a 20ms update rate but 10ms offset from the main loop invocation) to
           * help avoid high CPU loads. However, this does mean that any logged data that reads
           * directly from sensors will be slightly different from data used in the main robot
           * loop.
           */
          public static void bind(edu.wpi.first.epilogue.HelloWorld robot) {
            robot.addPeriodic(() -> {
              long start = System.nanoTime();
              helloWorldLogger.tryUpdate(config.dataLogger.getSubLogger(config.root), robot, config.errorHandler);
              edu.wpi.first.networktables.NetworkTableInstance.getDefault().getEntry("Epilogue/Stats/Last Run").setDouble((System.nanoTime() - start) / 1e6);
            }, robot.getPeriod(), robot.getPeriod() / 2);
          }
        }
        """;

    assertGeneratedEpiloguerContents(source, expected);
  }

  @Test
  void multipleRobots() {
    String source =
        """
          package edu.wpi.first.epilogue;

          @Epilogue
          class AlphaBot extends edu.wpi.first.wpilibj.TimedRobot { }

          @Epilogue
          class BetaBot extends edu.wpi.first.wpilibj.TimedRobot { }
          """;

    String expected =
        """
        package edu.wpi.first.epilogue;

        import edu.wpi.first.epilogue.AlphaBotLogger;
        import edu.wpi.first.epilogue.BetaBotLogger;

        public final class Epiloguer {
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
          public static boolean shouldLog(Epilogue.Importance importance) {
            return importance.compareTo(config.minimumImportance) >= 0;
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
            robot.addPeriodic(() -> {
              long start = System.nanoTime();
              alphaBotLogger.tryUpdate(config.dataLogger.getSubLogger(config.root), robot, config.errorHandler);
              edu.wpi.first.networktables.NetworkTableInstance.getDefault().getEntry("Epilogue/Stats/Last Run").setDouble((System.nanoTime() - start) / 1e6);
            }, robot.getPeriod(), robot.getPeriod() / 2);
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
            robot.addPeriodic(() -> {
              long start = System.nanoTime();
              betaBotLogger.tryUpdate(config.dataLogger.getSubLogger(config.root), robot, config.errorHandler);
              edu.wpi.first.networktables.NetworkTableInstance.getDefault().getEntry("Epilogue/Stats/Last Run").setDouble((System.nanoTime() - start) / 1e6);
            }, robot.getPeriod(), robot.getPeriod() / 2);
          }
        }
        """;

    assertGeneratedEpiloguerContents(source, expected);
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
          public void update(DataLogger logger, A object) {} // implementation is irrelevant
        }

        @Epilogue
        class HelloWorld {
          A a_b_or_c;
          B b;
          C c;
        }
        """;

    String expected =
        """
        package edu.wpi.first.epilogue;

        import edu.wpi.first.epilogue.HelloWorldLogger;
        import edu.wpi.first.epilogue.CustomLogger;

        public final class Epiloguer {
          private static final EpilogueConfiguration config = new EpilogueConfiguration();

          public static final HelloWorldLogger helloWorldLogger = new HelloWorldLogger();
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
          public static boolean shouldLog(Epilogue.Importance importance) {
            return importance.compareTo(config.minimumImportance) >= 0;
          }
        }
        """;

    assertGeneratedEpiloguerContents(source, expected);
  }

  private void assertGeneratedEpiloguerContents(
      String loggedClassContent, String loggerClassContent) {
    Compilation compilation =
        javac()
            .withProcessors(new AnnotationProcessor())
            .compile(JavaFileObjects.forSourceString("", loggedClassContent));

    assertThat(compilation).succeededWithoutWarnings();
    var generatedFiles = compilation.generatedSourceFiles();
    var generatedFile =
        generatedFiles.stream()
            .filter(jfo -> jfo.getName().contains("Epiloguer"))
            .findFirst()
            .orElseThrow(() -> new IllegalStateException("Epiloguer file was not generated!"));
    try {
      var content = generatedFile.getCharContent(false);
      assertEquals(loggerClassContent, content);
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
  }
}
