/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.tableviewer.popup;

/**
 * An interface for defining popups.
 * @author Sam
 * @see Popup
 * @see InputPopup
 */
public interface IPopup {
    
    /**
     * An enumeration representing the type of popup this is.
     */
    public enum Type {
        MESSAGE,
        WARNING,
        ERROR,
        INPUT
    }
    
    /**
     * A method used to show this popup.
     */    
    public void showPopup();
    
    /**
     * Initialize all components in this popup.
     */
    public void initComponents();
    
}
