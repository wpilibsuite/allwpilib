package edu.wpi.first.wpilibj.math;

import edu.wpi.first.wpiutil.RuntimeLoader;
import edu.wpi.first.wpiutil.math.DrakeJNI;

import java.io.IOException;
import java.util.concurrent.atomic.AtomicBoolean;

public final class StateSpaceUtilsJNI {
    static boolean libraryLoaded = false;
    static RuntimeLoader<DrakeJNI> loader = null;

    static {
        if (Helper.getExtractOnStaticLoad()) {
            try {
                loader = new RuntimeLoader<>("WPILibStateSpaceDriver", RuntimeLoader.getDefaultExtractionRoot(), DrakeJNI.class);
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
     * @throws IOException if the library could not be found.
     */
    public static synchronized void forceLoad() throws IOException {
        if (libraryLoaded) {
            return;
        }
        loader = new RuntimeLoader<>("WPILibStateSpaceDriver", RuntimeLoader.getDefaultExtractionRoot(), DrakeJNI.class);
        loader.loadLibrary();
        libraryLoaded = true;
    }

    /**
     * Computes the matrix exp.
     *
     * @param src  Array representing the matrix to be exponentiated.
     * @param rows how many rows there are.
     * @param dst  where the result will be stored.
     */
    public static native void exp(double[] src, int rows, double[] dst);

    /**
     * Returns true if (A, B) is a stabilizable pair.
     * <p>
     * (A,B) is stabilizable if and only if the uncontrollable eigenvalues of A, if
     * any, have absolute values less than one, where an eigenvalue is
     * uncontrollable if rank(lambda * I - A, B) &lt; n where n is number of states.
     *
     * @param states the number of states of the system.
     * @param inputs the number of inputs to the system.
     * @param A      System matrix.
     * @param B      Input matrix.
     * @return If the system is stabilizable.
     */
    public static native boolean isStabilizable(int states, int inputs, double[] A, double[] B);

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
