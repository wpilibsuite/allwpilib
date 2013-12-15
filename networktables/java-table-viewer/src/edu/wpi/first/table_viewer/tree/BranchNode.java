/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.table_viewer.tree;

import edu.wpi.first.wpilibj.tables.ITable;
import java.awt.Menu;
import java.util.HashMap;
import java.util.Map;
import javax.swing.JPopupMenu;

/**
 *
 * @author Sam
 */
public class BranchNode extends ITableNode {

    private Map<String, ITableNode> entries = new HashMap<>();

    public BranchNode(ITable table) {
        this(table, "NetworkTables");
    }

    public BranchNode(ITable table, String name) {
        super(table, name, true);
        table.addTableListener(this, true);
        table.addSubTableListener(this);
        setAllowsChildren(true);
    }

    @Override
    public void valueChanged(ITable source, String key, Object value, boolean isNew) {
        if (isNew && !entries.containsKey(key)) {
            if (value.toString().startsWith("NetworkTable: /")) {
                String tableName = value.toString().substring(source.toString().length() + 1);
                BranchNode branch = new BranchNode(source.getSubTable(tableName), key);
                this.add(branch);
                entries.put(key, branch);
            } else {
                LeafNode leaf = new LeafNode(source, key, value);
                this.add(leaf);
                entries.put(key, leaf);
            }
        }
    }

    @Override
    public boolean isLeaf() {
        return false;
    }

    @Override
    public JPopupMenu getMenu() {
        return menu;
    }
}
