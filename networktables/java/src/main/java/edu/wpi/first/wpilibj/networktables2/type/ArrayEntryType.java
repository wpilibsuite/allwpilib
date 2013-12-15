package edu.wpi.first.wpilibj.networktables2.type;

import edu.wpi.first.wpilibj.networktables2.TableKeyExistsWithDifferentTypeException;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

/**
 *
 * @author Mitchell
 */
public class ArrayEntryType extends ComplexEntryType {//TODO allow for array of complex type
    private final NetworkTableEntryType elementType;
    private final Class externalArrayType;

    public ArrayEntryType(byte id, NetworkTableEntryType elementType, Class externalArrayType) {
        super(id, "Array of [" + elementType.name + "]");
        if(!ArrayData.class.isAssignableFrom(externalArrayType))
            throw new RuntimeException("External Array Data Type must extend ArrayData");
        this.externalArrayType = externalArrayType;
        this.elementType = elementType;
    }

    public void sendValue(Object value, DataOutputStream os) throws IOException {
        if (value instanceof Object[]) {
            Object[] dataArray = (Object[]) value;
            if (dataArray.length > 255) {
                throw new IOException("Cannot write " + value + " as " + name + ". Arrays have a max length of 255 values");
            }
            os.writeByte(dataArray.length);
            for (int i = 0; i < dataArray.length; ++i) {
                elementType.sendValue(dataArray[i], os);
            }
        } else {
            throw new IOException("Cannot write " + value + " as " + name);
        }
    }

    public Object readValue(DataInputStream is) throws IOException {
        int length = is.readUnsignedByte();
        Object[] dataArray = new Object[length];//TODO cache object arrays
        for (int i = 0; i < length; ++i) {
            dataArray[i] = elementType.readValue(is);
        }
        return dataArray;
    }

    public Object internalizeValue(String key, Object externalRepresentation, Object currentInteralValue) {
        if (externalArrayType.isInstance(externalRepresentation)) {
            ArrayData externalArrayData = (ArrayData)externalRepresentation;
            Object[] internalArray;
            if(currentInteralValue instanceof Object[]
                    && (internalArray=((Object[])currentInteralValue)).length==externalArrayData.size()){
                System.arraycopy(externalArrayData.getDataArray(), 0, internalArray, 0, internalArray.length);
                return internalArray;
            }
            else{
                internalArray = new Object[externalArrayData.size()];
                System.arraycopy(externalArrayData.getDataArray(), 0, internalArray, 0, internalArray.length);
                return internalArray;
            }
        }
        throw new TableKeyExistsWithDifferentTypeException(key, this, externalRepresentation+" is not a "+externalArrayType);
    }

    public void exportValue(String key, Object internalData, Object externalRepresentation) {
        if (!externalArrayType.isInstance(externalRepresentation))
            throw new TableKeyExistsWithDifferentTypeException(key, this, externalRepresentation+" is not a "+externalArrayType);
        if(!(internalData instanceof Object[]))
            throw new TableKeyExistsWithDifferentTypeException(key, this, "Internal data: "+internalData+" is not an array");
        
        Object[] internalArray = (Object[])internalData;
        ArrayData externalArrayData = (ArrayData)externalRepresentation;
        externalArrayData.setSize(internalArray.length);
        System.arraycopy(internalArray, 0, externalArrayData.getDataArray(), 0, internalArray.length);
    }
    
    
    
}