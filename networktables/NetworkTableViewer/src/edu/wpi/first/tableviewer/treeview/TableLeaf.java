/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.tableviewer.treeview;

import edu.wpi.first.wpilibj.tables.ITable;

/**
 *
 * @author Sam
 */
public class TableLeaf extends ITableItem implements Comparable {

    private final ITable table;
    private String entryKey;
    private Object entryValue;
    private EntryType entryType = EntryType.UNSUPPORTED;

    @Override
    public int compareTo(Object o) {
        return this.getTableKey().toLowerCase().compareTo(((TableLeaf)o).getTableKey().toLowerCase());
    }

    public enum EntryType {
        BOOLEAN, NUMBER, STRING, UNSUPPORTED
    }

    public TableLeaf(ITable table, String key, Object value) {
        super();
        this.table = table;
        this.entryKey = key;
        this.entryValue = value;
        this.entryType = getTypeFromValue(value);
        setValue(this);
    }

    public ITable getTable() {
        return table;
    }

    public String getTableKey() {
        return entryKey;
    }

    public Object getTableValue() {
        return entryValue;
    }

    /**
     * Sets the display value of this TableLeaf to a new value.
     *
     * @param newValue The new value of this leaf.
     */
    public void setTableValue(Object newValue) {
        entryValue = newValue;
        entryType = getTypeFromValue(newValue);
        setValue(null);
        setValue(this);
        
    }

    private EntryType getTypeFromValue(Object value) {
        String valueClassName = value.getClass().toString().substring(value.getClass().toString().lastIndexOf(".") + 1);
        switch (valueClassName.toLowerCase()) {
            case "boolean":
                return EntryType.BOOLEAN;
            case "double":
                return EntryType.NUMBER;
            case "string":
                return EntryType.STRING;
            default:
                return EntryType.UNSUPPORTED;
        }
    }

    /**
     * Updates the value in this TableLeaf's ITable and updates the displayed
     * value.
     *
     * @param newValue The new value of this entry.
     * @see #setTableValue
     */
    public void updateValue(Object newValue) {
        setTableValue(newValue);
        table.putValue(entryKey, entryValue);
    }

    public EntryType getType() {
        return entryType;
    }

    @Override
    public String toString() {
        return entryKey + " = " + entryValue;
    }

    @Override
    public boolean isLeaf() {
        return true;
    }
}
