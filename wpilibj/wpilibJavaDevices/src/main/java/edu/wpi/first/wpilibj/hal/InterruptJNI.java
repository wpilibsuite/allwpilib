package edu.wpi.first.wpilibj.hal;

import java.nio.ByteBuffer;
import java.nio.IntBuffer;

public class InterruptJNI extends JNIWrapper {
	public interface InterruptJNIHandlerFunction {
		void apply(int interruptAssertedMask, Object param);
	};
	public static native void initializeInterruptJVM(IntBuffer status);
	public static native ByteBuffer initializeInterrupts(int interruptIndex, byte watcher, IntBuffer status);
	public static native void cleanInterrupts(ByteBuffer interrupt_pointer, IntBuffer status);
	public static native void waitForInterrupt(ByteBuffer interrupt_pointer, double timeout, IntBuffer status);
	public static native void enableInterrupts(ByteBuffer interrupt_pointer, IntBuffer status);
	public static native void disableInterrupts(ByteBuffer interrupt_pointer, IntBuffer status);
	public static native double readInterruptTimestamp(ByteBuffer interrupt_pointer, IntBuffer status);
	public static native void requestInterrupts(ByteBuffer interrupt_pointer, byte routing_module, int routing_pin, byte routing_analog_trigger, IntBuffer status);
	public static native void attachInterruptHandler(ByteBuffer interrupt_pointer, InterruptJNIHandlerFunction handler, Object param, IntBuffer status);
	public static native void setInterruptUpSourceEdge(ByteBuffer interrupt_pointer, byte risingEdge, byte fallingEdge, IntBuffer status);
}
