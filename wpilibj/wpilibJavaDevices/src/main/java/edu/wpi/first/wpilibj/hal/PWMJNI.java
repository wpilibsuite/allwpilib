package edu.wpi.first.wpilibj.hal;

import java.nio.IntBuffer;
import java.nio.ByteBuffer;

import edu.wpi.first.wpilibj.SensorBase;


public class PWMJNI extends DIOJNI {
	public static native boolean allocatePWMChannel(ByteBuffer digital_port_pointer, IntBuffer status);
	public static native void freePWMChannel(ByteBuffer digital_port_pointer, IntBuffer status);
	public static native void setPWM(ByteBuffer digital_port_pointer, short value, IntBuffer status);
	public static native short getPWM(ByteBuffer digital_port_pointer, IntBuffer status);
	public static native void latchPWMZero(ByteBuffer digital_port_pointer, IntBuffer status);
	public static native void setPWMPeriodScale(ByteBuffer digital_port_pointer, int squelchMask, IntBuffer status);
	public static native ByteBuffer allocatePWM(IntBuffer status);
	public static native void freePWM(ByteBuffer pwmGenerator, IntBuffer status);
	public static native void setPWMRate(double rate, IntBuffer status);
	public static native void setPWMDutyCycle(ByteBuffer pwmGenerator, double dutyCycle, IntBuffer status);
	public static native void setPWMOutputChannel(ByteBuffer pwmGenerator, int pin, IntBuffer status);
}
