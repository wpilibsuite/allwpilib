package edu.wpi.first.wpilibj.networktables2.type;

/**
 *
 * @author Mitchell
 */
public class ArrayData extends ComplexData{
    private final ArrayEntryType type;
    private Object[] data = new Object[0];
    public ArrayData(ArrayEntryType type){
        super(type);
        this.type = type;
    }
    
    protected Object getAsObject(int index){
        return data[index];
    }
    protected void _set(int index, Object value){
        data[index] = value;
    }
    protected void _add(Object value){
        setSize(size()+1);
        data[size()-1] = value;
    }
    public void remove(int index){
        if(index<0 || index>=size())
            throw new IndexOutOfBoundsException();
        if(index < size()-1)
            System.arraycopy(data, index+1, data, index, size()-index-1);
        setSize(size()-1);
    }
    public void setSize(int size){
        if(size==data.length)
            return;
        Object[] newArray = new Object[size];//TODO cache arrays
        if(size<data.length)
            System.arraycopy(data, 0, newArray, 0, size);
        else{
            System.arraycopy(data, 0, newArray, 0, data.length);
            for(int i = data.length; i<newArray.length; ++i)
                newArray[i] = null;
        }
        data = newArray;
    }
    public int size(){
        return data.length;
    }

    Object[] getDataArray() {
        return data;
    }
}
