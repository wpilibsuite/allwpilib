/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.tableviewer.tableview;

import java.net.URL;
import java.util.ResourceBundle;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.stage.Stage;

/**
 * FXML Controller class
 *
 * @author Sam
 */
public class TableViewFXMLController implements Initializable {
    
    @FXML
    public TableColumn keyColumn, valueColumn, typeColumn;
    @FXML
    public TableView tableView;
    
    private Stage stage;

    @Override
    public void initialize(URL url, ResourceBundle rb) {
        tableView.prefWidthProperty().bind(stage.widthProperty());
        keyColumn.prefWidthProperty().bind(stage.widthProperty().divide(3));
        valueColumn.prefWidthProperty().bind(stage.widthProperty().divide(3));
        typeColumn.prefWidthProperty().bind(stage.widthProperty().divide(3));
        
        keyColumn.setCellValueFactory(new PropertyValueFactory<EntryData, String>("key"));
        valueColumn.setCellValueFactory(new PropertyValueFactory<EntryData, String>("value"));
        typeColumn.setCellValueFactory(new PropertyValueFactory<EntryData, String>("type"));
    }
    
    public void setStage(Stage stage) {
        this.stage = stage;
    }
    
}
