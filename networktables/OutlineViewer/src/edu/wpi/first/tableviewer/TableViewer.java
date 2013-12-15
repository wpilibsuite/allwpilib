/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.tableviewer;

import edu.wpi.first.tableviewer.dialog.PreferencesDialog;
import javax.swing.UIManager;

/**
 *
 * @author Sam
 */
public class TableViewer {
    
    public static void main(String[] args) {
        try {
            if (System.getProperty("os.name").contains("Windows")) {
                UIManager.setLookAndFeel("javax.swing.plaf.nimbus.NimbusLookAndFeel");
            }
        } catch (Exception e) {
        }
        new PreferencesDialog().setVisible(true);
    }
    
}
