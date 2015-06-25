/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved. */
/* Open Source Software - may be modified and shared by FRC teams. The code */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project. */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.test;

import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.hasItems;
import static org.hamcrest.Matchers.not;
import static org.junit.Assert.assertEquals;

import java.util.List;

import org.junit.Before;
import org.junit.Ignore;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Suite;
import org.junit.runners.Suite.SuiteClasses;
import org.junit.runners.model.InitializationError;

import edu.wpi.first.wpilibj.test.AbstractTestSuite.ClassMethodPair;

/**
 * @author jonathanleitschuh
 *
 */
public class AbstractTestSuiteTest {

  @Ignore("Prevents ANT from trying to run these as tests")
  @RunWith(Suite.class)
  @SuiteClasses({FirstSampleTest.class, SecondSampleTest.class, ThirdSampleTest.class,
      FourthSampleTest.class, UnusualTest.class, ExampleSubSuite.class, EmptySuite.class})
  class TestForAbstractTestSuite extends AbstractTestSuite {
  }

  TestForAbstractTestSuite testSuite;

  /**
   * @throws java.lang.Exception
   */
  @Before
  public void setUp() throws Exception {
    testSuite = new TestForAbstractTestSuite();
  }

  @Test
  public void testGetTestsMatchingAll() throws InitializationError {
    // when
    List<Class<?>> collectedTests = testSuite.getAllClassMatching(".*");
    // then
    assertEquals(7, collectedTests.size());
  }

  @Test
  public void testGetTestsMatchingSample() throws InitializationError {
    // when
    List<Class<?>> collectedTests = testSuite.getAllClassMatching(".*Sample.*");
    // then
    assertEquals(4, collectedTests.size());
  }

  @Test
  public void testGetTestsMatchingUnusual() throws InitializationError {
    // when
    List<Class<?>> collectedTests = testSuite.getAllClassMatching(".*Unusual.*");
    // then
    assertEquals(1, collectedTests.size());
    assertEquals(UnusualTest.class, collectedTests.get(0));
  }

  @Test
  public void testGetTestsFromSuiteMatchingAll() throws InitializationError {
    // when
    List<Class<?>> collectedTests = testSuite.getSuiteOrTestMatchingRegex(".*");
    // then
    assertEquals(7, collectedTests.size());
  }

  @Test
  public void testGetTestsFromSuiteMatchingTest() throws InitializationError {
    // when
    List<Class<?>> collectedTests = testSuite.getSuiteOrTestMatchingRegex(".*Test.*");
    // then
    assertEquals(7, collectedTests.size());
    assertThat(collectedTests, hasItems(new Class<?>[] {FirstSubSuiteTest.class,
        SecondSubSuiteTest.class, UnusualTest.class}));
    assertThat(collectedTests,
        not(hasItems(new Class<?>[] {ExampleSubSuite.class, EmptySuite.class})));
  }

  @Test
  public void testGetMethodFromTest() {
    // when
    List<ClassMethodPair> pairs = testSuite.getMethodMatching(".*Method.*");
    // then
    assertEquals(1, pairs.size());
    assertEquals(FirstSubSuiteTest.class, pairs.get(0).methodClass);
    assertEquals(FirstSubSuiteTest.METHODNAME, pairs.get(0).methodName);

  }

}


class FirstSampleTest {
}


class SecondSampleTest {
}


class ThirdSampleTest {
}


class FourthSampleTest {
}


class UnusualTest {
} // This is a member of both suites


@Ignore("Prevents ANT from trying to run these as tests")
class FirstSubSuiteTest {
  public static final String METHODNAME = "aTestMethod";

  @Test
  public void aTestMethod() {}
}


class SecondSubSuiteTest {
}


@RunWith(Suite.class)
@SuiteClasses({FirstSubSuiteTest.class, SecondSubSuiteTest.class, UnusualTest.class})
@Ignore("Prevents ANT from trying to run these as tests")
class ExampleSubSuite extends AbstractTestSuite {
}


@Ignore("Prevents ANT from trying to run these as tests")
@RunWith(Suite.class)
@SuiteClasses({})
class EmptySuite extends AbstractTestSuite {
}
