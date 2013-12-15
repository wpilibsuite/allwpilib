/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.tableviewer.tableview;

import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.tables.ITableListener;
import javafx.beans.property.SimpleStringProperty;

/**
 *
 * @author Sam
 */
public class EntryData implements ITableListener {

    private final SimpleStringProperty 
            key = new SimpleStringProperty(),
            value = new SimpleStringProperty(),
            type = new SimpleStringProperty();

    public EntryData(ITable table, String key, Object value) {
        table.addTableListener(key, this, true);
        table.addSubTableListener(this);

        setKey(key);
        setValue(value.toString());
        setType(typeFromValue(value));
    }

    public void setKey(String key) {
        this.key.set(key);
    }

    public void setValue(String value) {
        this.value.set(value);
    }

    public void setType(String type) {
        this.type.set(type);
    }

    public String getKey() {
        return key.get();
    }

    public String getValue() {
        return value.get();
    }

    public String getType() {
        return type.get();
    }

    @Override
    public void valueChanged(ITable source, String key, Object value, boolean isNew) {
        System.out.println("Value of " + key + " changed to " + value);
        setKey(key);
        setValue(value.toString());
        setType(typeFromValue(value));
    }

    private String typeFromValue(Object value) {
        String valueClassName = value.getClass().toString().substring(value.getClass().toString().lastIndexOf(".") + 1);
        switch (valueClassName.toLowerCase()) {
            case "boolean":
                return "BOOLEAN";
            case "double":
                return "NUMBER";
            case "string":
                return "STRING";
            default:
                return "UNSUPPORTED";
        }
    }
}
