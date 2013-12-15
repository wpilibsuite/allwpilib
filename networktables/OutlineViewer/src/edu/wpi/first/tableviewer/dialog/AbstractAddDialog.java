/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.tableviewer.dialog;

import edu.wpi.first.wpilibj.tables.ITable;
import java.awt.event.ActionEvent;
import javax.swing.JDialog;
import javax.swing.JOptionPane;
import javax.swing.JTextField;
import javax.swing.tree.TreePath;
import org.netbeans.swing.outline.Outline;

/**
 *
 * @author Sam
 */
public abstract class AbstractAddDialog extends JDialog {

    private static Outline outline;
    protected final ITable table;
    protected final TreePath path;
    protected JTextField keyField = new JTextField();

    public static void setOutline(Outline outline) {
        AbstractAddDialog.outline = outline;
    }

    protected static Outline getOutline() {
        return outline;
    }

    public AbstractAddDialog(TreePath path, ITable table) {
        super();
        this.table = table;
        this.path = path;
    }

    /**
     * Checks if a key is provided and shows an error dialog if not.
     */
    public final void checkKey() {
        if (keyField.getText().isEmpty()) {
            JOptionPane.showMessageDialog(this, "Key cannot be empty", "Empty key", JOptionPane.ERROR_MESSAGE);
            throw new RuntimeException();
        }
    }

    public abstract void okayButtonActionPerformed(ActionEvent event);

    public final void cancelButtonActionPerformed(ActionEvent event) {
        dispose();
    }
}
