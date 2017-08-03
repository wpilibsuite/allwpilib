package edu.wpi.first.wpilibj.networktables2.type;

/**
 * @deprecated Use {@literal ArrayList<Boolean>} instead.
 */
@Deprecated
public class BooleanArray extends ArrayData {
  public boolean get(int index) {
    return ((Boolean)getAsObject(index)).booleanValue();
  }
  public void set(int index, boolean value) {
    _set(index, value?Boolean.TRUE:Boolean.FALSE);
  }
  public void add(boolean value) {
    _add(value?Boolean.TRUE:Boolean.FALSE);
  }
}
