/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.tableviewer;

import edu.wpi.first.tableviewer.TableEntryData.EntryType;
import edu.wpi.first.tableviewer.dialog.AbstractAddDialog;
import edu.wpi.first.wpilibj.networktables.NetworkTable;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.tables.ITableListener;
import java.awt.BorderLayout;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.ArrayList;
import java.util.prefs.Preferences;
import javax.swing.JFrame;
import javax.swing.JScrollPane;
import javax.swing.SwingUtilities;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreePath;
import org.netbeans.swing.outline.DefaultOutlineModel;
import org.netbeans.swing.outline.Outline;
import org.netbeans.swing.outline.OutlineModel;

public class OutlineFrame extends JFrame implements ITableListener {

    private final Outline outline;
    private final BranchNode rootBranch;
    private final boolean showMetadata;
    private final Preferences prefs = Preferences.userNodeForPackage(getClass());

    public OutlineFrame(String title, boolean showMetadata) {
        this.showMetadata = showMetadata;

        setTitle(title);
        setDefaultCloseOperation(EXIT_ON_CLOSE);

        rootBranch = new BranchNode("", "Root");
        DefaultTreeModel outlineTreeModel = new DefaultTreeModel(rootBranch);

        OutlineModel outlineModel = DefaultOutlineModel.createOutlineModel(
                outlineTreeModel, new NetworkTableRowModel(), true, "Key");

        outline = new Outline();
        outline.setModel(outlineModel);
        AbstractAddDialog.setOutline(outline);
        AbstractTreeNode.setOutline(outline);
        AbstractTreeNode.setTreeModel(outlineTreeModel);

        outline.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                TreePath path = outline.getClosestPathForLocation(e.getX(), e.getY());
                AbstractTreeNode node = (AbstractTreeNode) path.getLastPathComponent();
                if (e.getClickCount() == 2) { // toggle boolean value
                    // commented out because the deselection makes double clicking
                    // difficult and annoying to do accurately
                    
//                    if (node instanceof LeafNode) {
//                        LeafNode l = (LeafNode) node;
//                        if (l.getData().getType() == EntryType.BOOLEAN) {
//                            l.getData().setValue(!(boolean) l.getData().getValue());
//                        }
//                    }
                } else if (SwingUtilities.isRightMouseButton(e)) { // create a menu
                    if (node instanceof BranchNode) {
                        ((BranchNode) node).getMenu(path).show(outline, e.getX(), e.getY());
                    }
                }
            }
        });

        addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosing(WindowEvent e) {
                prefs.putInt("WindowX", getX());
                prefs.putInt("WindowY", getY());
                prefs.putInt("Width", getWidth());
                prefs.putInt("Height", getHeight());
            }

            @Override
            public void windowOpened(WindowEvent e) {
                // default to top-left corner
                setLocation(prefs.getInt("WindowX", 0), 
                            prefs.getInt("WindowY", 0));
                // default to 600x400
                setSize(prefs.getInt("Width", 600),
                        prefs.getInt("Height", 400));
            }
        });

        JScrollPane scrollPane = new JScrollPane();
        scrollPane.setViewportView(outline);
        add(scrollPane, BorderLayout.CENTER);

        NetworkTable.getTable("").addTableListener(this, true);

    }

    @Override
    public void valueChanged(ITable source, final String key, final Object value, boolean isNew) {
        SwingUtilities.invokeLater(new Runnable() {
            @Override
            public void run() {
                getBranchesToEntry(key, value);
            }
        });
    }

    private void getBranchesToEntry(final String fullKey, Object value) {
        // a list of all the subtable names leading up to the entry in the given key
        // this splits by / and removes empty // elements
        CustomList<String> subTableNames = splitDiscardingEmpty(fullKey, "/");
        String key = "";
        AbstractTreeNode currentNode = rootBranch, parentNode;
        for (String name : subTableNames) {
            parentNode = currentNode;
            currentNode = currentNode.get(name);
            key += "/" + name;
            if (subTableNames.getLast() == name) { // leaf
                if (currentNode == null) {
                    currentNode = new LeafNode(key, new TableEntryData(name, value));
                    if (currentNode.data.isMetadata() && !showMetadata) {
                        // don't show metadata directly
                        // instead, show the value in the branch's "Type" field
                        parentNode.data.setType(EntryType.getEntryType(currentNode.data.getValue().toString()));
                    } else {
                        parentNode.add(currentNode);
                        TreePath path = new TreePath(currentNode.getPath());
                        outline.expandPath(path);
                    }
                } else {
                    ((LeafNode) currentNode).updateValue(value);
                }

            } else if (currentNode == null) {
                currentNode = new BranchNode(key, name);
                parentNode.add(currentNode);
            }
        }
    }

    private CustomList<String> splitDiscardingEmpty(String str, String separator) {
        CustomList<String> results = new CustomList<>();
        for (String string : str.split(separator)) {
            if (string.length() > 0) {
                results.add(string);
            }
        }
        return results;
    }

    private class CustomList<E> extends ArrayList<E> {

        public E getLast() {
            return get(size() - 1);
        }
    }
}
