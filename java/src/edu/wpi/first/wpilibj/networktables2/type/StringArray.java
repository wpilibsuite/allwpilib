package edu.wpi.first.wpilibj.networktables2.type;

/**
 * @deprecated Use {@literal ArrayList<String>} instead.
 */
public class StringArray extends ArrayData {
  public String get(int index) {
    return ((String)getAsObject(index));
  }
  public void set(int index, String value) {
    _set(index, value);
  }
  public void add(String value) {
    _add(value);
  }
}
