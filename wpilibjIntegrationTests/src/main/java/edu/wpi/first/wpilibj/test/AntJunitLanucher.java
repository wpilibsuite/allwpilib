/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.test;

import org.apache.tools.ant.BuildLogger;
import org.apache.tools.ant.DefaultLogger;
import org.apache.tools.ant.Project;
import org.apache.tools.ant.taskdefs.optional.junit.FormatterElement;
import org.apache.tools.ant.taskdefs.optional.junit.JUnitTask;
import org.apache.tools.ant.taskdefs.optional.junit.JUnitTest;

import java.io.File;

/**
 * Provides an entry point for tests to run with ANT. This allows ant to output JUnit XML test
 * results for Jenkins.
 */
public class AntJunitLanucher {

  /**
   * Main entry point for jenkins
   *
   * @param args Arguments passed to java.
   */
  public static void main(String... args) {
    if (args.length == 0) {
      String path =
          String.format("%s/%s", System.getProperty("user.dir"), "/testResults/AntReports");
      String pathToReports = path;
      Project project = new Project();

      try {
        // Create the file to store the test output
        new File(pathToReports).mkdirs();

        project.setProperty("java.io.tmpdir", pathToReports);

        /* Output to screen */
        FormatterElement.TypeAttribute typeScreen = new FormatterElement.TypeAttribute();
        typeScreen.setValue("plain");
        FormatterElement formatToScreen = new FormatterElement();
        formatToScreen.setType(typeScreen);
        formatToScreen.setUseFile(false);
        formatToScreen.setOutput(System.out);

        JUnitTask task = new JUnitTask();
        task.addFormatter(formatToScreen);

        // add a build listener to the project
        BuildLogger logger = new DefaultLogger();
        logger.setOutputPrintStream(System.out);
        logger.setErrorPrintStream(System.err);
        logger.setMessageOutputLevel(Project.MSG_INFO);
        logger.setEmacsMode(true);
        project.addBuildListener(logger);

        task.setProject(project);

        // Set the output to the XML file
        FormatterElement.TypeAttribute type = new FormatterElement.TypeAttribute();
        type.setValue("xml");

        FormatterElement formater = new FormatterElement();
        formater.setType(type);
        task.addFormatter(formater);

        // Create the JUnitTest
        JUnitTest test = new JUnitTest(TestSuite.class.getName());
        test.setTodir(new File(pathToReports));
        task.addTest(test);

        TestBench.out().println("Beginning Test Execution With ANT");
        task.execute();
      } catch (Exception ex) {
        ex.printStackTrace();
      }
    } else {
      TestBench.out().println(
          "Run will not output XML for Jenkins because " + "tests are not being run with ANT");
      // This should never return as it makes its own call to
      // System.exit();
      TestSuite.main(args);
    }
    System.exit(0);
  }

}
