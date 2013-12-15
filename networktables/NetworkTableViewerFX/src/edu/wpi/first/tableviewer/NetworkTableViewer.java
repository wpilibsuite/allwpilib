/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.tableviewer;

import edu.wpi.first.tableviewer.popup.PreferencesDialog;
import edu.wpi.first.tableviewer.tableview.TableTableView;
import edu.wpi.first.wpilibj.networktables2.NetworkTableNode;
import java.nio.charset.Charset;

import javafx.application.Application;
import javafx.event.EventHandler;
import javafx.fxml.FXMLLoader;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.layout.BorderPane;
import javafx.stage.Modality;
import javafx.stage.Stage;
import javafx.stage.StageStyle;
import javafx.stage.WindowEvent;

/**
 *
 * @author Sam
 */
public class NetworkTableViewer extends Application {

    @Override
    public void start(Stage primaryStage) {
        FXMLLoader loader = new FXMLLoader(PreferencesDialog.class.getResource("PreferencesDialog.fxml"));
        Scene scene;
        PreferencesDialog controller = null;
        try {
            scene = new Scene(new Group());
            ((Group)scene.getRoot()).getChildren().add((BorderPane)loader.load());
            controller = loader.getController();            
            Stage s = new Stage();
            controller.setStage(s);
            s.setResizable(false);
            s.initStyle(StageStyle.UTILITY);
            s.initModality(Modality.WINDOW_MODAL);
            s.setScene(scene);
            s.setOnCloseRequest(new EventHandler<WindowEvent>() {
                @Override
                public void handle(WindowEvent t) {
                    System.exit(0);
                }
            });
            s.showAndWait();
        } catch (Exception e) {
            e.printStackTrace();
        }

        NetworkTableNode node = controller.getNode();
        TableTableView view = new TableTableView(node);
        view.show();
    }
}
