// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.epilogue.processor;

import java.io.IOException;
import java.io.PrintWriter;
import java.nio.charset.StandardCharsets;
import java.util.List;
import java.util.stream.Stream;
import javax.annotation.processing.FilerException;
import javax.annotation.processing.ProcessingEnvironment;
import javax.lang.model.element.TypeElement;
import javax.tools.StandardLocation;

/**
 * Generates the {@code META-INF/services} provider configuration resource file for Epilogue
 * services.
 */
public class EpilogueResourceGenerator {
  private final ProcessingEnvironment m_processingEnv;

  /**
   * Constructs an EpilogueResourceGenerator.
   *
   * @param processingEnv the annotation processing environment
   */
  public EpilogueResourceGenerator(ProcessingEnvironment processingEnv) {
    this.m_processingEnv = processingEnv;
  }

  /**
   * Generates the {@code META-INF/services/org.wpilib.epilogue.EpilogueService} file listing all
   * service implementation class names.
   *
   * @param mainRobotClasses the robot classes that inherit from RobotBase
   * @param timedRobotClasses the robot classes that inherit from TimedRobot
   */
  public void writeResourceFile(
      List<TypeElement> mainRobotClasses, List<TypeElement> timedRobotClasses) {
    try {
      List<TypeElement> allRobotClasses =
          Stream.concat(mainRobotClasses.stream(), timedRobotClasses.stream()).distinct().toList();

      if (allRobotClasses.isEmpty()) {
        return;
      }

      var serviceFile =
          m_processingEnv
              .getFiler()
              .createResource(
                  StandardLocation.CLASS_OUTPUT,
                  "",
                  "META-INF/services/org.wpilib.epilogue.EpilogueService",
                  allRobotClasses.toArray(new TypeElement[0]));

      try (var out =
          new PrintWriter(serviceFile.openOutputStream(), false, StandardCharsets.UTF_8)) {
        for (TypeElement robotClass : allRobotClasses) {
          out.println(robotClass.getQualifiedName() + "_EpilogueService");
        }
      }
    } catch (FilerException e) {
      // Ignore
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
  }
}
