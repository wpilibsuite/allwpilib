/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2011. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.testing;

/**
 *
 * @author dtjones
 */
public class Failure {
    TestFailure failure;
    Failure(TestFailure failure) {
        this.failure = failure;
    }

    /**
     * Get a string represention of the failed test.
     * @return A string representation of the failed test.
     */
    public String toString() {
        return getTest() + " " + getTestClass() + " : " + failure.getMessage();
    }

    /**
     * Get the name of the test that failed.
     * @return The name of the test that failed.
     */
    public String getTest() {
        return failure.getTest();
    }

    /**
     * Get the name of the test class that failed.
     * @return The name of the test class that failed.
     */
    public String getTestClass() {
        return failure.getTestClass();
    }

}
