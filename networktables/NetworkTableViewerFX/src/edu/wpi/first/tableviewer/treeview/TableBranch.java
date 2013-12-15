/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.tableviewer.treeview;

import edu.wpi.first.tableviewer.EntryData;
import edu.wpi.first.wpilibj.networktables.NetworkTableProvider;
import edu.wpi.first.wpilibj.networktables2.NetworkTableNode;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.tables.ITableListener;
import java.util.ArrayList;
import java.util.Collections;

import java.util.HashMap;
import java.util.List;
import javafx.beans.property.SimpleBooleanProperty;

import javafx.scene.control.TreeItem;

/**
 * A {@link TreeItem} responsible for displaying all the data within a certain
 * {@link ITable}. It can contain {@link TableLeaf TableLeaves} and other
 * {@code TableBranches} if there are subtables in the given {@code ITable}.
 *
 * @see TableLeaf
 * @author Sam
 */
public class TableBranch extends ITableItem implements ITableListener {

    /**
     * Keeps track of values in the table.
     */
    private HashMap<String, ITableItem> entries = new HashMap<>();
    private final ITable table;

    /**
     * Constructs a TableBranch that displays data from the given ITable.
     *
     * @param table The ITable to show the data of.
     */
    public TableBranch(final ITable table) {
        super();
        this.table = table;
        table.addTableListener(this, true);
        table.addSubTableListener(this);
        setExpanded(true);
//        expandedProperty().bind(new SimpleBooleanProperty(true)); //REMOVEME
        entryData = new EntryData(this);
        setValue(entryData);
    }

    /**
     * This should only be called once, when making the root TableBranch.
     *
     * @param node The root node to listen to. Sibling or parent nodes, if they
     * exist, will be ignored.
     * @see TableBranch(ITable)
     */
    public TableBranch(NetworkTableNode node) {
        this(new NetworkTableProvider(node).getRootTable());
    }

    @Override
    public void valueChanged(ITable source, String key, Object value, boolean isNew) {
        if (isNew && !entries.containsKey(key)) {
            if (value.toString().startsWith("NetworkTable: /")) {
                String tableName = value.toString().substring(source.toString().length() + 1);
                TableBranch branch = new TableBranch(source.getSubTable(tableName));
                this.getChildren().add(branch);
                entries.put(key, branch);
            } else {
                TableLeaf leaf = new TableLeaf(source, key, value);
                entries.put(key, leaf);
                this.getChildren().add(leaf);
                this.getChildren().removeAll(getLeaves());
                this.getChildren().addAll(sortLeaves(getLeaves()));
            }
        } else if (!isNew && entries.get(key) instanceof TableLeaf) {
            ((TableLeaf) entries.get(key)).setTableValue(value);
        }
    }

    /**
     * Sorts the leaves in this branch alphabetically. Doesn't apply to
     * sub-branches.
     *
     * @return The sorted list of leaves.
     */
    private List<TableLeaf> sortLeaves(List<TableLeaf> leaves) {
        List<TableLeaf> list = leaves.subList(0, leaves.size());
        Collections.sort(list);
        return list;
    }

    private List<TableLeaf> getLeaves() {
        List<TableLeaf> leaves = new ArrayList<>();
        for (ITableItem item : entries.values()) {
            if (item instanceof TableLeaf) {
                leaves.add((TableLeaf) item);
            }
        }
        return leaves;
    }

    /**
     * Gets the {@link ITable} associated with this branch.
     */
    public ITable getTable() {
        return table;
    }

    @Override
    public boolean isLeaf() {
        return false;
    }

    @Override
    public String toString() {
        return table.toString().substring(table.toString().lastIndexOf("/") + 1);
    }
}
