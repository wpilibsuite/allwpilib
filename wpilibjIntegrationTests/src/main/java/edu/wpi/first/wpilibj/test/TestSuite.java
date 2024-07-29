// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.test;

import edu.wpi.first.wpilibj.WpiLibJTestSuite;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.logging.LogManager;
import java.util.logging.Logger;
import java.util.regex.Pattern;
import junit.framework.JUnit4TestAdapter;
import junit.runner.Version;
import org.junit.runner.JUnitCore;
import org.junit.runner.Result;
import org.junit.runner.RunWith;
import org.junit.runner.notification.Failure;
import org.junit.runners.Suite;
import org.junit.runners.Suite.SuiteClasses;

/**
 * The WPILibJ Integeration Test Suite collects all of the tests to be run by junit. In order for a
 * test to be run, it must be added the list of suite classes below. The tests will be run in the
 * order they are listed in the suite classes annotation.
 */
@RunWith(Suite.class)
@SuiteClasses(WpiLibJTestSuite.class)
public class TestSuite extends AbstractTestSuite {
  static {
    // Sets up the logging output
    final InputStream inputStream = TestSuite.class.getResourceAsStream("/logging.properties");
    try {
      Objects.requireNonNull(inputStream, "./logging.properties was not loaded");
      LogManager.getLogManager().readConfiguration(inputStream);
      Logger.getAnonymousLogger().info("Loaded");
    } catch (final IOException | NullPointerException ex) {
      Logger.getAnonymousLogger().severe("Could not load default logging.properties file");
      Logger.getAnonymousLogger().severe(ex.getMessage());
    }
    try {
      inputStream.close();
    } catch (IOException e) {
      throw new RuntimeException(e.getMessage());
    }

    TestBench.out().println("Starting Tests");
  }

  @SuppressWarnings("unused")
  private static final Logger WPILIBJ_ROOT_LOGGER = Logger.getLogger("edu.wpi.first.wpilibj");

  @SuppressWarnings("unused")
  private static final Logger WPILIBJ_COMMAND_ROOT_LOGGER =
      Logger.getLogger("edu.wpi.first.wpilibj.command");

  private static final Class<?> QUICK_TEST = QuickTest.class;
  private static final String QUICK_TEST_FLAG = "--quick";
  private static final String HELP_FLAG = "--help";
  private static final String METHOD_NAME_FILTER = "--methodFilter=";
  private static final String METHOD_REPEAT_FILTER = "--repeat=";
  private static final String CLASS_NAME_FILTER = "--filter=";

  private static TestSuite instance = null;

  /** Get the singleton instance of the test suite. */
  public static TestSuite getInstance() {
    if (instance == null) {
      instance = new TestSuite();
    }
    return instance;
  }

  /** This has to be public so that the JUnit4. */
  public TestSuite() {}

  /** Displays a help message for the user when they use the --help flag at runtime. */
  protected static void displayHelp() {
    String helpMessage =
        "Test Parameters help: \n"
            + "\t"
            + QUICK_TEST_FLAG
            + " will cause the quick test to be run. Ignores other flags except for "
            + METHOD_REPEAT_FILTER
            + "\n"
            + "\t"
            + CLASS_NAME_FILTER
            + " will use the supplied regex text to search for suite/test class names "
            + "matching the regex and run them.\n"
            + "\t"
            + METHOD_NAME_FILTER
            + " will use the supplied regex text to search for test methods (excluding methods "
            + "with the @Ignore annotation) and run only those methods. Can be paired with "
            + METHOD_REPEAT_FILTER
            + " to "
            + "repeat the selected tests multiple times.\n"
            + "\t"
            + METHOD_REPEAT_FILTER
            + " will repeat the tests selected with either "
            + QUICK_TEST_FLAG
            + " or "
            + CLASS_NAME_FILTER
            + " and run them the given number of times.\n"
            + "[NOTE] All regex uses the syntax defined by java.util.regex.Pattern. This "
            + "documentation can be found at "
            + "http://docs.oracle.com/javase/7/docs/api/java/util/regex/Pattern.html\n"
            + "\n"
            + "\n";

    TestBench.out().println(helpMessage);
  }

  /**
   * Lets the user know that they used the TestSuite improperly and gives details about how to use
   * it correctly in the future.
   */
  protected static void displayInvalidUsage(String message, String... args) {
    StringBuilder invalidMessage = new StringBuilder("Invalid Usage: " + message + "\n");
    invalidMessage.append("Params received: ");
    for (String a : args) {
      invalidMessage.append(a).append(" ");
    }
    invalidMessage.append("\n");
    invalidMessage.append(
        "For details on proper usage of the runtime flags please run again with the "
            + HELP_FLAG
            + " flag.\n\n");

    TestBench.out().println(invalidMessage);
  }

  /**
   * Prints the loaded tests before they are run.
   *
   * @param classes the classes that were loaded.
   */
  protected static void printLoadedTests(final Class<?>... classes) {
    StringBuilder loadedTestsMessage = new StringBuilder("The following tests were loaded:\n");
    Package packagE = null;
    for (Class<?> c : classes) {
      if (c.getPackage().equals(packagE)) {
        packagE = c.getPackage();
        loadedTestsMessage.append(packagE.getName()).append("\n");
      }
      loadedTestsMessage.append("\t").append(c.getSimpleName()).append("\n");
    }
    TestBench.out().println(loadedTestsMessage);
  }

  /**
   * Parses the arguments passed at runtime and runs the appropriate tests based upon these
   * arguments.
   *
   * @param args the args passed into the program at runtime
   * @return the restults of the tests that have run. If no tests were run then null is returned.
   */
  protected static Result parseArgsRunAndGetResult(final String[] args) {
    if (args.length == 0) { // If there are no args passed at runtime then just
      // run all of the tests.
      printLoadedTests(TestSuite.class);
      return JUnitCore.runClasses(TestSuite.class);
    }

    // The method filter was passed
    boolean methodFilter = false;
    String methodRegex = "";
    // The class filter was passed
    boolean classFilter = false;
    String classRegex = "";
    int repeatCount = 1;

    for (String s : args) {
      if (Pattern.matches(METHOD_NAME_FILTER + ".*", s)) {
        methodFilter = true;
        methodRegex = s.replace(METHOD_NAME_FILTER, "");
      }
      if (Pattern.matches(METHOD_REPEAT_FILTER + ".*", s)) {
        try {
          repeatCount = Integer.parseInt(s.replace(METHOD_REPEAT_FILTER, ""));
        } catch (NumberFormatException ex) {
          displayInvalidUsage(
              "The argument passed to the repeat rule was not a valid integer.", args);
        }
      }
      if (Pattern.matches(CLASS_NAME_FILTER + ".*", s)) {
        classFilter = true;
        classRegex = s.replace(CLASS_NAME_FILTER, "");
      }
    }

    ArrayList<String> argsParsed = new ArrayList<>(List.of(args));
    if (argsParsed.contains(HELP_FLAG)) {
      // If the user inputs the help flag then return the help message and exit
      // without running any tests
      displayHelp();
      return null;
    }
    if (argsParsed.contains(QUICK_TEST_FLAG)) {
      printLoadedTests(QUICK_TEST);
      return JUnitCore.runClasses(QUICK_TEST);
    }

    /**
     * Stores the data from multiple {@link Result}s in one class that can be returned to display
     * the results.
     */
    class MultipleResult extends Result {
      private static final long serialVersionUID = 1L;
      private final List<Failure> m_failures = new ArrayList<>();
      private int m_runCount = 0;
      private int m_ignoreCount = 0;
      private long m_runTime = 0;

      @Override
      public int getRunCount() {
        return m_runCount;
      }

      @Override
      public int getFailureCount() {
        return m_failures.size();
      }

      @Override
      public long getRunTime() {
        return m_runTime;
      }

      @Override
      public List<Failure> getFailures() {
        return m_failures;
      }

      @Override
      public int getIgnoreCount() {
        return m_ignoreCount;
      }

      /**
       * Adds the given result's data to this result.
       *
       * @param result the result to add to this result
       */
      void addResult(Result result) {
        m_failures.addAll(result.getFailures());
        m_runCount += result.getRunCount();
        m_ignoreCount += result.getIgnoreCount();
        m_runTime += result.getRunTime();
      }
    }

    // If a specific method has been requested
    if (methodFilter) {
      List<ClassMethodPair> pairs = (new TestSuite()).getMethodMatching(methodRegex);
      if (pairs.isEmpty()) {
        displayInvalidUsage(
            "None of the arguments passed to the method name filter matched.", args);
        return null;
      }
      // Print out the list of tests before we run them
      TestBench.out().println("Running the following tests:");
      Class<?> classListing = null;
      for (ClassMethodPair p : pairs) {
        if (!p.m_methodClass.equals(classListing)) {
          // Only display the class name every time it changes
          classListing = p.m_methodClass;
          TestBench.out().println(classListing);
        }
        TestBench.out().println("\t" + p.m_methodName);
      }

      // The test results will be saved here
      MultipleResult results = new MultipleResult();
      // Runs tests multiple times if the repeat rule is used
      for (int i = 0; i < repeatCount; i++) {
        // Now run all of the tests
        for (ClassMethodPair p : pairs) {
          Result result = (new JUnitCore()).run(p.getMethodRunRequest());
          // Store the given results in one cohesive result
          results.addResult(result);
        }
      }

      return results;
    }

    // If a specific class has been requested
    if (classFilter) {
      List<Class<?>> testClasses = (new TestSuite()).getSuiteOrTestMatchingRegex(classRegex);
      if (testClasses.isEmpty()) {
        displayInvalidUsage("None of the arguments passed to the filter matched.", args);
        return null;
      }
      printLoadedTests(testClasses.toArray(new Class[0]));
      MultipleResult results = new MultipleResult();
      // Runs tests multiple times if the repeat rule is used
      for (int i = 0; i < repeatCount; i++) {
        Result result = (new JUnitCore()).run(testClasses.toArray(new Class[0]));
        // Store the given results in one cohesive result
        results.addResult(result);
      }
      return results;
    }
    displayInvalidUsage(
        "None of the parameters that you passed matched any of the accepted flags.", args);

    return null;
  }

  protected static void displayResults(Result result) {
    // Results are collected and displayed here
    TestBench.out().println("\n");
    if (!result.wasSuccessful()) {
      // Prints out a list of stack traces for the failed tests
      TestBench.out().println("Failure List: ");
      for (Failure f : result.getFailures()) {
        TestBench.out().println(f.getDescription());
        TestBench.out().println(f.getTrace());
      }
      TestBench.out().println();
      TestBench.out().println("FAILURES!!!");
      // Print the test statistics
      TestBench.out()
          .println(
              "Tests run: "
                  + result.getRunCount()
                  + ", Failures: "
                  + result.getFailureCount()
                  + ", Ignored: "
                  + result.getIgnoreCount()
                  + ", In "
                  + result.getRunTime()
                  + "ms");

      // Prints out a list of test that failed
      TestBench.out().println("Failure List (short):");
      String failureClass = result.getFailures().get(0).getDescription().getClassName();
      TestBench.out().println(failureClass);
      for (Failure f : result.getFailures()) {
        if (!failureClass.equals(f.getDescription().getClassName())) {
          failureClass = f.getDescription().getClassName();
          TestBench.out().println(failureClass);
        }
        TestBench.err().println("\t" + f.getDescription());
      }
    } else {
      TestBench.out().println("SUCCESS!");
      TestBench.out()
          .println(
              "Tests run: "
                  + result.getRunCount()
                  + ", Ignored: "
                  + result.getIgnoreCount()
                  + ", In "
                  + result.getRunTime()
                  + "ms");
    }
    TestBench.out().println();
  }

  /**
   * This is used by ant to get the Junit tests. This is required because the tests try to load
   * using a JUnit 3 framework. JUnit4 uses annotations to load tests. This method allows JUnit3 to
   * load JUnit4 tests.
   */
  public static junit.framework.Test suite() {
    return new JUnit4TestAdapter(TestSuite.class);
  }

  /**
   * The method called at runtime.
   *
   * @param args The test suites to run
   */
  public static void main(String[] args) {
    TestBench.out().println("JUnit version " + Version.id());

    // Tests are run here
    Result result = parseArgsRunAndGetResult(args);
    if (result != null) {
      // Results are collected and displayed here
      displayResults(result);

      System.exit(result.wasSuccessful() ? 0 : 1);
    }
    System.exit(1);
  }
}
