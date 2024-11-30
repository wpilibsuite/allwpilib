// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.processor;

import edu.wpi.first.epilogue.EpilogueConfiguration;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Collection;
import java.util.List;
import java.util.Map;
import javax.annotation.processing.ProcessingEnvironment;
import javax.lang.model.element.TypeElement;
import javax.lang.model.type.DeclaredType;
import javax.lang.model.type.TypeMirror;

/**
 * Generates the {@code Epilogue} file used as the main entry point to logging with Epilogue in a
 * robot program. {@code Epilogue} has instances of every generated logger class, a {@link
 * EpilogueConfiguration config} object, and (if the main robot class inherits from {@link
 * edu.wpi.first.wpilibj.TimedRobot TimedRobot}) a {@code bind()} method to automatically add a
 * periodic logging call to the robot.
 */
public class EpilogueGenerator {
  private final ProcessingEnvironment m_processingEnv;
  private final Map<TypeMirror, DeclaredType> m_customLoggers;

  public EpilogueGenerator(
      ProcessingEnvironment processingEnv, Map<TypeMirror, DeclaredType> customLoggers) {
    this.m_processingEnv = processingEnv;
    this.m_customLoggers = customLoggers;
  }

  /**
   * Creates the Epilogue file, which is the main entry point for users to set up and interact with
   * the generated loggers.
   *
   * @param loggerClassNames the names of the generated logger classes. Each of these will be
   *     instantiated in a public static field on the Epilogue class.
   * @param mainRobotClasses the main robot classes. May be empty. Used to generate a {@code bind()}
   *     method to add a callback hook to a TimedRobot to log itself.
   */
  @SuppressWarnings("checkstyle:LineLength") // Source code templates exceed the line length limit
  public void writeEpilogueFile(
      List<String> loggerClassNames, Collection<TypeElement> mainRobotClasses) {
    try {
      var centralStore =
          m_processingEnv.getFiler().createSourceFile("edu.wpi.first.epilogue.Epilogue");

      try (var out = new PrintWriter(centralStore.openOutputStream())) {
        out.println("package edu.wpi.first.epilogue;");
        out.println();

        out.println("import static edu.wpi.first.units.Units.Seconds;");
        out.println();

        out.println("import edu.wpi.first.hal.FRCNetComm;");
        out.println("import edu.wpi.first.hal.HAL;");
        out.println();

        loggerClassNames.stream()
            .sorted()
            .forEach(
                name -> {
                  if (!name.contains(".")) {
                    // Logger is in the global namespace, don't need to import
                    return;
                  }

                  out.println("import " + name + ";");
                });
        m_customLoggers.values().stream()
            .distinct()
            .forEach(
                loggerType -> {
                  var name = loggerType.asElement().toString();
                  if (!name.contains(".")) {
                    // Logger is in the global namespace, don't need to import
                    return;
                  }
                  out.println("import " + name + ";");
                });
        out.println();

        out.println("public final class Epilogue {");

        // Usage reporting
        out.println(
            """
              static {
                HAL.report(
                  FRCNetComm.tResourceType.kResourceType_LoggingFramework,
                  FRCNetComm.tInstances.kLoggingFramework_Epilogue
                );
              }
            """);

        out.println(
            "  private static final EpilogueConfiguration config = new EpilogueConfiguration();");
        out.println();

        loggerClassNames.forEach(
            name -> {
              String simple = StringUtils.simpleName(name);

              // public static final FooLogger fooLogger = new FooLogger();
              out.print("  public static final ");
              out.print(simple);
              out.print(" ");
              out.print(StringUtils.lowerCamelCase(simple));
              out.print(" = new ");
              out.print(simple);
              out.println("();");
            });
        m_customLoggers.values().stream()
            .distinct()
            .forEach(
                loggerType -> {
                  var loggerTypeName = loggerType.asElement().getSimpleName();
                  out.println(
                      "  public static final "
                          + loggerTypeName
                          + " "
                          + StringUtils.lowerCamelCase(loggerTypeName)
                          + " = new "
                          + loggerTypeName
                          + "();");
                });
        out.println();

        out.println(
            """
              public static void configure(java.util.function.Consumer<EpilogueConfiguration> configurator) {
                configurator.accept(config);
              }

              public static EpilogueConfiguration getConfig() {
                return config;
              }
            """);

        out.println(
            """
              /**
               * Checks if data associated with a given importance level should be logged.
               */
              public static boolean shouldLog(Logged.Importance importance) {
                return importance.compareTo(config.minimumImportance) >= 0;
              }
            """
                .stripTrailing());

        // Only generate a binding if the robot class is a TimedRobot
        if (!mainRobotClasses.isEmpty()) {
          for (TypeElement mainRobotClass : mainRobotClasses) {
            String robotClassName = mainRobotClass.getQualifiedName().toString();

            out.println();
            out.print(
                """
                  /**
                   * Updates Epilogue. This must be called periodically in order for Epilogue to record
                   * new values. Alternatively, {@code bind()} can be used to update at an offset from
                   * the main robot loop.
                   */
                """);
            out.println("  public static void update(" + robotClassName + " robot) {");
            out.println("    long start = System.nanoTime();");
            out.println(
                "    "
                    + StringUtils.loggerFieldName(mainRobotClass)
                    + ".tryUpdate(config.backend.getNested(config.root), robot, config.errorHandler);");
            out.println(
                "    config.backend.log(\"Epilogue/Stats/Last Run\", (System.nanoTime() - start) / 1e6);");
            out.println("  }");

            out.println();
            out.print(
                """
                  /**
                   * Binds Epilogue updates to a timed robot's update period. Log calls will be made at the
                   * same update rate as the robot's loop function, but will be offset by a full phase
                   * (for example, a 20ms update rate but 10ms offset from the main loop invocation) to
                   * help avoid high CPU loads. However, this does mean that any logged data that reads
                   * directly from sensors will be slightly different from data used in the main robot
                   * loop.
                   */
                """);
            out.println("  public static void bind(" + robotClassName + " robot) {");
            out.println("    if (config.loggingPeriod == null) {");
            out.println("      config.loggingPeriod = Seconds.of(robot.getPeriod());");
            out.println("    }");
            out.println("    if (config.loggingPeriodOffset == null) {");
            out.println("      config.loggingPeriodOffset = config.loggingPeriod.div(2);");
            out.println("    }");
            out.println();
            out.println("    robot.addPeriodic(() -> {");
            out.println("      update(robot);");
            out.println(
                "    }, config.loggingPeriod.in(Seconds), config.loggingPeriodOffset.in(Seconds));");
            out.println("  }");
          }
        }

        out.println("}");
      }
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
  }
}
