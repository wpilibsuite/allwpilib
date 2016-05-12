package edu.wpi.first.wpilibj.networktables2.type;

/**
 * @deprecated Use {@literal ArrayList<Double>} instead.
 */
@Deprecated
public class NumberArray extends ArrayData {
  public double get(int index) {
    return ((Double)getAsObject(index)).doubleValue();
  }
  public void set(int index, double value) {
    _set(index, new Double(value));
  }
  public void add(double value) {
    _add(new Double(value));
  }
}
