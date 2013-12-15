package edu.wpi.first.wpilibj.tables;

/**
 * A listener that listens to changes in values in a {@link ITable}
 * 
 * @author Mitchell
 *
 */
public interface ITableListener {
    /**
     * Called when a key-value pair is changed in a {@link ITable}
     * WARNING: If a new key-value is put in this method value changed will immediatly be called which could lead to recursive code
     * @param source the table the key-value pair exists in
     * @param key the key associated with the value that changed
     * @param value the new value
     * @param isNew true if the key did not previously exist in the table, otherwise it is false
     */
    public void valueChanged(ITable source, String key, Object value, boolean isNew);
}
