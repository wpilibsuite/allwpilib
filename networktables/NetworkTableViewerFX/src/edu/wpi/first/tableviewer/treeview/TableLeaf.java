/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.tableviewer.treeview;

import edu.wpi.first.tableviewer.EntryData;
import edu.wpi.first.wpilibj.tables.ITable;
import javafx.application.Platform;

/**
 *
 * @author Sam
 */
public class TableLeaf extends ITableItem implements Comparable {

    private final ITable table;

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
        entryData = new EntryData(key, value, getTypeFromValue(value));
        entryData.setTableItem(this);
        setValue(entryData);
    }

    public ITable getTable() {
        return table;
    }

    public String getTableKey() {
        return entryData.getKey();
    }

    public Object getTableValue() {
        return entryData.getValue();
    }

    public EntryType getEntryType() {
        return entryData.getType();
    }

    /**
     * Sets the display value of this TableLeaf to a new value.
     *
     * @param newValue The new value of this leaf.
     */
    public void setTableValue(Object newValue) {
        entryData.setValue(newValue);
        entryData.setType(getTypeFromValue(newValue));
        Platform.runLater(new Runnable() {
            @Override
            public void run() {
                setValue(EntryData.EMPTY);
                setValue(entryData);
            }
        });
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
//        table.putValue(entryData.getKey(), newValue);
        setTableValue(newValue);
    }

    @Override
    public boolean isLeaf() {
        return true;
    }
}
