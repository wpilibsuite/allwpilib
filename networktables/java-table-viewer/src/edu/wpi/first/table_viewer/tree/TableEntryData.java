/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.table_viewer.tree;

import edu.wpi.first.wpilibj.networktables2.type.NumberArray;
import java.text.DecimalFormat;
import java.util.Arrays;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author Sam
 */
public class TableEntryData {

    private Object key, value, type;

    public TableEntryData(String key, Object value) {
        this.key = key;
        try {
            this.value = new ArrayWrapper(value);
        } catch (Exception ex) {
            if (value instanceof Number) {
                DecimalFormat format = new DecimalFormat("#.#####");
                String dblVal = format.format((double) value);
                this.value = dblVal;
            } else {
                this.value = value;
            }
        }
        this.type = typeFromValue(value);
    }

    public String getKey() {
        return key.toString();
    }

    public String getValue() {
        return value.toString();
    }

    public String getType() {
        return type.toString();
    }

    @Override
    public String toString() {
        return key + space + value + space + type;
    }
    private final String space = "          ";

    private String typeFromValue(Object value) {
        String valueClassName = value.getClass().toString().substring(value.getClass().toString().lastIndexOf(".") + 1);
        switch (valueClassName.toLowerCase()) {
            case "boolean":
                return "BOOLEAN";
            case "double":
                return "NUMBER";
            case "string":
                return "STRING";
            case "object;": // the semicolon is required to catch arrays
                return "ARRAY";
            default:
                return valueClassName.toUpperCase();
        }
    }

    private class ArrayWrapper {

        private Object[] array;

        ArrayWrapper(Object array) throws Exception {
            if (array instanceof Object[]) {
                this.array = (Object[]) array;
            } else {
                throw new RuntimeException();
            }
        }

        @Override
        public String toString() {
            String arrStr = Arrays.toString(array);
            if (arrStr.length() > 2) {
                return arrStr.substring(1, arrStr.length() - 1);
            } else {
                return "Empty";
            }
        }
    }
}