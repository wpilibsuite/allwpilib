package edu.wpi.first.wpilibj.hal;

import java.nio.IntBuffer;
import java.nio.ByteBuffer;

public class InterruptJNI extends JNIWrapper {
	public interface InterruptHandlerFunction {
		void apply(int interruptAssertedMask, ByteBuffer param);
	};
	public static native ByteBuffer initializeInterrupts(int interruptIndex, byte watcher, IntBuffer status);
	public static native void cleanInterrupts(ByteBuffer interrupt_pointer, IntBuffer status);
	public static native void waitForInterrupt(ByteBuffer interrupt_pointer, double timeout, IntBuffer status);
	public static native void enableInterrupts(ByteBuffer interrupt_pointer, IntBuffer status);
	public static native void disableInterrupts(ByteBuffer interrupt_pointer, IntBuffer status);
	public static native double readInterruptTimestamp(ByteBuffer interrupt_pointer, IntBuffer status);
	public static native void requestInterrupts(ByteBuffer interrupt_pointer, byte routing_module, int routing_pin, byte routing_analog_trigger, IntBuffer status);
	public static native void attachInterruptHandler(ByteBuffer interrupt_pointer, InterruptHandlerFunction handler, ByteBuffer param, IntBuffer status);
	public static native void setInterruptUpSourceEdge(ByteBuffer interrupt_pointer, byte risingEdge, byte fallingEdge, IntBuffer status);
}
