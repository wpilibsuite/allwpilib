// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.epilogue.processor;

import java.io.IOException;
import java.io.PrintWriter;
import java.nio.charset.StandardCharsets;
import java.util.List;
import javax.annotation.processing.FilerException;
import javax.annotation.processing.ProcessingEnvironment;
import javax.lang.model.element.TypeElement;

/**
 * Generates {@link org.wpilib.epilogue.EpilogueService EpilogueService} implementation source files
 * for robot classes.
 */
@SuppressWarnings("checkstyle:LineLength") // Source code templates exceed the line length limit
public class EpilogueServiceGenerator {
  private final ProcessingEnvironment m_processingEnv;

  private enum Variant {
    STANDARD,
    BINDABLE,
  }

  /**
   * Constructs an EpilogueServiceGenerator.
   *
   * @param processingEnv the annotation processing environment
   */
  public EpilogueServiceGenerator(ProcessingEnvironment processingEnv) {
    this.m_processingEnv = processingEnv;
  }

  /**
   * Generates {@link org.wpilib.epilogue.EpilogueService EpilogueService} source files for the
   * given robot classes.
   *
   * @param mainRobotClasses the robot classes that inherit from RobotBase
   * @param timedRobotClasses the robot classes that inherit from TimedRobot
   */
  public void writeServiceFiles(
      List<TypeElement> mainRobotClasses, List<TypeElement> timedRobotClasses) {
    try {
      for (TypeElement clazz : mainRobotClasses) {
        var variant = timedRobotClasses.contains(clazz) ? Variant.BINDABLE : Variant.STANDARD;
        createLoggerService(clazz, variant);
      }
    } catch (FilerException e) {
      // Ignore
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
  }

  private void createLoggerService(TypeElement robotClass, Variant variant) throws IOException {
    var serviceName = robotClass.getSimpleName() + "_EpilogueService";

    var service =
        m_processingEnv
            .getFiler()
            .createSourceFile(robotClass.getQualifiedName() + "_EpilogueService", robotClass);

    try (var out = new PrintWriter(service.openOutputStream(), false, StandardCharsets.UTF_8)) {
      var pkg = m_processingEnv.getElementUtils().getPackageOf(robotClass);
      if (!pkg.isUnnamed()) {
        out.println("package " + pkg.getQualifiedName() + ";");
        out.println();
      }

      if (variant == Variant.BINDABLE) {
        out.println("import static org.wpilib.units.Units.Seconds;");
        out.println();
      }

      out.println("import org.wpilib.epilogue.Epilogue;");
      out.println("import org.wpilib.epilogue.EpilogueConfiguration;");
      out.println("import org.wpilib.epilogue.EpilogueService;");
      out.println("import org.wpilib.framework.RobotBase;");
      out.println();

      var baseType = switch (variant) {
        case STANDARD -> "EpilogueService";
        case BINDABLE -> "EpilogueService.Bindable";
      };
      out.printf(
          "public final class %s implements %s<%s> {%n",
          serviceName, baseType, robotClass.getSimpleName());
      out.println("  @Override");
      out.println("  public boolean supportsExactly(RobotBase root) {");
      out.printf(
          "    return root != null && root.getClass().equals(%s.class);%n",
          robotClass.getQualifiedName());
      out.println("  }");
      out.println();
      out.println("  @Override");
      out.printf("  public void update(%s root) {%n", robotClass.getSimpleName());
      out.println("    long start = System.nanoTime();");
      out.println("    EpilogueConfiguration config = Epilogue.getConfig();");
      out.printf(
          "    org.wpilib.epilogue.generated.EpilogueLoggers.%s.tryUpdate(config.table.getTable(config.root), root, config.errorHandler);%n",
          StringUtils.loggerFieldName(robotClass));
      out.println(
          "    config.table.log(\"Epilogue/Stats/Last Run\", (System.nanoTime() - start) / 1e6);");
      out.println("  }");

      if (variant == Variant.BINDABLE) {
        out.println();
        out.println("  @Override");
        out.printf("  public void bind(%s root) {%n", robotClass.getSimpleName());
        out.println("    EpilogueConfiguration config = Epilogue.getConfig();");
        out.println("    if (config.loggingPeriod == null) {");
        out.println("      config.loggingPeriod = Seconds.of(root.getPeriod());");
        out.println("    }");
        out.println("    if (config.loggingPeriodOffset == null) {");
        out.println("      config.loggingPeriodOffset = config.loggingPeriod.div(2);");
        out.println("    }");
        out.println();
        out.println("    root.addPeriodic(() -> {");
        out.println("      update(root);");
        out.println(
            "    }, config.loggingPeriod.in(Seconds), config.loggingPeriodOffset.in(Seconds));");
        out.println("  }");
      }
      out.println("}");
    }
  }
}
