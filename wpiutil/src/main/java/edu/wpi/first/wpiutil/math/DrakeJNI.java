/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpiutil.math;

import edu.wpi.first.wpiutil.RuntimeLoader;

import java.io.IOException;
import java.util.concurrent.atomic.AtomicBoolean;

public final class DrakeJNI {
    static boolean libraryLoaded = false;
    static RuntimeLoader<DrakeJNI> loader = null;

    static {
        if (Helper.getExtractOnStaticLoad()) {
            try {
                loader = new RuntimeLoader<>("wpiutiljni", RuntimeLoader.getDefaultExtractionRoot(), DrakeJNI.class);
                loader.loadLibrary();
            } catch (IOException ex) {
                ex.printStackTrace();
                System.exit(1);
            }
            libraryLoaded = true;
        }
    }

    /**
     * Force load the library.
     *
     * @throws IOException If the library could not be loaded or found.
     */
    public static synchronized void forceLoad() throws IOException {
        if (libraryLoaded) {
            return;
        }
        loader = new RuntimeLoader<>("wpiutiljni", RuntimeLoader.getDefaultExtractionRoot(), DrakeJNI.class);
        loader.loadLibrary();
        libraryLoaded = true;
    }

    /**
     * Solves the discrete alegebraic Riccati equation.
     *
     * @param A      Array containing elements of A in row-major order.
     * @param B      Array containing elements of B in row-major order.
     * @param Q      Array containing elements of Q in row-major order.
     * @param R      Array containing elements of R in row-major order.
     * @param states Number of states in A matrix.
     * @param inputs Number of inputs in B matrix.
     * @param S      Array storage for DARE solution.
     */
    public static native void discreteAlgebraicRiccatiEquation(
            double[] A,
            double[] B,
            double[] Q,
            double[] R,
            int states,
            int inputs,
            double[] S);

    public static class Helper {
        private static AtomicBoolean extractOnStaticLoad = new AtomicBoolean(true);

        public static boolean getExtractOnStaticLoad() {
            return extractOnStaticLoad.get();
        }

        public static void setExtractOnStaticLoad(boolean load) {
            extractOnStaticLoad.set(load);
        }
    }
}
