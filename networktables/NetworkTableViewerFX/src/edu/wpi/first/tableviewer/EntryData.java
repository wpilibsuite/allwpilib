/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.tableviewer;

import edu.wpi.first.tableviewer.treeview.ITableItem;
import edu.wpi.first.tableviewer.treeview.TableBranch;
import edu.wpi.first.tableviewer.treeview.TableLeaf;
import edu.wpi.first.tableviewer.treeview.TableLeaf.EntryType;
import java.util.HashMap;
import java.util.Map;

/**
 *
 * @author Sam
 */
public class EntryData {

    public static final EntryData EMPTY = new EntryData(null, null, null);
    
    private String key;
    private Object value;
    private EntryType type;
    
    private boolean representsLeaf = true;
    private ITableItem tableItem;

    public EntryData(String key, Object value, EntryType type) {
        this.key = key;
        this.value = value;
        this.type = type;
    }

    public EntryData(TableLeaf leaf) {
        this(leaf.getTableKey(), leaf.getTableValue(), leaf.getEntryType());
        tableItem = leaf;
    }

    public EntryData(TableBranch branch) {
        key = branch.toString();
        representsLeaf = false;
        tableItem = branch;
    }
    
    public boolean representsLeaf() {
        return representsLeaf;
    }

    public void setTableItem(ITableItem item) {
        tableItem = item;
    }
    
    public ITableItem getTableItem() {
        return tableItem;
    }

    public void setKey(String key) {
        this.key = key;
    }

    public void setValue(Object value) {
        this.value = value;
    }

    public void setType(EntryType type) {
        this.type = type;
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
}
