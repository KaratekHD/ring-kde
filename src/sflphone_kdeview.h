/***************************************************************************
 *   Copyright (C) 2009 by Savoir-Faire Linux                              *
 *   Author : Jérémy Quentin                                               *
 *   jeremy.quentin@savoirfairelinux.com                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef sflphone_kdeVIEW_H
#define sflphone_kdeVIEW_H

#include <QtGui/QWidget>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QList>
#include <QtGui/QListWidgetItem>
#include <QtGui/QKeyEvent>
#include <QErrorMessage>
#include <KXmlGuiWindow>

#include "ui_sflphone_kdeview_base.h"
#include "conf/ConfigurationDialog.h"
#include "CallList.h"
#include "AccountWizard.h"
#include "Contact.h"
#include "sflphone_kdeview.h"
#include "AccountList.h"

#include "ui_sflphone_kdeview_base.h"

class ConfigurationDialogKDE;


/**
 * This is the main view class for sflphone-client-kde.  Most of the non-menu,
 * non-toolbar, and non-statusbar (e.g., non frame) GUI code should go
 * here.
 *
 * @short Main view
 * @author Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>
 * @version 0.1
 */
class sflphone_kdeView : public QWidget, public Ui::SFLPhone_view
{
	Q_OBJECT
    
private:

	static ConfigurationDialogKDE * configDialog;
	static AccountList * accountList;
	AccountWizard * wizard;
	//List of calls in the window, and past ones.
	//Handles both current calls (dialing, ringing...) and history.
	CallList * callList;
	QErrorMessage * errorWindow;
	//Account used prioritary if defined and registered. If not, the first registered account in accountList is used.
	static QString priorAccountId;

protected:
	
	/**
	 * override context menu handling
	 * @param event
	 */
	void contextMenuEvent(QContextMenuEvent *event);

public:
	//Constructors & Destructors
	sflphone_kdeView(QWidget *parent);
	virtual ~sflphone_kdeView();
	/**
	 * Called at construction. Updates all the display
	 * according to the settings.
	 */
	void loadWindow();
	
	//Getters
	/**
	 *   Seeks the account to use.
	 *   If priorAccountId is defined and the corresponding
	 *   account exists and is registered, uses this one, else,
	 *   asks the first registered of accountList.
	 *   If there is no account registered, returns NULL.
	 * @return the account to use if an outgoing call is placed.
	 */
	static Account * firstRegisteredAccount();
	
	static AccountList * getAccountList();
	QErrorMessage * getErrorWindow();
	
	//Daemon getters
	/**
	* Used to sort contacts according to their types with Kabc.
	* @return the integer resulting to the flags of the types 
	* chosen to be displayed in SFLphone configuration.
	*/
	int phoneNumberTypesDisplayed();
	
	//Updates
	QVector<Contact *> findContactsInKAddressBook(QString textSearched, bool & full);
	
private slots:
	/**
	 *   Performs the action action on the call call, then updates window.
	 *   The call object will handle the action with its "actionPerformed" method.
	 *   See the documentation for more details.
	 * @param call the call on which to perform the action
	 * @param action the code of the action to perform
	 */
	void action(Call * call, call_action action);
	
	/**
	 *   Sets the account account to be the prior account.
	 *   That means it's gonna be used when the user places a call
	 *   if it's defined and registered, else the first registered of 
	 *   accountList will be used.
	 * @param account the account to use prior
	 */
	void setAccountFirst(Account * account);
	
	/**
	 *   Handles the behaviour when the user types something with
	 *   the dialpad widget or his keyboard (normally it's a one char
	 *   string but we use a string just in case).
	 *   Adds str to the selected item if in the main window
	 *   and creates a new item if no item is selected.
	 *   Send DTMF if appropriate according to current item's state.
	 *   Adds str to the search bar if in history or address book.
	 * @param str the string sent by the user
	 */
	void typeString(QString str);
	/**
	 *   Handles the behaviour when the user types a backspace
	 *   according to the current state (window, item selected...)
	 */
	void backspace();
	/**
	 *   Handles the behaviour when the user types escape
	 *   according to the current state (window, item selected...)
	 */
	void escape();
	/**
	 *   Handles the behaviour when the user types enter
	 *   according to the current state (window, item selected...)
	 */
	void enter();
	/**
	 *   Displays a message window with editable text of the selected
	 *   phone number in history or address book.
	 *   Once the user accepts, place a call with the chosen phone number.
	 *   Keeps the peer name of the contact or past call.
	 */
	void editBeforeCall();
	
	/**
	 *   Alternates colors of the list widget with the application's palettes's
	 *   base and alternateBase colors.
	 * @param listWidget the list widget to which we alternate colors
	 */
	void alternateColors(QListWidget * listWidget);
	
	/**
	 *   Adds the call's item to the call-list's listwidget.
	 *   This function doesn't add anything to the callList object.
	 * @param call the call to add to the call-list's listwidget
	 */
	void addCallToCallList(Call * call);
	/**
	 * Adds the call's history-item to the call-history's listwidget.
	 * This function doesn't add anything to the callList object.
	 * @param call the past call to add to the call-history's listwidget
	 */
	void addCallToCallHistory(Call * call);
	/**
	 * Adds the contact's item to the addressbook's listwidget.
	 * @param contact the contact to add to the addressbook's listwidget
	 */
	void addContactToContactList(Contact * contact);
	
	/**
	 *   Updates call's item according to its state.
	 *   (Actually it only deletes the item from the main window's
	 *   list widget once the call is OVER.)
	 * @param call the call to which update the item
	 */
	void updateCallItem(Call * call);
	/**
	 *   Updates the toolbar's actions' display according to the selected 
	 *   item's state.
	 */
	void updateWindowCallState();
	/**
	 * Updates the history's search bar's display according to the current
	 * text searched. 
	 * If empty, hide the search bar.
	 */
	void updateSearchHistory();
	void updateCallHistory();
	void updateAddressBook();
	void updateRecordButton();
	void updateVolumeButton();
	void updateRecordBar();
	void updateVolumeBar();
	void updateVolumeControls();
	void updateDialpad();
	
public slots:
	void updateStatusMessage();
	
	
	virtual void keyPressEvent(QKeyEvent *event)
	{
		int key = event->key();
		if(key == Qt::Key_Escape)
			escape();
		else if(key == Qt::Key_Return || key == Qt::Key_Enter)
			enter();
		else if(key == Qt::Key_Backspace)
			backspace();
		else
		{
			QString text = event->text();
			if(! event->text().isEmpty())
			{
				typeString(text);
			}
		}
	}

	void on_action_displayVolumeControls_triggered();
	void on_action_displayDialpad_triggered();
// 	void on_action_configureAccounts_triggered();
// 	void on_action_configureAudio_triggered();
	void on_action_configureSflPhone_triggered();
	void on_action_accountCreationWizard_triggered();
	void on_action_accept_triggered();
	void on_action_refuse_triggered();
	void on_action_hold_triggered();
	void on_action_transfer_triggered();
	void on_action_record_triggered();
	void on_action_history_triggered(bool checked);
	void on_action_addressBook_triggered(bool checked);
	void on_action_mailBox_triggered();
	
	void on_widget_dialpad_typed(QString text);
	
	void on_lineEdit_searchHistory_textChanged();
	void on_lineEdit_addressBook_textChanged();
	
	void on_slider_recVol_valueChanged(int value);
	void on_slider_sndVol_valueChanged(int value);
	
	void on_toolButton_recVol_clicked(bool checked);
	void on_toolButton_sndVol_clicked(bool checked);
	
	void on_listWidget_callList_currentItemChanged();
	void on_listWidget_callList_itemChanged();
	void on_listWidget_callList_itemDoubleClicked(QListWidgetItem * item);
	void on_listWidget_callHistory_currentItemChanged();
	void on_listWidget_callHistory_itemDoubleClicked(QListWidgetItem * item);
	void on_listWidget_addressBook_currentItemChanged();
	void on_listWidget_addressBook_itemDoubleClicked(QListWidgetItem * item);
	
	void on_stackedWidget_screen_currentChanged(int index);
	
	void on1_callStateChanged(const QString &callID, const QString &state);
	void on1_error(MapStringString details);
	void on1_incomingCall(const QString &accountID, const QString &callID/*, const QString &from*/);
	void on1_incomingMessage(const QString &accountID, const QString &message);
	void on1_voiceMailNotify(const QString &accountID, int count);
	void on1_volumeChanged(const QString &device, double value);
	
signals:
	void statusMessageChanged(const QString & message);

};

#endif // sflphone_kdeVIEW_H
