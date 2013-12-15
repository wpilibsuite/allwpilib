/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.table_viewer.dialog;

import edu.wpi.first.wpilibj.tables.ITable;
import javax.swing.JDialog;

/**
 *
 * @author Sam
 */
public abstract class IAddValueDialog extends JDialog {
    
    protected ITable table;
    
    public IAddValueDialog(ITable table) {
        super();
        this.table = table;
    }
    
}
