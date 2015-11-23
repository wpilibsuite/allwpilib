package edu.wpi.first.wpilibj.hal;

public class DigitalGlitchFilterJNI extends JNIWrapper {
  public static native void setFilterSelect(long digital_port_pointer, int filter_index);
  public static native int getFilterSelect(long digital_port_pointer);
  public static native void setFilterPeriod(int filter_index, int fpga_cycles);
  public static native int getFilterPeriod(int filter_index);
}
