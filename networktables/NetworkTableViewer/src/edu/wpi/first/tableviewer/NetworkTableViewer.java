/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.tableviewer;

import edu.wpi.first.tableviewer.popup.PreferencesPopup;
import edu.wpi.first.tableviewer.treeview.TableTreeView;

import javafx.application.Application;
import javafx.stage.Stage;

/**
 *
 * @author Sam
 */
public class NetworkTableViewer extends Application {

    @Override
    public void start(Stage stage) {
        PreferencesPopup preferencesPopup = new PreferencesPopup();
        preferencesPopup.showPopup();

        TableTreeView view = new TableTreeView(preferencesPopup.getNode());
        view.show();
    }
}
