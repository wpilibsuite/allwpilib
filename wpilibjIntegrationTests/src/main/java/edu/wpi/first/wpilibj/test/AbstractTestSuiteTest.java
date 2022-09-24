// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.test;

import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.hasItems;
import static org.hamcrest.Matchers.not;
import static org.junit.Assert.assertEquals;

import edu.wpi.first.wpilibj.test.AbstractTestSuite.ClassMethodPair;
import java.util.List;
import org.junit.Before;
import org.junit.Ignore;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Suite;
import org.junit.runners.Suite.SuiteClasses;

/**
 * Yes, this is the test system testing itself. Functionally, this is making sure that all tests get
 * run correctly when you use parametrized arguments.
 */
@SuppressWarnings("MultipleTopLevelClasses")
public class AbstractTestSuiteTest {
  @Ignore("Prevents ANT from trying to run these as tests")
  @RunWith(Suite.class)
  @SuiteClasses({
    FirstSampleTest.class,
    SecondSampleTest.class,
    ThirdSampleTest.class,
    FourthSampleTest.class,
    UnusualTest.class,
    ExampleSubSuite.class,
    EmptySuite.class
  })
  static class TestForAbstractTestSuite extends AbstractTestSuite {}

  TestForAbstractTestSuite m_testSuite;

  @Before
  public void setUp() {
    m_testSuite = new TestForAbstractTestSuite();
  }

  @Test
  public void testGetTestsMatchingAll() {
    // when
    List<Class<?>> collectedTests = m_testSuite.getAllClassMatching(".*");
    // then
    assertEquals(7, collectedTests.size());
  }

  @Test
  public void testGetTestsMatchingSample() {
    // when
    List<Class<?>> collectedTests = m_testSuite.getAllClassMatching(".*Sample.*");
    // then
    assertEquals(4, collectedTests.size());
  }

  @Test
  public void testGetTestsMatchingUnusual() {
    // when
    List<Class<?>> collectedTests = m_testSuite.getAllClassMatching(".*Unusual.*");
    // then
    assertEquals(1, collectedTests.size());
    assertEquals(UnusualTest.class, collectedTests.get(0));
  }

  @Test
  public void testGetTestsFromSuiteMatchingAll() {
    // when
    List<Class<?>> collectedTests = m_testSuite.getSuiteOrTestMatchingRegex(".*");
    // then
    assertEquals(7, collectedTests.size());
  }

  @Test
  public void testGetTestsFromSuiteMatchingTest() {
    // when
    List<Class<?>> collectedTests = m_testSuite.getSuiteOrTestMatchingRegex(".*Test.*");
    // then
    assertEquals(7, collectedTests.size());
    assertThat(
        collectedTests,
        hasItems(FirstSubSuiteTest.class, SecondSubSuiteTest.class, UnusualTest.class));
    assertThat(
        collectedTests, not(hasItems(new Class<?>[] {ExampleSubSuite.class, EmptySuite.class})));
  }

  @Test
  public void testGetMethodFromTest() {
    // when
    List<ClassMethodPair> pairs = m_testSuite.getMethodMatching(".*Method.*");
    // then
    assertEquals(1, pairs.size());
    assertEquals(FirstSubSuiteTest.class, pairs.get(0).m_methodClass);
    assertEquals(FirstSubSuiteTest.METHODNAME, pairs.get(0).m_methodName);
  }
}

@SuppressWarnings("OneTopLevelClass")
class FirstSampleTest {}

@SuppressWarnings("OneTopLevelClass")
class SecondSampleTest {}

@SuppressWarnings("OneTopLevelClass")
class ThirdSampleTest {}

@SuppressWarnings("OneTopLevelClass")
class FourthSampleTest {}

@SuppressWarnings("OneTopLevelClass")
class UnusualTest {} // This is a member of both suites

@Ignore("Prevents ANT from trying to run these as tests")
@SuppressWarnings("OneTopLevelClass")
class FirstSubSuiteTest {
  public static final String METHODNAME = "aTestMethod";

  @Test
  public void testMethod() {}
}

@SuppressWarnings("OneTopLevelClass")
class SecondSubSuiteTest {}

@RunWith(Suite.class)
@SuiteClasses({FirstSubSuiteTest.class, SecondSubSuiteTest.class, UnusualTest.class})
@Ignore("Prevents ANT from trying to run these as tests")
@SuppressWarnings("OneTopLevelClass")
class ExampleSubSuite extends AbstractTestSuite {}

@Ignore("Prevents ANT from trying to run these as tests")
@RunWith(Suite.class)
@SuiteClasses({})
@SuppressWarnings("OneTopLevelClass")
class EmptySuite extends AbstractTestSuite {}
