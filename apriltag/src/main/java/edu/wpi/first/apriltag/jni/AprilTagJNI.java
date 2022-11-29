/*
Copyright (c) 2022 Photon Vision. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   * Neither the name of FIRST, WPILib, nor the names of other WPILib
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY FIRST AND OTHER WPILIB CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY NONINFRINGEMENT AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL FIRST OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

package edu.wpi.first.apriltag.jni;

import edu.wpi.first.util.RuntimeLoader;
import java.io.IOException;
import java.util.concurrent.atomic.AtomicBoolean;
import org.opencv.core.Mat;

public class AprilTagJNI {
  static boolean libraryLoaded = false;

  static RuntimeLoader<AprilTagJNI> loader = null;

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
        loader =
            new RuntimeLoader<>(
                "apriltagjni", RuntimeLoader.getDefaultExtractionRoot(), AprilTagJNI.class);
        loader.loadLibrary();
      } catch (IOException ex) {
        ex.printStackTrace();
        System.exit(1);
      }
      libraryLoaded = true;
    }
  }

  // Returns a pointer to a apriltag_detector_t
  public static native long aprilTagCreate(
      String fam, double decimate, double blur, int threads, boolean debug, boolean refine_edges);

  // Destroy and free a previously created detector.
  public static native void aprilTagDestroy(long detector);

  private static native Object[] aprilTagDetectInternal(
      long detector,
      long imgAddr,
      int rows,
      int cols,
      boolean doPoseEstimation,
      double tagWidth,
      double fx,
      double fy,
      double cx,
      double cy,
      int nIters);

  // Detect targets given a GRAY frame. Returns a pointer toa zarray
  public static DetectionResult[] aprilTagDetect(
      long detector,
      Mat img,
      boolean doPoseEstimation,
      double tagWidth,
      double fx,
      double fy,
      double cx,
      double cy,
      int nIters) {
    return (DetectionResult[])
        aprilTagDetectInternal(
            detector,
            img.dataAddr(),
            img.rows(),
            img.cols(),
            doPoseEstimation,
            tagWidth,
            fx,
            fy,
            cx,
            cy,
            nIters);
  }
}
