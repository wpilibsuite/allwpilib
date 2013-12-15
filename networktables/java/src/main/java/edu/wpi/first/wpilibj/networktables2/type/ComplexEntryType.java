package edu.wpi.first.wpilibj.networktables2.type;

/**
 *
 * @author Mitchell
 */
public abstract class ComplexEntryType extends NetworkTableEntryType{
    
    protected ComplexEntryType(byte id, String name){
            super(id, name);
    }
    
    public abstract Object internalizeValue(String key, Object externalRepresentation, Object currentInteralValue);
    public abstract void exportValue(String key, Object internalData, Object externalRepresentation);
    
}
