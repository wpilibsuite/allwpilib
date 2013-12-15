/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2011. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.testing;

import java.io.PrintStream;


/**
 * This Exception is thrown when a test fails.
 * @author dtjones
 */
class TestFailure extends RuntimeException {
    private String testClass = null;
    private String test = null;
    /**
     * An execution point representing the line where the assertion that failed
     * was called.
     */
    protected final StackTraceElement failedTest;
    /**
     * Create a new TestFailure exception to fail the currently runningt test.
     * @param msg The message to pass with the exception explaining what failed
     *  in the test
     */
    protected TestFailure (String msg) {
        super(msg);
        failedTest = Thread.currentThread().getStackTrace()[3];
        // failedTest = VM.reifyCurrentStack(-1)[3];
    }

    void setTestName(String testClass, String test) {
        this.testClass = testClass;
        this.test = test;
    }

    /**
     * Get the name of the test class
     * @return the name of the test class
     */
    public String getTestClass() {
        return testClass;
    }

    /**
     * Get the name of the test
     * @return the name of the test
     */
    public String getTest() {
        return test;
    }
    
    public void print(PrintStream out) {
    	out.print(failedTest.toString());
    }
}
