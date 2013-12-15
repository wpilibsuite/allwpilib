/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.tableviewer.treeview;

import edu.wpi.first.tableviewer.EntryData;
import javafx.scene.control.TreeItem;

/**
 *
 * @author Sam
 */
public abstract class ITableItem extends TreeItem<EntryData> {
    
    protected EntryData entryData;

    public ITableItem() {
        super();
    }
    
    public final EntryData getData() {
        return entryData;
    }
    
    @Override
    public abstract boolean isLeaf();
}
