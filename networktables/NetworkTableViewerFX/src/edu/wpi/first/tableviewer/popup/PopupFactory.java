/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.tableviewer.popup;

import edu.wpi.first.tableviewer.popup.IPopup.Type;
import edu.wpi.first.tableviewer.popup.AddValuePopup.InputType;
import edu.wpi.first.tableviewer.treeview.TableLeaf;
import edu.wpi.first.wpilibj.tables.ITable;

/**
 * A factory for creating and showing {@link Popup Popups} of a defined type.
 * @author Sam
 */
public final class PopupFactory {

    public static void showMessageDialog(String title, String message) {
        IPopup popup = new Popup(title, message, Type.MESSAGE);
        popup.showPopup();
    }

    public static void showMessageDialog(String message) {
        showMessageDialog("", message);
    }
    
    public static void showWarningDialog(String title, String message) {
        IPopup popup = new Popup(title, message, Type.WARNING);
        popup.showPopup();
    }
    
    public static void showWarningDialog(String message) {
        showWarningDialog("", message);
    }

    public static void showErrorDialog(String title, String message) {
        IPopup popup = new Popup(title, message, Type.ERROR);
        popup.showPopup();
    }

    public static void showErrorDialog(String message) {
        showErrorDialog("", message);
    }
    
    public static void showAddBooleanDialog(ITable table) {
        IPopup popup = new AddValuePopup(table, InputType.BOOLEAN);
        popup.showPopup();
    }
    
    public static void showAddNumberDialog(ITable table) {
        IPopup popup = new AddValuePopup(table, InputType.NUMBER);
        popup.showPopup();
    }
    
    public static void showAddStringDialog(ITable table) {
        IPopup popup = new AddValuePopup(table, InputType.STRING);
        popup.showPopup();
    }
    
    public static void showEditorDialog(TableLeaf leaf) {
        IPopup popup = new EditorPopup(leaf);
        popup.showPopup();
    }
    
}
