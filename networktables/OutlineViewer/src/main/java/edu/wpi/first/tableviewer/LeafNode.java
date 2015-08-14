/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.tableviewer;

import edu.wpi.first.wpilibj.networktables.NetworkTable;
import javax.swing.event.TableModelEvent;

/**
 * Responsible for showing data inside a single NetworkTableEntry: a key, a
 * value, and a type that has been extrapolated from the value.
 *
 * @author Sam
 */
public class LeafNode extends AbstractTreeNode {

    public LeafNode(String key, TableEntryData data) {
        super(key, data);
        table = NetworkTable.getTable(key.substring(0, key.lastIndexOf('/')));
    }

    /**
     * Changes the value of the data displayed within this leaf. This is only a
     * graphical update; use #setValue to actually change the value in the
     * NetworkTable.
     *
     * @param newValue The new value for this leaf to show.
     */
    public void updateValue(Object newValue) {
        data.setValue(newValue);
        outline.tableChanged(new TableModelEvent(outline.getModel()));
    }

    /**
     * Pushes the value of this leaf to the NetworkTable associated with it.
     *
     * @param newValue The value to send to the NetworkTable.
     */
    public void setValue(Object newValue) {
        String v = newValue.toString();
        switch (data.getType().toString()) {
            case "Boolean":
                table.putBoolean(data.getKey(), v.equalsIgnoreCase("true"));
                break;
            case "Number":
                try {
                    table.putNumber(data.getKey(), Double.parseDouble(v));
                } catch (NumberFormatException e) {
                    System.err.println("Invalid number " + v);
                }
                break;
            case "String":
                table.putString(data.getKey(), v);
                break;
            default:
                System.err.println("Cannot edit a value of type " + data.getType());
                break;
        }
    }

    @Override
    public boolean isLeaf() {
        return true;
    }

    @Override
    public String toString() {
        return data.getKey();
    }
}
