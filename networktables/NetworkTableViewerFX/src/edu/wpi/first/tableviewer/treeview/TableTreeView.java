/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.tableviewer.treeview;

import edu.wpi.first.wpilibj.networktables2.NetworkTableNode;
import edu.wpi.first.wpilibj.networktables2.server.NetworkTableServer;

import javafx.event.EventHandler;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.control.TreeCell;
import javafx.scene.control.TreeItem;
import javafx.scene.control.TreeView;
import javafx.stage.Stage;
import javafx.stage.WindowEvent;
import javafx.util.Callback;

/**
 *
 * @author Sam
 */
public class TableTreeView extends Stage {

    private TreeView<ITableItem> tree;
    private static NetworkTableNode rootTableNode;

    public static void setRootNode(NetworkTableNode node) {
        rootTableNode = node;
    }

    public TableTreeView(NetworkTableNode node) {
        Scene scene = initTree();
        setTitle("Network Table Viewer " + ((node instanceof NetworkTableServer) ? "Server" : "Client"));
        setRootNode(node);
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

    private Scene initTree() {
        Scene scene = new Scene(new Group());

        scene.getStylesheets().add("edu/wpi/first/tableviewer/stylesheets/ViewerStyleSheet.css");
        TableBranch rootBranch = new TableBranch(rootTableNode); // create a branch to show the entire contents of the node

        TreeItem root = new TreeItem<>();
        root.getChildren().add(rootBranch);

        tree = new TreeView(root);
        tree.setShowRoot(false);
        tree.prefWidthProperty().bind(scene.widthProperty());
        tree.prefHeightProperty().bind(scene.heightProperty());

        tree.setCellFactory(new Callback<TreeView<ITableItem>, TreeCell<ITableItem>>() {
            @Override
            public TreeCell call(TreeView<ITableItem> p) {
                return TableCell.getCell();
            }
        });

        ((Group) scene.getRoot()).getChildren().add(tree);
        return scene;
    }
}
