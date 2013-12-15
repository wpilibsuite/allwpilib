/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.tableviewer.treeview;

import javafx.scene.control.TreeItem;

/**
 *
 * @author Sam
 */
public abstract class ITableItem extends TreeItem<TreeItem> {

    public ITableItem() {
        super();
    }

    @Override
    public abstract String toString();

    @Override
    public abstract boolean isLeaf();
}
