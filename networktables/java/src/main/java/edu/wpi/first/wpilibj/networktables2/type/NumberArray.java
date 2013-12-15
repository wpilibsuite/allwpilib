package edu.wpi.first.wpilibj.networktables2.type;

/**
 *
 * @author Mitchell
 */
public class NumberArray extends ArrayData{

    private static final byte NUMBER_ARRAY_RAW_ID = 0x11;
    public static final ArrayEntryType TYPE = new ArrayEntryType(NUMBER_ARRAY_RAW_ID, DefaultEntryTypes.DOUBLE, NumberArray.class);


    public NumberArray(){
        super(TYPE);
    }
    
    public double get(int index){
        return ((Double)getAsObject(index)).doubleValue();
    }
    public void set(int index, double value){
        _set(index, new Double(value));//TODO cache double values
    }
    public void add(double value){
        _add(new Double(value));//TODO cache double values
    }
}
