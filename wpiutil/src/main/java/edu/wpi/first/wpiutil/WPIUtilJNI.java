/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpiutil;

import java.io.IOException;
import java.util.concurrent.atomic.AtomicBoolean;

public final class WPIUtilJNI {
  static boolean libraryLoaded = false;
  static RuntimeLoader<WPIUtilJNI> loader = null;

  public static class Helper {
    private static AtomicBoolean extractOnStaticLoad = new AtomicBoolean(true);

    public static boolean getExtractOnStaticLoad() {
      return extractOnStaticLoad.get();
    }

    public static void setExtractOnStaticLoad(boolean load) {
      extractOnStaticLoad.set(load);
    }
  }

  static {
    if (Helper.getExtractOnStaticLoad()) {
      try {
        loader = new RuntimeLoader<>("wpiutiljni", RuntimeLoader.getDefaultExtractionRoot(), WPIUtilJNI.class);
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
   */
  public static synchronized void forceLoad() throws IOException {
    if (libraryLoaded) {
      return;
    }
    loader = new RuntimeLoader<>("wpiutiljni", RuntimeLoader.getDefaultExtractionRoot(), WPIUtilJNI.class);
    loader.loadLibrary();
    libraryLoaded = true;
  }

  public static native void addPortForwarder(int port, String remoteHost, int remotePort);
  public static native void removePortForwarder(int port);

  /**
   * Computes the matrix exp.
   *
   * @param src  Array of elements of the matrix to be exponentiated.
   * @param rows how many rows there are.
   * @param dst  Array where the result will be stored.
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
}
