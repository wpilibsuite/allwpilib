/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * A Skeleton object to be used with Kinect data from the
 * FRC Kinect server on the DriverStation
 * @author koconnor
 */
public class Skeleton {

    /**
     * The TrackState of the skeleton
     */
    public static class tTrackState {

        /** The integer value representing this enumeration. */
        public final int value;

        protected static final int kNotTracked_val = 0;
        protected static final int kPositionOnly_val = 1;
        protected static final int kTracked_val = 2;

        /** TrackState: Not Tracked */
        public static final tTrackState kNotTracked = new tTrackState(kNotTracked_val);
        /** TrackState: Position Only */
        public static final tTrackState kPositionOnly = new tTrackState(kPositionOnly_val);
        /** TrackState: Tracked */
        public static final tTrackState kTracked = new tTrackState(kTracked_val);

        private tTrackState(int value) {
            this.value = value;
        }
    }

    /**
     * The Joint TrackingState
     */
    public static class tJointTrackingState {
        /** The integer value representing this enumeration. */
        public final int value;

        protected static final int kNotTracked_val = 0;
        protected static final int kInferred_val = 1;
        protected static final int kTracked_val = 2;

        /** Joint TrackingState: Not Tracked */
        public static final tJointTrackingState kNotTracked = new tJointTrackingState(kNotTracked_val);
        /** Joint TrackingState: Inferred */
        public static final tJointTrackingState kInferred = new tJointTrackingState(kInferred_val);
        /** Joint TrackingState: Tracked */
        public static final tJointTrackingState kTracked = new tJointTrackingState(kTracked_val);

        private tJointTrackingState(int value) {
            this.value = value;
        }
    }

    /**
     * An individual Joint from Kinect data
     */
    public class Joint {
        protected float x, y, z;
        protected byte trackingState;

        Joint() {
            x = y = z = 0;
        }

        public float getX() {
            return x;
        }
        public float getY() {
            return y;
        }
        public float getZ() {
            return z;
        }
        public tJointTrackingState getTrackingState() {
            switch(trackingState) {
            case 1:
                return tJointTrackingState.kInferred;
            case 2:
                return tJointTrackingState.kTracked;
            default:
                return tJointTrackingState.kNotTracked;
            }
        }
    }

    /**
     * Helper class used to index the joints in a (@link Skeleton)
     */
    public static class tJointTypes {
        public final int value;

        protected static final int kHipCenter_val = 0;
        protected static final int kSpine_val = 1;
        protected static final int kShoulderCenter_val = 2;
        protected static final int kHead_val = 3;
        protected static final int kShoulderLeft_val = 4;
        protected static final int kElbowLeft_val = 5;
        protected static final int kWristLeft_val = 6;
        protected static final int kHandLeft_val = 7;
        protected static final int kShoulderRight_val = 8;
        protected static final int kElbowRight_val = 9;
        protected static final int kWristRight_val = 10;
        protected static final int kHandRight_val = 11;
        protected static final int kHipLeft_val = 12;
        protected static final int kKneeLeft_val = 13;
        protected static final int kAnkleLeft_val = 14;
        protected static final int kFootLeft_val = 15;
        protected static final int kHipRight_val = 16;
        protected static final int kKneeRight_val = 17;
        protected static final int kAnkleRight_val = 18;
        protected static final int kFootRight_val = 19;
        protected static final int kCount_val = 20;

        public static final tJointTypes kHipCenter = new tJointTypes(kHipCenter_val);
        public static final tJointTypes kSpine = new tJointTypes(kSpine_val);
        public static final tJointTypes kShoulderCenter = new tJointTypes(kShoulderCenter_val);
        public static final tJointTypes kHead = new tJointTypes(kHead_val);
        public static final tJointTypes kShoulderLeft = new tJointTypes(kShoulderLeft_val);
        public static final tJointTypes kElbowLeft = new tJointTypes(kElbowLeft_val);
        public static final tJointTypes kWristLeft = new tJointTypes(kWristLeft_val);
        public static final tJointTypes kHandLeft = new tJointTypes(kHandLeft_val);
        public static final tJointTypes kShoulderRight = new tJointTypes(kShoulderRight_val);
        public static final tJointTypes kElbowRight = new tJointTypes(kElbowRight_val);
        public static final tJointTypes kWristRight = new tJointTypes(kWristRight_val);
        public static final tJointTypes kHandRight = new tJointTypes(kHandRight_val);
        public static final tJointTypes kHipLeft = new tJointTypes(kHipLeft_val);
        public static final tJointTypes kKneeLeft = new tJointTypes(kKneeLeft_val);
        public static final tJointTypes kAnkleLeft = new tJointTypes(kAnkleLeft_val);
        public static final tJointTypes kFootLeft = new tJointTypes(kFootLeft_val);
        public static final tJointTypes kHipRight = new tJointTypes(kHipRight_val);
        public static final tJointTypes kKneeRight = new tJointTypes(kKneeRight_val);
        public static final tJointTypes kAnkleRight = new tJointTypes(kAnkleRight_val);
        public static final tJointTypes kFootRight = new tJointTypes(kFootRight_val);
        public static final tJointTypes kCount = new tJointTypes(kCount_val);

        private tJointTypes(int value) {
            this.value = value;
        }
    }

    public Joint GetHandRight() {
        return skeleton[tJointTypes.kHandRight.value];
    }
    public Joint GetHandLeft() {
        return skeleton[tJointTypes.kHandLeft.value];
    }
    public Joint GetWristRight() {
        return skeleton[tJointTypes.kWristRight.value];
    }
    public Joint GetWristLeft() {
        return skeleton[tJointTypes.kWristLeft.value];
    }
    public Joint GetElbowLeft() {
        return skeleton[tJointTypes.kElbowLeft.value];
    }
    public Joint GetElbowRight() {
        return skeleton[tJointTypes.kElbowRight.value];
    }
    public Joint GetShoulderLeft() {
        return skeleton[tJointTypes.kShoulderLeft.value];
    }
    public Joint GetShoulderRight() {
        return skeleton[tJointTypes.kShoulderRight.value];
    }
    public Joint GetShoulderCenter() {
        return skeleton[tJointTypes.kShoulderCenter.value];
    }
    public Joint GetHead() {
        return skeleton[tJointTypes.kHead.value];
    }
    public Joint GetSpine() {
        return skeleton[tJointTypes.kSpine.value];
    }
    public Joint GetHipCenter() {
        return skeleton[tJointTypes.kHipCenter.value];
    }
    public Joint GetHipRight() {
        return skeleton[tJointTypes.kHipRight.value];
    }
    public Joint GetHipLeft() {
        return skeleton[tJointTypes.kHipLeft.value];
    }
    public Joint GetKneeLeft() {
        return skeleton[tJointTypes.kKneeLeft.value];
    }
    public Joint GetKneeRight() {
        return skeleton[tJointTypes.kKneeRight.value];
    }
    public Joint GetAnkleLeft() {
        return skeleton[tJointTypes.kAnkleLeft.value];
    }
    public Joint GetAnkleRight() {
        return skeleton[tJointTypes.kAnkleRight.value];
    }
    public Joint GetFootLeft() {
        return skeleton[tJointTypes.kFootLeft.value];
    }
    public Joint GetFootRight() {
        return skeleton[tJointTypes.kFootRight.value];
    }
    public Joint GetJoint(tJointTypes index) {
        return skeleton[index.value];
    }

    public tTrackState GetTrackState() {
        return trackState;
    }

    Skeleton() {
        for(int i=0; i<20; i++) {
            skeleton[i] = new Joint();
        }
    }

    protected Joint[] skeleton = new Joint[20];
    protected tTrackState trackState;
}
