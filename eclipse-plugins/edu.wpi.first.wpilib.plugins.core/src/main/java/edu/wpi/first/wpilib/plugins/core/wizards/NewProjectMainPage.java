package edu.wpi.first.wpilib.plugins.core.wizards;

import java.util.Map;

import javax.activation.FileDataSource;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import org.eclipse.jface.dialogs.IDialogPage;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;

/**
 * The "New" wizard page allows setting the container for the new file as well
 * as the file name. The page will only accept file name without the extension
 * OR with the extension that matches the expected one (mpe).
 */

public class NewProjectMainPage extends WizardPage {
	private Text projectNameText;
	private Text packageText;
	Map<String, ProjectType> types;
	private Text worldText;
	private Button worldButton;

	Button simpleRobot, iterativeRobot, commandRobot;
	private boolean showPackage;
	private boolean showProjectTypes;
	private TeamNumberPage teamNumberPage;

	/**
	 * Constructor for SampleNewWizardPage.
	 * @param teamNumberPage 
	 * 
	 * @param pageName
	 */
	public NewProjectMainPage(ISelection selection, TeamNumberPage teamNumberPage) {
		super("wizardPage");
		this.teamNumberPage = teamNumberPage;
		showPackage = true;
		showProjectTypes = false;
	}

	/**
	 * @see IDialogPage#createControl(Composite)
	 */
	public void createControl(Composite parent) {
		Composite container = new Composite(parent, SWT.NULL);
		GridLayout layout = new GridLayout();
		container.setLayout(layout);
		layout.numColumns = 2;
		layout.verticalSpacing = 9;
		Label label = new Label(container, SWT.NULL);
		label.setText("Project &Name:");

		projectNameText = new Text(container, SWT.BORDER | SWT.SINGLE);
		GridData gd = new GridData(GridData.FILL_HORIZONTAL);
		projectNameText.setLayoutData(gd);
		projectNameText.addModifyListener(new ModifyListener() {
			public void modifyText(ModifyEvent e) {
				dialogChanged();
			}
		});

		if (showPackage) {
			label = new Label(container, SWT.NULL);
			label.setText("&Package:");
		
			packageText = new Text(container, SWT.BORDER | SWT.SINGLE);
			gd = new GridData(GridData.FILL_HORIZONTAL);
			packageText.setLayoutData(gd);
			packageText.addModifyListener(new ModifyListener() {
				public void modifyText(ModifyEvent e) {
					dialogChanged();
				}
			});
		}
		
		if (showProjectTypes) {
			Group projectTypeGroup = new Group(container, SWT.BORDER);
			projectTypeGroup.setText("Project Type");
			gd = new GridData(GridData.FILL_HORIZONTAL);
			gd.horizontalSpan = 2;
			projectTypeGroup.setLayoutData(gd);
			GridLayout groupLayout = new GridLayout();
			groupLayout.numColumns = 1;
			projectTypeGroup.setLayout(groupLayout);
			
			simpleRobot = new Button(projectTypeGroup, SWT.RADIO | SWT.WRAP);
			//simpleRobot.setData( RWT.MARKUP_ENABLED, Boolean.TRUE );
			simpleRobot.setText("Simple Robot: A flexible robot project intended for robots that implement basic functionality. Can also be used as a starting point for teams with a custom framework.");
			gd = new GridData(GridData.FILL_HORIZONTAL);
			gd.widthHint = 300;
			simpleRobot.setLayoutData(gd);
			simpleRobot.setSelection(true);
			
			iterativeRobot = new Button(projectTypeGroup, SWT.RADIO | SWT.WRAP);
			iterativeRobot.setText("Iterative Robot: A robot project that allows robots to be implemented in an iterative manner.");
			gd = new GridData(GridData.FILL_HORIZONTAL);
			gd.widthHint = 300;
			iterativeRobot.setLayoutData(gd);
			
			commandRobot = new Button(projectTypeGroup, SWT.RADIO | SWT.WRAP);
			commandRobot.setText("Command-Based Robot: A robot project that allows robots to be implemented using the command based model to allow complex functionality to be developed from simpler functionality.");
			gd = new GridData(GridData.FILL_HORIZONTAL);
			gd.widthHint = 300;
			commandRobot.setLayoutData(gd);
		}

		label = new Label(container, SWT.NULL);
		label.setText("Simulation &World:");
		

		Composite comp = new Composite(container, SWT.NULL);
		gd = new GridData(GridData.FILL_HORIZONTAL);
		comp.setLayoutData(gd);
		GridLayout groupLayout = new GridLayout();
		groupLayout.numColumns = 2;
		comp.setLayout(groupLayout);
		worldText = new Text(comp, SWT.BORDER | SWT.SINGLE);
		worldText.setLayoutData(gd);
 		worldText.setText("/usr/share/frcsim/worlds/GearsBotDemo.world");
		worldText.addModifyListener(new ModifyListener() {
			public void modifyText(ModifyEvent e) {
				dialogChanged();
			}
		});
		worldButton = new Button(comp, SWT.NULL);
		worldButton.setText("Browse");
		worldButton.addSelectionListener(new SelectionAdapter() {
			@Override public void widgetSelected(SelectionEvent e) {
				browse();
			}
		});
		
		initialize();
		dialogChanged();
		setControl(container);
	}

	/**
	 * Tests if the current workbench selection is a suitable container to use.
	 */
	private void initialize() {
		String teamNumber = TeamNumberPage.getTeamNumberFromPage(teamNumberPage);
		if (showPackage) {
			packageText.setText("org.usfirst.frc.team"+teamNumber+".robot");
			if (teamNumberPage != null) {
				teamNumberPage.registerChangeListener(new ChangeListener() {
					@Override public void stateChanged(ChangeEvent e) {
						String teamNumber = TeamNumberPage.getTeamNumberFromPage(teamNumberPage);
						packageText.setText("org.usfirst.frc.team"+teamNumber+".robot");
						
					}
				});
			}
		}
	}

	/**
	 * Ensures that both text fields are set.
	 */
	private void dialogChanged() {
		String projectName = getProjectName();
		
		String packageString = "";
		if (showPackage) packageString = getPackage();

		if (projectName.length() == 0) {
			updateStatus("Project name must be specified");
			return;
		}
		if (showPackage && packageString.length() == 0) {
			updateStatus("Package must be specified");
			return;
		}
		if (showPackage && !packageString.matches("^([a-zA-Z_]{1}[a-zA-Z0-9_]*(\\.[a-zA-Z_]{1}[a-zA-Z0-9_]*)*)$")) {
			updateStatus("Must be valid java package");
			return;
		}
		
		updateStatus(null);
	}

	private void browse() {
		FileDialog dialog = new FileDialog(getShell(), SWT.OPEN);
		dialog.setText("Pick a World to Simulate");
		dialog.setFileName(worldText.getText());
		dialog.setFilterNames(new String[] { "World Files", "All Files (*.*)" });
	    dialog.setFilterExtensions(new String[] { "*.world", "*.*" });
		String result = dialog.open();
		if (result != null) {
			worldText.setText(result);
		}	
	}

	private void updateStatus(String message) {
		setErrorMessage(message);
		setPageComplete(message == null);
	}

	public String getProjectName() {
		return projectNameText.getText();
	}

	public String getPackage() {
		return packageText.getText();
	}
	
	public ProjectType getProjectType() {
		if (!showProjectTypes) return null;
		else if (simpleRobot.getSelection()) return types.get(ProjectType.SIMPLE);
		else if (iterativeRobot.getSelection()) return types.get(ProjectType.ITERATIVE);
		else return types.get(ProjectType.COMMAND_BASED);
	}

	public String getWorld() {
		return worldText.getText().replace(System.getenv("HOME"), "${user.home}");
	}

	public void setShowPackage(boolean bool) {
		showPackage = bool;
	}

	public void setShowProjectTypes(boolean bool) {
		showProjectTypes = bool;
	}
	
	public void setProjectTypes(Map<String, ProjectType> types) {
		showProjectTypes = true;
		this.types = types;
	}
}
