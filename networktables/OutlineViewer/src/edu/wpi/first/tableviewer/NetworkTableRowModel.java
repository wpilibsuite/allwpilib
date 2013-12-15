/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.tableviewer;

import org.netbeans.swing.outline.RowModel;

/**
 * And implementation of RowModel for the Outline view.
 *
 * @author Sam
 */
public class NetworkTableRowModel implements RowModel {

    @Override
    public Class getColumnClass(int column) {
        switch (column) {
            case 0:
                return String.class;
            case 1:
                return Object.class;
            default:
                return null;
        }
    }

    @Override
    public int getColumnCount() {
        return 2;
    }

    @Override
    public String getColumnName(int column) {
        switch (column) {
            case 0:
                return "Value";
            case 1:
                return "Type";
            default:
                return "";
        }
    }

    @Override
    public Object getValueFor(Object node, int column) {
        AbstractTreeNode n = (AbstractTreeNode) node;
        return n.getValueFor(column);
    }

    @Override
    public boolean isCellEditable(Object node, int column) {
        try {
            LeafNode leaf = (LeafNode) node;
            return column == 0
                   && leaf.data.getType() != TableEntryData.EntryType.ARRAY
                   && !leaf.data.isMetadata();
        } catch (Exception e) {
            return false;
        }
    }

    @Override
    public void setValueFor(Object node, int column, Object value) {
        if (node instanceof LeafNode) {
            LeafNode l = (LeafNode) node;
            l.setValue(value);
        }
    }
}
