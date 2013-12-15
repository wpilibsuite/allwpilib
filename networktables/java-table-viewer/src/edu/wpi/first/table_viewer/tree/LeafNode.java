/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.table_viewer.tree;

import edu.wpi.first.table_viewer.dialog.EditBooleanDialog;
import edu.wpi.first.table_viewer.dialog.EditNumberDialog;
import edu.wpi.first.table_viewer.dialog.EditStringDialog;
import edu.wpi.first.wpilibj.tables.ITable;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;
import javax.swing.JSeparator;

/**
 *
 * @author Sam
 */
public class LeafNode extends ITableNode {

    private ITable table;
    private String key;
    private boolean insertedEdit = false;

    public LeafNode(ITable table, String key, Object value) {
        super(table, new TableEntryData(key, value), false);
        table.addTableListener(key, this, true);
        this.table = table;
        this.key = key;
    }

    public String getKey() {
        return key;
    }

    public String getValue() {
        return ((TableEntryData) userObject).getValue();
    }

    public String getType() {
        return ((TableEntryData) userObject).getType();

    }

    public void setTableValue(Object newValue) {
        table.putValue(key, newValue);
    }

    @Override
    public boolean isLeaf() {
        return true;
    }

    @Override
    public void valueChanged(ITable source, String key, Object value, boolean isNew) {
        setUserObject(new TableEntryData(key, value));
        treeModel.reload(this);
    }

    @Override
    public JPopupMenu getMenu() {
        if (!insertedEdit) {
            JMenuItem editorItem = new JMenuItem("Edit");
            editorItem.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    if (getType().equals("BOOLEAN")) {
                        new EditBooleanDialog(LeafNode.this).setVisible(true);
                    } else if (getType().equals("NUMBER")) {
                        new EditNumberDialog(LeafNode.this).setVisible(true);
                    } else if (getType().equals("STRING")) {
                        new EditStringDialog(LeafNode.this).setVisible(true);
                    }
                }
            });
            menu.add(editorItem, 0);
            menu.add(new JSeparator(), 1);
            insertedEdit = true;
        }
        return menu;
    }
}
