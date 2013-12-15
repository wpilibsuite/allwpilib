/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.tableviewer.tableview;

import edu.wpi.first.wpilibj.networktables.NetworkTableProvider;
import edu.wpi.first.wpilibj.networktables2.NetworkTableNode;
import edu.wpi.first.wpilibj.networktables2.server.NetworkTableServer;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.tables.ITableListener;
import javafx.collections.FXCollections;
import javafx.collections.ObservableMap;
import javafx.event.EventHandler;
import javafx.fxml.FXMLLoader;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.control.TableView;
import javafx.scene.layout.BorderPane;
import javafx.stage.Stage;
import javafx.stage.WindowEvent;

/**
 *
 * @author Sam
 */
public class TableTableView extends Stage implements ITableListener {

    private static ITable rootTable;
    private TableView tableView;
    private ObservableMap<String, EntryData> data = FXCollections.observableHashMap();

    public static void setRootTableNode(NetworkTableNode node) {
        rootTable = new NetworkTableProvider(node).getRootTable();
    }

    public TableTableView(NetworkTableNode node) {
        super();
        setRootTableNode(node);
        rootTable.addTableListener(this, true);
        rootTable.addSubTableListener(this);
        FXMLLoader loader = new FXMLLoader(getClass().getResource("TableViewFXML.fxml"));
        setTitle("Network Table Viewer " + ((node instanceof NetworkTableServer) ? "Server" : "Client"));
        Scene scene = new Scene(new Group());
        try {
            ((Group) scene.getRoot()).getChildren().add((BorderPane) loader.load());
            setScene(scene);
            setOnCloseRequest(new EventHandler<WindowEvent>() {
                @Override
                public void handle(WindowEvent t) {
                    System.exit(0);
                }
            });
        } catch (Exception e) {
            e.printStackTrace();
        }
        setHeight(500);
        setWidth(380);
        setMinHeight(500);
        setMinWidth(380);
        setScene(scene);
        setOnCloseRequest(new EventHandler<WindowEvent>() {
            @Override
            public void handle(WindowEvent t) {
                // network tables keep the application running, so exit
                System.exit(0);
            }
        });
    }

    @Override
    public void valueChanged(ITable source, String key, Object value, boolean isNew) {
        data.put(key, new EntryData(source, key, value));
    }
    
}
