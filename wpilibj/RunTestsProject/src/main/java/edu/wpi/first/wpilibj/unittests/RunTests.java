/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.unittests;

import edu.wpi.first.testing.TestClass;
import edu.wpi.first.wpilibj.SimpleRobot;


/**
 * The VM is configured to automatically run this class, and to call the
 * functions corresponding to each mode, as described in the SimpleRobot
 * documentation. If you change the name of this class or the package after
 * creating this project, you must also update the manifest file in the resource
 * directory.
 */
public class RunTests extends SimpleRobot {
    public class Tags {
        // Test life-cycle
        public class Lifecycle {
            public static final String INDEVEL = "in-dev";
            public static final String INTEST = "in-test";
            public static final String INPRODUCTION = "in-production";
            public static final String DEPRECATED = "deprecated";
        }

        // Type of test
        public class Type {
            public static final String DIGITAL = "digital";
            public static final String ANALOG = "analog";
            public static final String COMMANDBASED = "command-based-robot";
            public static final String PWM = "pwm";
            public static final String PREFERENCES = "preferences";
            public static final String NETWORKTABLES = "networktables";
        }
    }
    
    public void robotInit() {
        System.out.println("!!! WAITING FOR ENABLE");
    }
    /**
     * This function is called once each time the robot enters autonomous mode.
     */
    public void autonomous() {
        
    }

    /**
     * This function is called once each time the robot enters operator control.
     */
    public void operatorControl() {
        System.out.println("!!! TESTS BEGINNING");
        
        runTests();
        
        System.out.println("!!! TESTS COMPLETE");
    }
    
    private void runTests() {
        // Uncomment line below to run tests that are not yet trustworthy
        // Do not check this in!
        //TestClass.runAllWithTag(Tags.Lifecycle.INDEVEL);
        // Uncomment line below to run tests related to networktables
        // Do not check this in!
        //TestClass.runAllWithTag(Tags.Type.NETWORKTABLES);
        TestClass.runAllWithTag(Tags.Lifecycle.INTEST);
        TestClass.runAllWithTag(Tags.Lifecycle.INPRODUCTION);

        System.out.println("##### The following tests (if any) are deprecated #####");
        //TestClass.runAllWithTag(Tags.Lifecycle.DEPRECATED);
    }
}
