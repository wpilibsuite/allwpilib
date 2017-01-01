/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.test;

import org.junit.Ignore;
import org.junit.Test;
import org.junit.runner.Request;
import org.junit.runners.Suite.SuiteClasses;

import java.lang.reflect.Method;
import java.util.List;
import java.util.Vector;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.regex.Pattern;

/**
 * Allows tests suites and tests to be run selectively from the command line using a regex text
 * pattern.
 */
public abstract class AbstractTestSuite {
  private static final Logger logger = Logger.getLogger(AbstractTestSuite.class.getName());

  /**
   * Gets all of the classes listed within the SuiteClasses annotation. To use it, annotate a class
   * with <code>@RunWith(Suite.class)</code> and <code>@SuiteClasses({TestClass1.class,
   * ...})</code>. When you run this class, it will run all the tests in all the suite classes. When
   * loading the tests using regex the test list will be generated from this annotation.
   *
   * @return the list of classes listed in the <code>@SuiteClasses({TestClass1.class, ...})</code>.
   * @throws RuntimeException If the <code>@SuiteClasses</code> annotation is missing.
   */
  protected List<Class<?>> getAnnotatedTestClasses() {
    SuiteClasses annotation = getClass().getAnnotation(SuiteClasses.class);
    List<Class<?>> classes = new Vector<Class<?>>();
    if (annotation == null) {
      throw new RuntimeException(String.format("class '%s' must have a SuiteClasses annotation",
          getClass().getName()));
    }
    for (Class<?> c : annotation.value()) {
      classes.add(c);
    }
    return classes;
  }

  private boolean areAnySuperClassesOfTypeAbstractTestSuite(Class<?> check) {
    while (check != null) {
      if (check.equals(AbstractTestSuite.class)) {
        return true;
      }
      check = check.getSuperclass();
    }
    return false;
  }

  /**
   * Stores a method name and method class pair. Used when searching for methods matching a given
   * regex text.
   */
  protected class ClassMethodPair {
    public final Class<?> m_methodClass;
    public final String m_methodName;

    public ClassMethodPair(Class<?> klass, Method method) {
      m_methodClass = klass;
      m_methodName = method.getName();
    }

    public Request getMethodRunRequest() {
      return Request.method(m_methodClass, m_methodName);
    }
  }

  protected List<ClassMethodPair> getMethodMatching(final String regex) {
    List<ClassMethodPair> classMethodPairs = new Vector<ClassMethodPair>();
    // Get all of the test classes
    for (Class<?> c : getAllContainedBaseTests()) {
      for (Method m : c.getMethods()) {
        // If this is a test method that is not trying to be ignored and it
        // matches the regex
        if (m.getAnnotation(Test.class) != null && m.getAnnotation(Ignore.class) == null
            && Pattern.matches(regex, m.getName())) {
          ClassMethodPair pair = new ClassMethodPair(c, m);
          classMethodPairs.add(pair);
        }
      }
    }
    return classMethodPairs;
  }


  /**
   * Gets all of the test classes listed in this suite. Does not include any of the test suites. All
   * of these classes contain tests.
   *
   * @param runningList the running list of classes to prevent recursion.
   * @return The list of base test classes.
   */
  private List<Class<?>> getAllContainedBaseTests(List<Class<?>> runningList) {
    for (Class<?> c : getAnnotatedTestClasses()) {
      // Check to see if this is a test class or a suite
      if (areAnySuperClassesOfTypeAbstractTestSuite(c)) {
        // Create a new instance of this class so that we can retrieve its data
        try {
          AbstractTestSuite suite = (AbstractTestSuite) c.newInstance();
          // Add the tests from this suite that match the regex to the list of
          // tests to run
          runningList = suite.getAllContainedBaseTests(runningList);
        } catch (InstantiationException | IllegalAccessException ex) {
          // This shouldn't happen unless the constructor is changed in some
          // way.
          logger.log(Level.SEVERE, "Test suites can not take paramaters in their constructors.",
              ex);
        }
      } else if (c.getAnnotation(SuiteClasses.class) != null) {
        logger.log(Level.SEVERE,
            String.format("class '%s' must extend %s to be searchable using regex.",
                c.getName(), AbstractTestSuite.class.getName()));
      } else { // This is a class containing tests
        // so add it to the list
        runningList.add(c);
      }
    }
    return runningList;
  }

  /**
   * Gets all of the test classes listed in this suite. Does not include any of the test suites. All
   * of these classes contain tests.
   *
   * @return The list of base test classes.
   */
  public List<Class<?>> getAllContainedBaseTests() {
    List<Class<?>> runningBaseTests = new Vector<Class<?>>();
    return getAllContainedBaseTests(runningBaseTests);
  }


  /**
   * Retrieves all of the classes listed in the <code>@SuiteClasses</code> annotation that match the
   * given regex text.
   *
   * @param regex       the text pattern to retrieve.
   * @param runningList the running list of classes to prevent recursion
   * @return The list of classes matching the regex pattern
   */
  private List<Class<?>> getAllClassMatching(final String regex, final List<Class<?>> runningList) {
    for (Class<?> c : getAnnotatedTestClasses()) {
      // Compare the regex against the simple name of the class
      if (Pattern.matches(regex, c.getName()) && !runningList.contains(c)) {
        runningList.add(c);
      }
    }
    return runningList;
  }

  /**
   * Retrieves all of the classes listed in the <code>@SuiteClasses</code> annotation that match the
   * given regex text.
   *
   * @param regex the text pattern to retrieve.
   * @return The list of classes matching the regex pattern
   */
  public List<Class<?>> getAllClassMatching(final String regex) {
    final List<Class<?>> matchingClasses = new Vector<Class<?>>();
    return getAllClassMatching(regex, matchingClasses);
  }

  /**
   * Searches through all of the suites and tests and loads only the test or test suites matching
   * the regex text. This method also prevents a single test from being loaded multiple times by
   * loading the suite first then loading tests from all non loaded suites.
   *
   * @param regex the regex text to search for
   * @return the list of suite and/or test classes matching the regex.
   */
  private List<Class<?>> getSuiteOrTestMatchingRegex(final String regex, List<Class<?>>
      runningList) {
    // Get any test suites matching the regex using the superclass methods
    runningList = getAllClassMatching(regex, runningList);


    // Then search any test suites not retrieved already for test classes
    // matching the regex.
    List<Class<?>> unCollectedSuites = getAllClasses();
    // If we already have a test suite then we don't want to load the test twice
    // so remove the suite from the list
    unCollectedSuites.removeAll(runningList);
    for (Class<?> c : unCollectedSuites) {
      // Prevents recursively adding tests/suites that have already been added
      if (!runningList.contains(c)) {
        try {
          final AbstractTestSuite suite;
          // Check the class to make sure that it is not a test class
          if (areAnySuperClassesOfTypeAbstractTestSuite(c)) {
            // Create a new instance of this class so that we can retrieve its
            // data.
            suite = (AbstractTestSuite) c.newInstance();
            // Add the tests from this suite that match the regex to the list of
            // tests to run
            runningList = suite.getSuiteOrTestMatchingRegex(regex, runningList);
          }

        } catch (InstantiationException | IllegalAccessException ex) {
          // This shouldn't happen unless the constructor is changed in some
          // way.
          logger.log(Level.SEVERE, "Test suites can not take paramaters in their constructors.",
              ex);
        }
      }
    }
    return runningList;
  }

  /**
   * Searches through all of the suites and tests and loads only the test or test suites matching
   * the regex text. This method also prevents a single test from being loaded multiple times by
   * loading the suite first then loading tests from all non loaded suites.
   *
   * @param regex the regex text to search for
   * @return the list of suite and/or test classes matching the regex.
   */
  protected List<Class<?>> getSuiteOrTestMatchingRegex(final String regex) {
    final List<Class<?>> matchingClasses = new Vector<Class<?>>();
    return getSuiteOrTestMatchingRegex(regex, matchingClasses);
  }


  /**
   * Retrieves all of the classes listed in the <code>@SuiteClasses</code> annotation.
   *
   * @return List of SuiteClasses
   * @throws RuntimeException If the <code>@SuiteClasses</code> annotation is missing.
   */
  public List<Class<?>> getAllClasses() {
    return getAnnotatedTestClasses();
  }

  /**
   * Gets the name of all of the classes listed within the <code>@SuiteClasses</code> annotation.
   *
   * @return the list of classes.
   * @throws RuntimeException If the <code>@SuiteClasses</code> annotation is missing.
   */
  public List<String> getAllClassName() {
    List<String> classNames = new Vector<String>();
    for (Class<?> c : getAnnotatedTestClasses()) {
      classNames.add(c.getName());
    }
    return classNames;
  }
}
