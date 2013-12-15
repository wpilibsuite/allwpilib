/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.table_viewer.tree;

import edu.wpi.first.table_viewer.dialog.AddBooleanDialog;
import edu.wpi.first.table_viewer.dialog.AddNumberDialog;
import edu.wpi.first.table_viewer.dialog.AddStringDialog;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.tables.ITableListener;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;
import javax.swing.JTree;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;

/**
 *
 * @author Sam
 */
public abstract class ITableNode extends DefaultMutableTreeNode implements ITableListener {

    protected JPopupMenu menu = new JPopupMenu();
    protected static DefaultTreeModel treeModel;

    public static void setTree(JTree tree) {
        treeModel = (DefaultTreeModel) tree.getModel();
    }

    public ITableNode(final ITable table, Object userObject, boolean allowsChildren) {
        super(userObject, allowsChildren);
        JMenuItem addBoolean = new JMenuItem("Add boolean"),
                  addNumber = new JMenuItem("Add number"),
                  addString = new JMenuItem("Add string");
        
        addBoolean.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                new AddBooleanDialog(table).setVisible(true);
            }
        });
        
        addNumber.addActionListener(new ActionListener() {

            @Override
            public void actionPerformed(ActionEvent e) {
                new AddNumberDialog(table).setVisible(true);
            }
        });
        
        addString.addActionListener(new ActionListener() {

            @Override
            public void actionPerformed(ActionEvent e) {
                new AddStringDialog(table).setVisible(true);
            }
        });
        
        menu.add(addBoolean);
        menu.add(addNumber);
        menu.add(addString);
    }

    @Override
    public abstract boolean isLeaf();

    public abstract JPopupMenu getMenu();
    
}
