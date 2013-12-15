package edu.wpi.first.wpilibj.networktables2.type;

/**
 *
 * @author Mitchell
 */
public class StringArray extends ArrayData{

    private static final byte STRING_ARRAY_RAW_ID = 0x12;
    public static final ArrayEntryType TYPE = new ArrayEntryType(STRING_ARRAY_RAW_ID, DefaultEntryTypes.STRING, StringArray.class);


    public StringArray(){
        super(TYPE);
    }
    
    public String get(int index){
        return ((String)getAsObject(index));
    }
    public void set(int index, String value){
        _set(index, value);
    }
    public void add(String value){
        _add(value);
    }
}
