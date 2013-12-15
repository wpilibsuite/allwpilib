/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.tableviewer.popup;

import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.scene.text.TextAlignment;
import javafx.stage.Modality;
import javafx.stage.Stage;

/**
 * A generic popup. Call
 * {@link PopupFactory Popupfactory}.{@code showXXXXDialog()} to show a
 * predefined popup of a specific type.
 *
 * @see IPopup
 * @see InputPopup
 * @see PopupFactory
 * @author Sam
 */
class Popup extends Stage implements IPopup {
    
    protected final String message;
    protected final Type type;
    
    Popup() {
        this(null, null, Type.INPUT);
    }

    Popup(String title, String message, Type type) {
        super();
        this.message = message;
        this.type = type;

        setTitle(title);
        setWidth(200);
        setHeight(150);
        setResizable(false);

        initModality(Modality.WINDOW_MODAL);
    }

    private void initComponents(Type messageType) {
        final Scene scene = new Scene(new Group());
        scene.setFill(Color.WHITESMOKE);
        final Label messageText = new Label(message);
        final Button okayButton = new Button("Okay");
        final VBox box = new VBox();

        messageText.setAlignment(Pos.CENTER);
        messageText.setTextAlignment(TextAlignment.CENTER);
        messageText.setWrapText(true);
        okayButton.setAlignment(Pos.CENTER);

        okayButton.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent t) {
                close();
            }
        });

        String messageClass = "";
        switch (messageType) {
            case MESSAGE:
                messageClass = "";
                box.setPadding(new Insets(10, 0, 0, 0));
                break;
            case WARNING:
                messageClass = "warning-";
                box.setPadding(new Insets(10, 0, 0, 0));
                break;
            case ERROR:
                messageClass = "error-";
                box.setPadding(new Insets(30, 0, 0, 0));
                break;
            default:
                break;
        }

        scene.getStylesheets().add(
                "edu/wpi/first/tableviewer/stylesheets/PopupCSS.css");
        messageText.getStyleClass().add(messageClass + "message");

        box.setPrefWidth(200);
        box.setSpacing(15);
        box.setAlignment(Pos.CENTER);

        box.getChildren().addAll(messageText, okayButton);

        ((Group) scene.getRoot()).getChildren().addAll(box);

        setScene(scene);
    }

    @Override
    public void initComponents() {
        initComponents(type);
    }

    @Override
    public void showPopup() {
        initComponents();
        showAndWait();
    }
}
