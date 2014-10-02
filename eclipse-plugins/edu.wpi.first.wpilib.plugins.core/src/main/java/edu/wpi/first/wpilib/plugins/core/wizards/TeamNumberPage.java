package edu.wpi.first.wpilib.plugins.core.wizards;

import javax.swing.event.ChangeListener;

import org.eclipse.jface.dialogs.IDialogPage;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;

import edu.wpi.first.wpilib.plugins.core.WPILibCore;

public class TeamNumberPage extends WizardPage {
	private Text teamNumberText;
	private ChangeListener listener;

	/**
	 * Constructor for SampleNewWizardPage.
	 * 
	 * @param pageName
	 */
	public TeamNumberPage(ISelection selection) {
		super("wizardPage");
		setTitle("Set Team Number");
		setDescription("This page lets you set your team number so that code can be loaded onto the right robot.");
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
		label.setText("Team &Number:");

		teamNumberText = new Text(container, SWT.BORDER | SWT.SINGLE);
		GridData gd = new GridData(GridData.FILL_HORIZONTAL);
		teamNumberText.setLayoutData(gd);
		teamNumberText.addModifyListener(new ModifyListener() {
			public void modifyText(ModifyEvent e) {
				dialogChanged();
			}
		});
		
		label = new Label(container, SWT.NULL);
		label.setText("Team number is a global setting that can be changed in Window > Preferences > WPILib Preferences > Team Number");
		gd = new GridData(GridData.FILL_HORIZONTAL);
		gd.horizontalSpan = 2;
		label.setLayoutData(gd);
		
		initialize();
		dialogChanged();
		setControl(container);
	}

	/**
	 * Tests if the current workbench selection is a suitable container to use.
	 */
	private void initialize() {
		String teamNumber = Integer.toString(WPILibCore.getDefault().getTeamNumber(null));
		teamNumberText.setText(teamNumber);
	}

	/**
	 * Ensures that both text fields are set.
	 */
	private void dialogChanged() {
		String teamNumber = getTeamNumber();
		if (listener != null) listener.stateChanged(null); 
		if (!teamNumber.matches("^([1-9][0-9]*)$")) {
			updateStatus("Team number must be a valid integer without leading zeroes.");
			return;
		}
		updateStatus(null);
	}

	private void updateStatus(String message) {
		setErrorMessage(message);
		setPageComplete(message == null);
	}

	public String getTeamNumber() {
		return teamNumberText.getText();
	}
	
	public static boolean needsTeamNumberPage() {
		return WPILibCore.getDefault().getTeamNumber(null) == 0;
	}

	public static String getTeamNumberFromPage(TeamNumberPage teamNumberPage) {
		if (teamNumberPage != null) {
			return teamNumberPage.getTeamNumber();
		} else {
			return Integer.toString(WPILibCore.getDefault().getTeamNumber(null));
		}
	}

	public void registerChangeListener(ChangeListener changeListener) {
		listener = changeListener;
	}
}
