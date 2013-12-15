package edu.wpi.first.table_viewer;

import java.util.Arrays;

import javax.swing.tree.DefaultTreeCellRenderer;

public class DefaultNetworkTablesValueRenderer extends DefaultTreeCellRenderer {
    
    public void setValue(Object value) {
        if (value == null) {
            setText("null");
        } else if (value instanceof Object[]) {
            setText(Arrays.toString((Object[]) value));
        } else {
            setText(value.toString());
        }
    }
}