/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.tableviewer;

import java.util.Arrays;
import java.util.HashMap;

/**
 *
 * @author Sam
 */
public class TableEntryData {

    private String key;
    private Object value;
    private EntryType type;

    /**
     * An enum-like structure to hold the type of data this entry contains.
     * Several are pre-baked and can be used like enum constants:
     * <pre>
     *  ARRAY
     *  BOOLEAN
     *  METADATA
     *  NUMBER
     *  STRING
     * </pre> Use {@link #getEntryType(String name) getEntryType} to grab one
     * that isn't pre-baked. This is typically used to show custom metadata
     * information from a {@code ~TYPE~} entry in a
     * {@link BranchNode BranchNode's} {@code Type} cell.
     */
    public static final class EntryType {

        private static final HashMap<String, EntryType> ENTRY_TYPES = new HashMap<>();
        public static final EntryType ARRAY = new EntryType("Array");
        public static final EntryType BOOLEAN = new EntryType("Boolean");
        public static final EntryType METADATA = new EntryType("Metadata");
        public static final EntryType NUMBER = new EntryType("Number");
        public static final EntryType STRING = new EntryType("String");
        public static final EntryType UNSUPPORTED = new EntryType("Unsupported");
        private final String name;

        private EntryType(final String name) {
            this.name = name;
            ENTRY_TYPES.put(name, this);
        }

        public static EntryType getEntryType(String name) {
            EntryType type = ENTRY_TYPES.get(name);
            if (type == null) {
                type = new EntryType(name);
            }
            return type;
        }

        @Override
        public String toString() {
            return name;
        }
    }

    public TableEntryData(String key, Object value) {
        this.key = key;
        if (value != null) {
            this.value = value;
            this.type = typeFromValue(value);
            if (type.equals(EntryType.ARRAY)) {
                this.value = Arrays.toString((Object[]) value);
            }
        }
    }

    public String getKey() {
        return key;
    }

    public Object getValue() {
        return value;
    }

    public EntryType getType() {
        return type;
    }

    /**
     * Sets the value of this TableEntrydata. Does not change the type.
     */
    public void setValue(Object newValue) {
        this.value = newValue;
        if (newValue instanceof Object[]) {
            this.value = Arrays.toString((Object[]) newValue);
        }
    }

    /**
     * Explicitly changes the type of this data. Used on BranchNodes when a
     * metadata entry comes in and to show what kind of system it shows data
     * from (such as "Speed controller", "Subsystem", etc.).
     */
    public void setType(EntryType type) {
        this.type = type;
    }

    /**
     * Generates an {@link EntryType} based on the value of the table entry.
     */
    private EntryType typeFromValue(Object value) {
        String valueClassName = value.getClass().toString().substring(value.getClass().toString().lastIndexOf(".") + 1);
        if (isMetadata()) {
            return EntryType.METADATA;
        }
        switch (valueClassName.toLowerCase()) {
            case "boolean":
                return EntryType.BOOLEAN;
            case "double":
                return EntryType.NUMBER;
            case "string":
                return EntryType.STRING;
            case "object;": // the semicolon is neccessary to catch arrays
                return EntryType.ARRAY;
            default:
                return EntryType.UNSUPPORTED;
        }
    }

    /**
     * Sees if the data within this structure is metadata (i.e. has a key
     * bookended by tildes ("~") and is in all caps). Used to show/hide metadata
     * leaves in branches.
     */
    public boolean isMetadata() {
        return key.startsWith("~")
               && key.endsWith("~")
               && key.toUpperCase().equals(key);
    }
}
