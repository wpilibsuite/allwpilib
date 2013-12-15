/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * This interface allows for PIDController to automatically read from this
 * object
 * @author dtjones
 */
public interface PIDSource {

    /**
     * A description for the type of output value to provide to a PIDController
     */
    public static class PIDSourceParameter {
        public final int value;
        static final int kDistance_val = 0;
        static final int kRate_val = 1;
        static final int kAngle_val = 2;
        public static final PIDSourceParameter kDistance = new PIDSourceParameter(kDistance_val);
        public static final PIDSourceParameter kRate = new PIDSourceParameter(kRate_val);
        public static final PIDSourceParameter kAngle = new PIDSourceParameter(kAngle_val);

        private PIDSourceParameter(int value) {
            this.value = value;
        }
    }

    /**
     * Get the result to use in PIDController
     * @return the result to use in PIDController
     */
    public double pidGet();
}
