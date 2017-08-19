package edu.wpi.cscore;

import edu.wpi.cscore.CameraServerJNI;
import edu.wpi.first.wpiutil.RuntimeDetector;

public class DevMain {
    public static void main(String[] args) {
        System.out.println("Hello World!");
        System.out.println(RuntimeDetector.getPlatformPath());
        System.out.println(CameraServerJNI.getHostname());
    }
}
