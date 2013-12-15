/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.table_viewer.tree;

import edu.wpi.first.table_viewer.dialog.EditBooleanDialog;
import edu.wpi.first.table_viewer.dialog.EditNumberDialog;
import edu.wpi.first.table_viewer.dialog.EditStringDialog;
import edu.wpi.first.wpilibj.networktables.NetworkTableProvider;
import edu.wpi.first.wpilibj.networktables2.NetworkTableNode;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.tables.ITableListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import javax.swing.JPopupMenu;
import javax.swing.JTree;
import javax.swing.SwingUtilities;
import javax.swing.tree.DefaultTreeModel;

/**
 *
 * @author Sam
 */
public class NetworkTableTree extends JTree implements ITableListener {

    private final BranchNode rootNode;

    public NetworkTableTree(NetworkTableNode node) {
        super();
        ITable table = new NetworkTableProvider(node).getRootTable();

        ITableNode.setTree(this);

        rootNode = new BranchNode(table);
        table.addTableListener(this, true);
        table.addSubTableListener(this);

        ((DefaultTreeModel) this.getModel()).setRoot(rootNode);
        MouseAdapter ml = new MouseAdapter() {
            @Override
            public void mousePressed(MouseEvent e) {
                int selRow = getRowForLocation(e.getX(), e.getY());
                if (SwingUtilities.isRightMouseButton(e)) {
                    int row = getClosestRowForLocation(e.getX(), e.getY());
                    setSelectionRow(row);
                    handleRightClick(e.getX(), e.getY());
                }
                if (selRow != -1) {
                    if (e.getClickCount() == 2) {
                        handleDoubleClick();
                    }
                }
            }
        };

        addMouseListener(ml);

    }

    private void handleDoubleClick() {
        ITableNode selectedNode = ((ITableNode) getSelectionPath().getLastPathComponent());
        if (selectedNode instanceof LeafNode) {
            if (((LeafNode) selectedNode).getType().equalsIgnoreCase("number")) {
                new EditNumberDialog((LeafNode) selectedNode).setVisible(true);
            } else if (((LeafNode) selectedNode).getType().equalsIgnoreCase("string")) {
                new EditStringDialog((LeafNode) selectedNode).setVisible(true);
            } else if (((LeafNode) selectedNode).getType().equalsIgnoreCase("boolean")) {
                new EditBooleanDialog((LeafNode) selectedNode).setVisible(true);
            }
        }
    }

    private void handleRightClick(int x, int y) {
        ITableNode selectedNode = ((ITableNode) getSelectionPath().getLastPathComponent());
        System.out.println("Right click at [" + x + "," + y + "] on node " + selectedNode);
        JPopupMenu m = selectedNode.getMenu();
        m.show(this, x, y);
    }

    @Override
    public void valueChanged(ITable source, String key, Object value, boolean isNew) {
        System.out.println(key + " changed to " + value);
        ((DefaultTreeModel) this.getModel()).reload();
    }
}
