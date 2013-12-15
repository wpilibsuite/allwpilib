/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2011. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.testing;

import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Set;
import java.util.Vector;

import org.reflections.Reflections;

/**
 * This class should be extended by test classes. Make sure to call the
 * constructor for each subclass once so that the class is added to the master
 * hashtable
 * @author dtjones
 */
public abstract class TestClass {
    static Hashtable testClassesByName = new Hashtable();
    static Hashtable testClassesByTag = new Hashtable();
    Hashtable tests = new Hashtable();
    static Vector failedTests = new Vector();

    
    static {
        Reflections reflections = new Reflections("edu.wpi.first.wpilibj.unittests");

        Set<Class<? extends TestClass>> tests = reflections.getSubTypesOf(TestClass.class);
        for (Class<? extends TestClass> test : tests) {
        	try {
				test.newInstance();
			} catch (InstantiationException | IllegalAccessException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
        }
    }

    /**
     * Create a new instance of the test class and add it to the master table
     * of test classes
     */
    public TestClass () {
        testClassesByName.put(getName(), this);

        String[] tags = getTags();
        for(int i = 0; i < tags.length; i++) {
            if(testClassesByTag.containsKey(tags[i])) {
                // The tag exists; add ourself to it
                ((Vector) testClassesByTag.get(tags[i])).addElement(this);
            } else {
                // The tag does not already exist; create it
                Vector testClassVector = new Vector();
                testClassesByTag.put(tags[i], testClassVector);

                // Add ourself to it
                testClassVector.addElement(this);
            }
        }
    }

    /**
     * This class should be extended within subclasses of each TestClass to
     * add a test. In the constructor for the TestClass you could put :
     *  new Test() {
     *      public String getName() { return "Sample"; }
     *      public void run() {
     *          //your test here
     *          assertFail("I don't work");
     *      }
     *  };
     */
    public abstract class Test {
        private String testName;
        /**
         * Create a new Test and add it to the master classes table of tests
         * @param name The name for this test, This should be unique for this TestClass.
         */
        public Test(String name) {
            this.testName = name;
            tests.put(getName(), this);
        }
        /**
         * Get the name of this test, should be unique within this test class.
         * @return the name of the test
         */
        public String getName () { return this.testName;};
        /**
         * The code to test. Failure is triggered using one of the assert methods
         */
        public abstract void run ();
        /**
         * Setup code to run before this test is run
         */
        public void setup() {}
        /**
         * Teardown code to run after the test is run
         */
        public void teardown() {}
        
        /**
         * Return a string representing this Test.
         * @return The name of the Test.
         */
        public String toString() {
            return getName();
        }

        /**
         * Runs this test case.
         */
        final void test() {
            try {
                setup();
                run();
                onPass();
            } catch (TestFailure e) {
                onFailure(e);
            } finally {
                teardown();
            }
        }

        /**
         * Default failure hook called when a test fails. May be
         * overridden.
         *
         * @param e The TestFailure to report.
         */
        protected void onFailure(TestFailure e) {
//          System.out.println("    " + getName() + " failed : " + e.getMessage());
            System.out.println("!!! TEST FAILED: " + getName() + " with: " + e.getMessage());
            System.out.print("    ");
            e.print(System.out);
            System.out.println();
            e.setTestName(
                TestClass.this.getName(),
                this.getName());
            failedTests.addElement(new Failure(e));
        }

        /**
         * Default pass hook called when a test passes. May be overridden.
         */
        public void onPass() {
            System.out.println("    " + getName() + " passed");
        }
    }

    /**
     * A test which does not emit a test failure sentinel to the console
     * when it fails, such that an automated test harness like Hudson does
     * not mark the build as unstable.
     */
    public abstract class TestUnderDevelopment extends Test {
        public TestUnderDevelopment(String name) {
            super(name);
        }

        protected void onFailure(TestFailure e) {
            System.out.println("### " + getName() + " failed : " + e.getMessage());
            System.out.print("    ");
            e.print(System.out);
            System.out.println();
            e.setTestName(
                TestClass.this.getName(),
                this.getName());
            failedTests.addElement(new Failure(e));
        }
    }

    /**
     * Get the name of this TestClass. This should be unique within the testClassesByName
     * @return The name of this test class
     */
    public abstract String getName();

    /**
     * Returns an array of tag names for which this class is associated.
     *
     * @return An array of tag names that this class is associated with.
     */
    public String[] getTags() {
        return new String[] {};
    }

    /**
     * Setup code to run before the tests within this class are run
     */
    public void setup() {}
    /**
     * Teardown code to run after the tests within this class are run
     */
    public void teardown() {}

    /**
     * Get a list of all the failed tests.
     * @return A list of all of the failed tests.
     */
    public static Failure[] getFailures() {
        Failure[] failures = new Failure[failedTests.size()];
        failedTests.copyInto(failures);
        return failures;
    }

    /**
     * Get a string representing this TestClass.
     * @return The name of this test class.
     */
    public String toString() {
        return getName();
    }

    final void test() {
        System.out.println("Running: " + getName());
        setup();
        Enumeration elements = tests.elements();
        while(elements.hasMoreElements())
            ((Test)elements.nextElement()).test();
        teardown();
    }

    final void test(String test) {
        System.out.println("Running: " + getName());
        setup();
        ((Test)tests.get(test)).test();
        teardown();
    }

    /**
     * Run the tests within the class name given
     * @param clas The name of the TestClass to run
     */
    public static void run(String clas) {
        TestClass testCase = (TestClass)testClassesByName.get(clas);
        testCase.test();
        System.out.println("Completed: " + testCase.getName());
        System.out.println();
    }

    /**
     * Run the given test within the given class
     * @param clas The name of the class to run.
     * @param test The name of the test to run.
     */
    public static void run(String clas, String test) {
        TestClass testCase = (TestClass)testClassesByName.get(clas);
        testCase.test(test);
        System.out.println("Completed: " + testCase.getName());
        System.out.println();
    }

    /**
     * Run all of the tests in all of the classes.
     */
    public static void runAll () {
        Enumeration elements = testClassesByName.elements();
        while(elements.hasMoreElements())
            ((TestClass)elements.nextElement()).test();
        System.out.println("Tests complete");
    }

    /**
     * Runs all tests within test classes whose getTags() result contains the 
     * given string.
     *
     * @param tagName The tag name for which all associated tests should run.
     */
    public static void runAllWithTag(String tagName) {
        Vector tests = (Vector) testClassesByTag.get(tagName);

        if(tests != null) {
            Enumeration elements = tests.elements();
            while(elements.hasMoreElements())
                ((TestClass)elements.nextElement()).test();
            System.out.println("Tests complete");
        }
    }

    /**
     * Fail the test.
     * @param msg A message descibing the failure.
     */
    protected void assertFail (String msg) {
        throw new TestFailure(msg);
    }

    /**
     * Fail the test if the given items are not the same
     * @param expected The expected value.
     * @param actual The actual value.
     */
    protected void assertEquals (Object expected, Object actual) {
        if ((null == expected) ^ (null == actual))
            throw new TestFailure("Expected " + expected + " got " + actual);
        if (!expected.equals(actual))
            throw new TestFailure("Expected " + expected + " got " + actual);
    }

    /**
     * Fail the test if the given items are not the same
     * @param expected The expected value.
     * @param actual The actual value.
     */
    protected void assertEquals (long expected, long actual) {
        if (expected != actual)
            throw new TestFailure("Expected " + expected + " got " + actual);
    }

    /**
     * Fail the test if the given items are not the same
     * @param expected The expected value.
     * @param actual The actual value.
     */
    protected void assertEquals (double expected, double actual) {
        if (expected != actual)
            throw new TestFailure("Expected " + expected + " got " + actual);
    }

    /**
     * Fail the test if the given items are not the same
     * @param expected The expected value.
     * @param actual The actual value.
     * @param tolerance The amount by which the values must match
     */
    protected void assertEquals (double expected, double actual, double tolerance) {
        if (Math.abs(expected - actual) > tolerance)
            throw new TestFailure("Expected " + expected + " got " + actual);
    }

    /**
     * Fail the test if the given value is false.
     * @param value Boolean value that must be true for success
     */
    protected void assertTrue (boolean value) {
        if ( ! value )
            throw new TestFailure("Assertion was false");
    }

    /**
     * Fail the test if the given value is true.
     * @param value Boolean value that must be false for success
     */
    protected void assertFalse (boolean value) {
        if ( value )
            throw new TestFailure("Assertion was true");
    }
}
