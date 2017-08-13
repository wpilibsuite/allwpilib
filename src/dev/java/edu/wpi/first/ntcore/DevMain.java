package edu.wpi.first.ntcore;

import edu.wpi.first.wpilibj.networktables.NetworkTablesJNI;
import edu.wpi.first.wpiutil.RuntimeDetector;

public class DevMain {
    public static void main(String[] args) {
        System.out.println("Hello World!");
        System.out.println(RuntimeDetector.getPlatformPath());
        NetworkTablesJNI.flush();
    }
}
