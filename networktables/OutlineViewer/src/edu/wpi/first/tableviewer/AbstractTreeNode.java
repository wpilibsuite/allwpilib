/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.tableviewer;

import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.networktables.NetworkTableProvider;
import edu.wpi.first.wpilibj.networktables2.NetworkTableNode;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import org.netbeans.swing.outline.Outline;

/**
 *
 * @author Sam
 */
public abstract class AbstractTreeNode extends DefaultMutableTreeNode {

    protected ITable table;
    protected TableEntryData data;
    protected static DefaultTreeModel treeModel;
    protected static Outline outline;

    public static void setTreeModel(DefaultTreeModel model) {
        treeModel = model;
    }

    public static void setOutline(Outline outline) {
        AbstractTreeNode.outline = outline;
    }

    public AbstractTreeNode(NetworkTableNode node, String key, TableEntryData data) {
        super(data);
        this.data = data;
        if (treeModel != null) {
            treeModel.reload(this);
        }
        table = new NetworkTableProvider(node).getTable(key);
    }

    /**
     * Gets the value to display for the specified column index.
     *
     * @see NetworkTableRowModel#getValueFor(Object node, int column)
     * @param index The index of the column to get the value for.
     * @return The value to display in the given column, or null if the given
     * index is out of bounds.
     */
    public final Object getValueFor(int index) {
        switch (index) {
            case 0:
                return data.getValue();
            case 1:
                return data.getType();
            default:
                return null;
        }
    }

    public TableEntryData getData() {
        return data;
    }

    /**
     * Gets a child node by a specified name. If no child by that name exists,
     * this method returns null.
     *
     * @param name The name of the child to look for.
     * @return The child node of the given name, or {@code null} if none exists.
     */
    public AbstractTreeNode get(String name) {
        for (int i = 0; i < getChildCount(); i++) {
            AbstractTreeNode node = (AbstractTreeNode) getChildAt(i);
            if (node.getData().getKey().equals(name)) {
                return node;
            }
        }
        return null;
    }

    /**
     * Used to make leaves unable to be displayed as NetworkTables and
     * vice-versa.
     *
     * @return Whether this node is a {@link LeafNode} or {@link BranchNode}.
     */
    @Override
    public abstract boolean isLeaf();

    /**
     * Used to show how this node is displayed in the tree. Making this return
     * "1234" will make everything in the "Key" column in the {@link Outline}
     * show "1234" next to their icons.
     *
     * @return
     */
    @Override
    public abstract String toString();
}
