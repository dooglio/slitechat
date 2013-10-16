/** 
 * \brief Main GUI window frame for SLiteChat.
 *
 * Copyright (c) 2009-2010 by R. Douglas Barbieri
 * 
 * The source code in this file ("Source Code") is provided by R. Douglas Barbieri
 * to you under the terms of the GNU General Public License, version 2.0
 * ("GPL").  Terms of the GPL can be found in doc/GPL-license.txt in this distribution.
 * 
 * By copying, modifying or distributing this software, you acknowledge
 * that you have read and understood your obligations described above,
 * and agree to abide by those obligations.
 * 
 * ALL SOURCE CODE IN THIS DISTRIBUTION IS PROVIDED "AS IS." THE AUTHOR MAKES NO
 * WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
 * COMPLETENESS OR PERFORMANCE.
 */

#include "MainWindow.h"
#include "ExportWindow.h"
#include "LoginWindow.h"
#include "Preferences.h"
#include "SearchWindow.h"
#include "Config.h"
#include "Utility.h"
#include "Common.h"
#include "MessageDialog.h"

// LLC
//
#include "GridList.h"

#include <string>
#include <iostream>

// Boost
//
#include <boost/bind.hpp>

// Qt4
//
#include <QSettings>
#include <QDir>
#include <QChar>
#include <QNetworkRequest>
#include <QNetworkReply>

namespace
{
	
QIcon GetOnlineIcon( const bool online )
{
	QString iconName;
	//
	if( online )
	{
		iconName = QString::fromUtf8(":/mainIcon/online_icon");
	}
	else
	{
		iconName = QString::fromUtf8(":/mainIcon/offline_icon");
	}
	//
	QIcon icon;
	icon.addPixmap( QPixmap(iconName), QIcon::Normal, QIcon::Off );
	return icon;
}

}
// namespace


MainWindow::MainWindow( QWidget* parent )
	: QMainWindow( parent )
	, m_ui( new Ui_MainWindow )
	, m_localChatWindow(0)
	, m_netState(NetStateShutdown)
	, m_timerId(-1)
	, m_initialCheck(true)
	, m_typingInIM(false)
{
	const int sl_grid_index = LLC::GridList::find( "Second Life" );

	// Start the messaging system, which loads our user settings
	//
	LLC::Manager llmgr;
	llmgr.DeclareString( LLC::String(FIRSTNAME), LLC::String(), LLC::String("First name of the avatar") );
	llmgr.DeclareString( LLC::String(LASTNAME) , LLC::String(), LLC::String("Last name of the avatar") );
	llmgr.DeclareString( LLC::String(LASTSLURL), LLC::String("home"), LLC::String("Last slurl visited avatar") );
	llmgr.DeclareUInt  ( LLC::String(LASTGRID), sl_grid_index, LLC::String("Last grid visited") );
	llmgr.DeclareBool  ( LLC::String(SAVEPASSWORD), false, LLC::String("Should the password be saved?") );
	llmgr.DeclareString( LLC::String(MUNGEDPASSWORD), LLC::String(), LLC::String("Password in munged format") );
	llmgr.DeclareBool  ( LLC::String(PERSISTIM), DEFAULT_PIM, LLC::String("Should IM history be kept between sessions") );
	llmgr.DeclareBool  ( LLC::String(PERSISTLC), DEFAULT_PLC, LLC::String("Should local chat history be kept between sessions") );
	llmgr.DeclareBool  ( LLC::String(TIMESTAMPS), DEFAULT_TIMEST, LLC::String("Put a date in front of messages") );
	llmgr.DeclareBool  ( LLC::String(IGNORE_GROUP_IMS), DEFAULT_IGNOREGRP, LLC::String("Ignore ALL group IMs") );
	llmgr.DeclareUInt  ( LLC::String(LOCAL_LANGUAGE), DEFAULT_LL, LLC::String("Number of local language") );
	llmgr.DeclareString( LLC::String(LOCAL_LANGUAGE_CODE), LLC::String(DEFAULT_LLC), LLC::String("Code of local language") );
	llmgr.DeclareBool  ( LLC::String(GOOGLE_XLATE), DEFAULT_GX, LLC::String("Should we use Google to translate messages") );
	llmgr.DeclareBool  ( LLC::String(ECHO_SOURCE), DEFAULT_ES, LLC::String("Should source message be displayed") );
	//
	llmgr.StartMessagingSystem("SLiteChat", "settings.xml");
	
	// Now set up the UI
	m_ui->setupUi( this );
	//
	m_localChatWindow = m_ui->m_localChatWindow;
	m_localChatWindow->SetFocus();
	m_localChatWindow->ConnectTypingSignal	( boost::bind( &MainWindow::OnIMTyping  , this, _1     ) );
	m_localChatWindow->ConnectImSignal		( boost::bind( &MainWindow::CreateImTab , this, _1, _2 ) );

	// Listen to messages from SL
	//
	ConnectMgrSignals();

	// Update the menu
	//
	UpdateAppState();

	// Now load our saved values
	//
	m_loginInfo.m_firstName 		= LS2Q( llmgr.GetString(FIRSTNAME) 		);
	m_loginInfo.m_lastName  		= LS2Q( llmgr.GetString(LASTNAME)  		);
	m_loginInfo.m_slurl  			= LS2Q( llmgr.GetString(LASTSLURL) 		);
	m_loginInfo.m_mungedPassword	= LS2Q( llmgr.GetString(MUNGEDPASSWORD) );
	m_loginInfo.m_grid				= llmgr.GetUInt(LASTGRID);
	m_loginInfo.m_savePassword		= llmgr.GetBool(SAVEPASSWORD);
	//
	// Turn on/off translation engine according to settings
	//
	llmgr.SetTranslateMessages( llmgr.GetBool(GOOGLE_XLATE) );
	//
	// Set selected language code
	//
	QString lang_code = LS2Q( llmgr.GetString(LOCAL_LANGUAGE_CODE) );
	llmgr.SetLanguage( lang_code == "sys"? Q2LS(GetSystemLangCode()): Q2LS(lang_code) );
	
	// Set the toggle action to the View Menu so we can bring back the docked widgets
	// should the user hide them.
	//
	QAction* friendsViewAction = m_ui->m_friendsDock->toggleViewAction();
	QAction* groupsViewAction  = m_ui->m_groupsDock ->toggleViewAction();
	m_ui->m_menuView->addAction( friendsViewAction );
	m_ui->m_menuView->addAction( groupsViewAction  );
	
	// Set last window size
	//
	QSettings settings;
	settings.beginGroup( GUI_SECTION );
	//
	restoreGeometry( settings.value( WINDOWGEOMETRY, saveGeometry() ).toByteArray() );
	restoreState( settings.value( WINDOWSTATE, saveState( 1 ) ).toByteArray() );
	//
	QString fontName = settings.value( GUIFONT, font().toString() ).toString();
	QFont theFont;
	theFont.fromString( fontName );
	setFont( theFont );
	//
	settings.endGroup();
	
	// Hook up http updater signals and check for updates...
	//
	QObject::connect	( &m_networkManager
						, SIGNAL(finished(QNetworkReply*))
						, this
						, SLOT(OnRequestFinished(QNetworkReply*))
						);
	OnActionHelpCheckForUpdates();
}


MainWindow::~MainWindow()
{
	QObject::disconnect	( &m_networkManager
						, SIGNAL(finished(QNetworkReply*))
						, this
						, SLOT(OnRequestFinished(QNetworkReply*))
						);

	if( m_timerId != -1 )
	{
		killTimer( m_timerId );
	}
	
	QAction* friendsViewAction = m_ui->m_friendsDock->toggleViewAction();
	QAction* groupsViewAction  = m_ui->m_groupsDock ->toggleViewAction();
	m_ui->m_menuView->removeAction( friendsViewAction );
	m_ui->m_menuView->removeAction( groupsViewAction  );

	// Persist settings
	//
	QSettings settings;
	settings.beginGroup( GUI_SECTION );
	settings.setValue( WINDOWGEOMETRY, saveGeometry() );
	settings.setValue( WINDOWSTATE, saveState( 1 ) );
	settings.setValue( GUIFONT, font().toString() );
	settings.endGroup();

	// Purge out all potentially hidden dialogs
	//
	MessageDialog::PurgeAllDialogs();
}


void MainWindow::ConnectMgrSignals()
{
	// Connect LLChatLib signals to our object
	//
	LLC::Manager	llmgr;	// Singleton
	//
	llmgr.ConnectFriendAddSignal             ( boost::bind( &MainWindow::OnAddFriendSignal             , this, _1 ) );
	llmgr.ConnectCacheSignal                 ( boost::bind( &MainWindow::OnCacheSignal                 , this, _1, _2, _3 ) );
	llmgr.ConnectGroupCacheSignal            ( boost::bind( &MainWindow::OnGroupCacheSignal            , this, _1, _2	) );
	llmgr.ConnectGroupChatAgentUpdateSignal  ( boost::bind( &MainWindow::OnGroupChatAgentUpdateSignal  , this, _1, _2, _3 ) );
	llmgr.ConnectImSignal                    ( boost::bind( &MainWindow::OnImSignal                    , this, _1, _2, _3, _4, _5, _6 ) );
	llmgr.ConnectGroupChatSignal             ( boost::bind( &MainWindow::OnGroupChatSignal             , this, _1, _2, _3, _4, _5, _6, _7 ) );
	llmgr.ConnectLocalChatSignal             ( boost::bind( &MainWindow::OnLocalChatSignal             , this, _1, _2, _3, _4, _5, _6, _7 ) );
	llmgr.ConnectTypingSignal                ( boost::bind( &MainWindow::OnTypingSignal                , this, _1, _2, _3 ) );
	llmgr.ConnectOnlineSignal                ( boost::bind( &MainWindow::OnOnlineSignal                , this, _1, _2 ) );
	llmgr.ConnectFriendAcceptSignal          ( boost::bind( &MainWindow::OnFriendshipAcceptSignal      , this, _1, _2, _3 ) );
	llmgr.ConnectFriendDeclineSignal         ( boost::bind( &MainWindow::OnFriendshipDeclineSignal     , this, _1, _2, _3 ) );
	llmgr.ConnectTerminateFriendshipSignal   ( boost::bind( &MainWindow::OnTerminateFrienshipSignal    , this, _1 ) );
	llmgr.ConnectFriendOfferSignal           ( boost::bind( &MainWindow::OnFriendOfferSignal           , this, _1, _2, _3, _4, _5, _6 ) );
	llmgr.ConnectMessageBoxSignal            ( boost::bind( &MainWindow::OnMessageBoxSignal            , this, _1 ) );
	llmgr.ConnectForcedQuitSignal            ( boost::bind( &MainWindow::OnForcedQuitSignal            , this, _1 ) );
	llmgr.ConnectLogoutReplySignal           ( boost::bind( &MainWindow::OnLogoutReplySignal           , this ) );
	llmgr.ConnectAgentMovementCompleteSignal ( boost::bind( &MainWindow::OnAgentMovementCompleteSignal , this ) );
	//
	llmgr.ConnectTeleportRequestSignal       ( boost::bind( &MainWindow::OnTeleportRequestSignal       , this, _1, _2, _3 ) );
	llmgr.ConnectTeleportRequestedSignal     ( boost::bind( &MainWindow::OnTeleportRequestedSignal     , this, _1 ) );
	llmgr.ConnectTeleportStartSignal         ( boost::bind( &MainWindow::OnTeleportStartSignal         , this, _1 ) );
	llmgr.ConnectTeleportProgressSignal      ( boost::bind( &MainWindow::OnTeleportProgressSignal      , this, _1, _2 ) );
	llmgr.ConnectTeleportLocalSignal         ( boost::bind( &MainWindow::OnTeleportLocalSignal         , this ) );
	llmgr.ConnectTeleportFailedSignal        ( boost::bind( &MainWindow::OnTeleportFailedSignal        , this, _1 ) );
	llmgr.ConnectTeleportFinishSignal        ( boost::bind( &MainWindow::OnTeleportFinishSignal        , this ) );
}


void MainWindow::showEvent( QShowEvent * event )
{
    QWidget* widget = m_ui->m_tabWidget->currentWidget();
    if( widget )
    {
		ChatWindow* chatWnd = dynamic_cast<ChatWindow*>(widget);
		if( chatWnd )
		{
			chatWnd->SetFocus();
		}
    }
}


void MainWindow::closeEvent( QCloseEvent* event )
{
	killTimer( m_timerId );

	//
	if( m_netState == NetStateConnected )
	{
		QString msg = tr("Are you sure you want to logout and exit the application?");

		// Ask to shutdown first if connected
		//
		if( MessageDialog::Question( this, tr("Closing SLiteChat!"), msg ) )
	    {
			Logout();
			//
	        event->accept();
	    }
	    else
	    {
	        event->ignore();
			//
			m_timerId = startTimer( MESSAGE_TIMEOUT_MSECS );
	    }
	}
}


void MainWindow::NotifyImConnected( const bool online )
{
	ChatWindowMap::iterator			iter = m_imWindowMap.begin();
	const ChatWindowMap::iterator	end  = m_imWindowMap.end();
	//
	for( ; iter != end; ++iter )
	{
		ChatWindowPtr chatWnd = iter->second;
		if( chatWnd )
		{
			chatWnd->SetConnected( online );
		}
	}
}


void MainWindow::UpdateAppState()
{
	switch( m_netState )
	{
		case NetStateShutdown:
			{
				m_ui->m_actionFileLogin   ->setEnabled( true  );
				m_ui->m_actionFileLogout  ->setEnabled( false );
				m_ui->m_actionFileExport  ->setEnabled( false );
				m_ui->m_actionFriendAdd   ->setEnabled( false );
				m_ui->m_actionFriendRemove->setEnabled( false );
				m_ui->m_actionFriendIM    ->setEnabled( false );
				m_ui->m_actionGroupSearch ->setEnabled( false );
				m_ui->m_actionGroupLeave  ->setEnabled( false );
				m_ui->m_actionGroupIM     ->setEnabled( false );
				//
				m_ui->m_addFriend         ->setEnabled( false );
				m_ui->m_removeFriend      ->setEnabled( false );
				m_ui->m_sendMessage       ->setEnabled( false );
				m_ui->m_addGroup          ->setEnabled( false );
				m_ui->m_leaveGroup        ->setEnabled( false );
				m_ui->m_sendGroupMessage  ->setEnabled( false );
				m_ui->m_actionFileTeleportHome->setEnabled( false );
				//
				SetTabIcon( 0, OFFLINE );
				//
				setWindowTitle( tr("SLiteChat [OFFLINE]") );
			}
			break;


		// Display the login window so the user can authenticate
		//
		case NetStateLogin:
			{
				m_ui->m_actionFileLogin   ->setEnabled( false );
				m_ui->m_actionFileLogout  ->setEnabled( false );
				m_ui->m_actionFileExport  ->setEnabled( false );
				m_ui->m_actionFriendAdd   ->setEnabled( false );
				m_ui->m_actionFriendRemove->setEnabled( false );
				m_ui->m_actionFriendIM    ->setEnabled( false );
				m_ui->m_actionGroupSearch ->setEnabled( false );
				m_ui->m_actionGroupLeave  ->setEnabled( false );
				m_ui->m_actionGroupIM     ->setEnabled( false );
				//
				m_ui->m_addFriend         ->setEnabled( false );
				m_ui->m_removeFriend      ->setEnabled( false );
				m_ui->m_sendMessage       ->setEnabled( false );
				m_ui->m_addGroup          ->setEnabled( false );
				m_ui->m_leaveGroup        ->setEnabled( false );
				m_ui->m_sendGroupMessage  ->setEnabled( false );
				m_ui->m_actionFileTeleportHome->setEnabled( false );
				//
				SetTabIcon( 0, OFFLINE );
				//
				setWindowTitle( tr("SLiteChat [OFFLINE]") );
			}
			break;

		// Wait for the server's response
		//
		case NetStateConnected:
			{
				QTreeWidgetItem* treeItem = m_ui->m_friendsList->currentItem();
				QListWidgetItem* listItem = m_ui->m_groupList  ->currentItem();
				//
				const bool friend_selected = (treeItem != 0);
				const bool group_selected  = (listItem != 0);
				//
				m_ui->m_actionFileLogin   ->setEnabled( false );
				m_ui->m_actionFileLogout  ->setEnabled( true  );
				m_ui->m_actionFileExport  ->setEnabled( true  );
				m_ui->m_actionFriendAdd   ->setEnabled( true  );
				m_ui->m_actionFriendRemove->setEnabled( friend_selected );
				m_ui->m_actionFriendIM    ->setEnabled( friend_selected );
				m_ui->m_actionGroupSearch ->setEnabled( false );
				m_ui->m_actionGroupLeave  ->setEnabled( false );
				m_ui->m_actionGroupIM     ->setEnabled( false );
				//
				m_ui->m_addFriend         ->setEnabled( true );
				m_ui->m_removeFriend      ->setEnabled( friend_selected );
				m_ui->m_sendMessage       ->setEnabled( friend_selected );
				m_ui->m_addGroup          ->setEnabled( true );
				m_ui->m_leaveGroup        ->setEnabled( group_selected );
				m_ui->m_sendGroupMessage  ->setEnabled( group_selected );
				m_ui->m_actionFileTeleportHome->setEnabled( true );
				//
				SetTabIcon( 0, LOCALCHAT );
				//
				setWindowTitle( tr("%1 - SLiteChat").arg( m_myName ) );
			}
			break;
	}
}


void MainWindow::Login()
{
	m_localChatWindow->AddLogEntry( tr("Please enter your login information") );
	//
	LoginWindow	loginDlg( this );
	loginDlg.SetLoginInfo( m_loginInfo );
	//
	if( loginDlg.exec() == QDialog::Accepted )
	{
		// Get info from dialog
		//
		m_loginInfo					= loginDlg.GetLoginInfo();
		const QString firstName 	= m_loginInfo.m_firstName;
		const QString lastName  	= m_loginInfo.m_lastName;
		const LLC::String gridURL	= LLC::GridList::url( m_loginInfo.m_grid );

		// Default SL grid
		// https://login.agni.lindenlab.com/cgi-bin/login.cgi

		LLC::Manager llmgr;
		llmgr.Authenticate(	gridURL
						  , Q2LS(firstName)
						  , Q2LS(lastName)
						  , Q2LS(m_loginInfo.m_mungedPassword)
						  , Q2LS(m_loginInfo.m_slurl)
						  );
		//
		llmgr.SetString( FIRSTNAME   , Q2LS(m_loginInfo.m_firstName) );
		llmgr.SetString( LASTNAME    , Q2LS(m_loginInfo.m_lastName)  );
		llmgr.SetString( LASTSLURL   , Q2LS(m_loginInfo.m_slurl) 	 );
		llmgr.SetUInt  ( LASTGRID    , m_loginInfo.m_grid       	 );
		llmgr.SetBool  ( SAVEPASSWORD, m_loginInfo.m_savePassword    );
		//
		if( m_loginInfo.m_savePassword )
		{
			llmgr.SetString( MUNGEDPASSWORD, Q2LS(m_loginInfo.m_mungedPassword) );
		}
		else
		{
			m_loginInfo.m_mungedPassword.clear();
			//
			llmgr.SetString( MUNGEDPASSWORD, LLC::String() );
		}
		//

		m_myName = firstName + tr(" ") + lastName;
		m_localChatWindow->AddLogEntry( tr("Logging in as %1, please wait....").arg(m_myName) );
		m_localChatWindow->SetMyName( m_myName );

		// Change the network state to "Login" and start the appropriate timer
		//
		m_netState = NetStateLogin;
		m_timerId = startTimer( MESSAGE_TIMEOUT_MSECS );
	}
	else
	{
		m_localChatWindow->AddLogEntry( tr("Login aborted...") );
		m_localChatWindow->SetMyName( "" );
		m_netState = NetStateShutdown;
	}

	UpdateAppState();
}


void MainWindow::Logout()
{
	// Request a logout from the grid
	//
	LLC::Manager llmgr;
	llmgr.RequestLogout();

	m_localChatWindow->AddLogEntry( tr("Logout of %1 requested.").arg(m_myName) );
}


void MainWindow::timerEvent( QTimerEvent* event )
{
	killTimer( m_timerId );

	switch( m_netState )
	{
		// Wait for the server's response
		//
		case NetStateLogin:
			{
				ClearChatTabs();
				//
				try
				{
					LLC::Manager llmgr;
					//
					if( llmgr.CheckForResponse() )
					{
						// Got good response, let's finish logging into the grid
						//
						m_localChatWindow->AddLogEntry( tr("%1 logged in successfully!").arg(m_myName) );

						m_localChatWindow->AddLogEntry( tr("Requesting buddy list...") );
						llmgr.RequestBuddyList();

						m_localChatWindow->AddLogEntry( tr("Announcing in grid...") );
						llmgr.AnnounceInSim();

						m_localChatWindow->AddLogEntry( tr("Ready!") );
						
						// Tell any leftover IM windows that we are back.
						//
						NotifyImConnected( true );

						m_netState = NetStateConnected;

						Common::IsOnline( true );
					}
				}
				catch( LLC::AuthException x )
				{
					// A problem, go back to the login state to rectify
					//
					QString message = tr("Cannot log in! Reason: %1").arg( x.GetMessageText() );
					m_localChatWindow->AddLogEntry( message );
					MessageDialog::Critical( this, tr("Login Error!"), message );
					//
					m_netState = NetStateShutdown;
				}

				UpdateAppState();
			}
			break;

		// We are logged in and everything is happy. Process messages.
		//
		case NetStateConnected:
			{
				LLC::Manager llmgr;
				if( llmgr.IsOnline() )
				{
					llmgr.PumpMessages();
				}
				else
				{
					// We lost the connection--move to shutdown state
					//
					m_netState = NetStateShutdown;
					//
					Common::IsOnline( false );
				}
				//
				MessageDialog::PurgeHiddenDialogs();
			}
			break;

		case NetStateShutdown:
			// Do nothing
			Common::IsOnline( false );
			break;
	}

	if( m_netState != NetStateShutdown )
	{
		m_timerId = startTimer( MESSAGE_TIMEOUT_MSECS );
	}
}


MainWindow::ChatWindowPtr MainWindow::GetIMWindow( const QString& id, const QString& fullName, const bool create, const bool is_group )
{
	ChatWindowPtr chatWnd = m_imWindowMap[id];
	//
	if( !chatWnd && create )
	{
		QTabWidget*		tabWidget = m_ui->m_tabWidget;
		// Create one if non-existent
		//
		LLC::Manager llmgr;
		chatWnd.reset( new ChatWindow( tabWidget, id, fullName, m_myName, is_group ) );
		m_imWindowMap[id] = chatWnd;

		// Lookup the name
		//
		QString name( fullName );
		if( name.isEmpty() )
		{
			name = LS2Q( llmgr.GetNameFromCache( Q2LS(id) ) );
		}

		// Add to the tabWidget
		//
		tabWidget->addTab( chatWnd.get(), name );
		const int pageIndex = tabWidget->indexOf( chatWnd.get() );
		chatWnd->SetPageIndex( pageIndex );

		// Connect the close signal
		//
		chatWnd->ConnectCloseSignal ( boost::bind( &MainWindow::OnIMWindowCloseRequest	, this, _1 ) );
		chatWnd->ConnectTypingSignal( boost::bind( &MainWindow::OnIMTyping 				, this, _1 ) );
		chatWnd->ConnectImSignal    ( boost::bind( &MainWindow::CreateImTab				, this, _1, _2 ) );
	}
	//
	return chatWnd;
}


void MainWindow::HandleFriendRemoval( const long index )
{
	LLC::Manager llmgr;
	//
	QTreeWidgetItem* item	= m_ui->m_friendsList->topLevelItem( index );
	QString id      		= item->data( 0, Qt::UserRole ).toString();
	QString name    		= LS2Q( llmgr.GetNameFromCache( Q2LS(id) ) );
	QString msg 			= tr("Are you sure you want to remove %1 from your friends list?").arg(name);
	//
	MessageDialog::Question	( this
							, tr("Friend Removal Request")
							, msg
							, boost::bind( &LLC::Manager::TerminateFriendship, llmgr, Q2LS(id) )
							);
}


void MainWindow::CreateImTab( const QString& agentId, const QString& fullName )
{
	LLC::Manager	llmgr;
	//
	if( agentId == LS2Q(llmgr.GetAgentId()) )
	{
		// Never IM ourselves!
		//
		return;
	}
	//
	QString name = fullName;
	if( name.isEmpty() )
	{
		name = LS2Q( llmgr.GetNameFromCache( Q2LS(agentId) ) );
	}
	//
	ChatWindowPtr	chatWnd	= GetIMWindow( agentId, name, true /*create*/ );
	QTabWidget*		tabWidget = m_ui->m_tabWidget;

	// Make sure the page is visible
	//
	tabWidget->setCurrentIndex( chatWnd->GetPageIndex() );
	//
	if( !chatWnd->GetImId().isEmpty() )
	{
		tabWidget->setTabText( chatWnd->GetPageIndex(), name );
		SetTabIcon( chatWnd->GetPageIndex(), IM_NORMAL );
	}
	//
	chatWnd->SetFocus();
}


void MainWindow::CreateGroupTab( const QString& groupId, const QString& fullName )
{
	LLC::Manager	llmgr;
	QString			name		= fullName;
	QTabWidget*		tabWidget	= m_ui->m_tabWidget;

	if( name.isEmpty() )
	{
		name = LS2Q( llmgr.LookupGroupName( Q2LS(groupId) ) );
	}

	// Send a request to start a group chat. TODO: this is broken!
	//
	ChatWindowPtr	chatWnd	= GetIMWindow( groupId, name, false /*create*/, true /*is_group*/ );
	if( !chatWnd )
	{
		// Only send request if the window doesn't yet exist
		//
		llmgr.SendGroupChatStartRequest( Q2LS(groupId) );

		// Now create it
		//
		chatWnd	= GetIMWindow( groupId, name, true /*create*/, true /*is_group*/ );
	}

	// Make sure the page is visible
	//
	tabWidget->setCurrentIndex( chatWnd->GetPageIndex() );
	//
	if( !chatWnd->GetImId().isEmpty() )
	{
		tabWidget->setTabText( chatWnd->GetPageIndex(), name );
		SetTabIcon( chatWnd->GetPageIndex(), GROUP_NORMAL );
	}
	//
	chatWnd->SetFocus();
}


void MainWindow::OnFriendsListItemClicked( QTreeWidgetItem* index, int column )
{
	UpdateAppState();
}


void MainWindow::OnFriendsListItemDoubleClicked( QTreeWidgetItem* index, int column )
{
	CreateImTab( index->data( 0, Qt::UserRole ).toString() );
}


void MainWindow::OnGroupListItemClicked( QListWidgetItem* index )
{
	UpdateAppState();
}


void MainWindow::OnGroupListItemDoubleClicked( QListWidgetItem* index )
{
	CreateGroupTab( index->data( Qt::UserRole ).toString() );
}


void MainWindow::OnTabWidgetCurrentChanged( int selection )
{
	if( selection != -1 )
	{
		QTabWidget*	tabWidget = m_ui->m_tabWidget;
		ChatWindow* chatWnd = dynamic_cast<ChatWindow*>(tabWidget->widget( selection ));
		if( chatWnd && !chatWnd->GetImId().isEmpty() )
		{
			chatWnd->SetFocus();
			IconType type = (selection == 0)
						  ? LOCALCHAT
						  : chatWnd->IsGroup()
							? GROUP_NORMAL
							: IM_NORMAL;
			SetTabIcon( selection, type ); // Replace the icon
			//
		}
		//
		m_ui->m_actionFileClose->setEnabled( selection > 0 );
	}
}


void MainWindow::OnIMWindowCloseRequest( QString imId )
{
	// Drop it out of the map. wxWindows will return it to the free store.
	//
	ChatWindowPtr chatWnd = m_imWindowMap[imId];
	m_imWindowMap[imId].reset();

	// Now delete the page associated with the resident's chat session
	// from the tabWidget.
	//
	if( chatWnd )
	{
		m_ui->m_tabWidget->removeTab( chatWnd->GetPageIndex() );
	}
}


void MainWindow::OnIMTyping( bool typing )
{
	m_typingInIM = typing;
}


void MainWindow::OnTabWindowCloseRequest( int tabIndex )
{
	if( tabIndex == 0 )
	{
		// Cannot remove local chat tab...
		//
		return;
	}

	// Drop it out of the map. wxWindows will return it to the free store.
	//
	QTabWidget*	tabWidget = m_ui->m_tabWidget;
	ChatWindow* chatWnd = dynamic_cast<ChatWindow*>(tabWidget->widget( tabIndex ));

	// Removing tab
	//
	tabWidget->removeTab( tabIndex );

	// Now delete the chat window
	//
	if( chatWnd )
	{
		m_imWindowMap[chatWnd->GetImId()].reset();
	}
}


void MainWindow::OnActionFileLogin()
{
	Login();
}


void MainWindow::OnActionFileLogout()
{
	Logout();
}


void MainWindow::OnActionFileTeleportHome()
{
	LLC::Manager llmgr;
	llmgr.TeleportHome();
}


void MainWindow::OnActionFilePreferences()
{
	Preferences prefDlg( this );
	//
	if( prefDlg.exec() == QDialog::Accepted )
	{
		setFont( prefDlg.GetFont() );
	}
}


void MainWindow::OnActionFileExport()
{
	ExportWindow	exportDlg( this );
	//
	exportDlg.exec();
}


void MainWindow::OnActionFileClose()
{
	const int currentIndex = m_ui->m_tabWidget->currentIndex();
	//
	if( currentIndex > 0 )
	{
		OnTabWindowCloseRequest( currentIndex );
	}
}


void MainWindow::OnActionFileExit()
{
	close();
}


void MainWindow::SendOfferFriendship( const QString& agentId, const QString& searchKeyword )
{
	LLC::Manager llmgr;
	llmgr.OfferFriendship( Q2LS(agentId), Q2LS(tr("Will you be my friend?")) );

	m_lastSearch = searchKeyword;
}


void MainWindow::OnActionFriendAdd()
{
	SearchWindow searchDlg( this );
	searchDlg.SetSearchKeyword( m_lastSearch );
	//
	const int return_value = searchDlg.exec();
	switch( return_value )
	{
		case SearchWindow::ADD:
			{
				LLC::Manager llmgr;
				const QString agentId  = searchDlg.GetSelectedId();
				const QString fullName = searchDlg.GetSelectedName();
				//
				if( llmgr.IsFriend( Q2LS(agentId) ) )
				{
					QString msg = tr("%1 is already your friend!").arg( fullName );
					MessageDialog::Warning( this, tr("Notice"), msg, false );
				}
				else
				{
					QString msg = tr("Are you sure you want to offer %1 friendship?").arg( fullName );
					MessageDialog::Question	( this
											, tr("Offer Friendship")
											, msg
											, boost::bind	( &MainWindow::SendOfferFriendship
															, this
															, agentId
															, searchDlg.GetSearchKeyword()
															)
											);
				}
			}
			break;
			
		case SearchWindow::SEND_IM:
			CreateImTab( searchDlg.GetSelectedId(), searchDlg.GetSelectedName() );
			m_lastSearch = searchDlg.GetSearchKeyword();
			break;
	}
}


void MainWindow::OnActionFriendRemove()
{
	QTreeWidget*	 tree         = m_ui->m_friendsList;
    QTreeWidgetItem* selectedItem = tree->currentItem();
    //
    if( selectedItem )
    {
		HandleFriendRemoval( tree->indexOfTopLevelItem( selectedItem ) );
    }
}


void MainWindow::OnActionFriendIM()
{
	QTreeWidgetItem* item = m_ui->m_friendsList->currentItem();
	//
    if( item )
    {
		CreateImTab( item->data( 0, Qt::UserRole ).toString() );
    }
}


void MainWindow::DisplayTodoWarning()
{
	MessageDialog::Warning
		( this
		, tr("Attention!")
		, tr("This feature is not yet supported, but will be rolled out as soon as possible.")
		, false
	    ); 
}


void MainWindow::ClearChatTabs()
{
	// Clear tabs
	//
	QTabWidget* tabWidget = m_ui->m_tabWidget;
	const int tabCount = tabWidget->count();
	//
	// Leave LocalChat alone but clear out all other tabs
	//
	for( int index = 1; index < tabCount; ++index )
	{
		tabWidget->removeTab( index );
	}
	//
	m_imWindowMap.clear();
}


void MainWindow::OnActionGroupSearch()
{
	// TODO: Implement
	DisplayTodoWarning();
}


void MainWindow::OnActionGroupLeave()
{
	DisplayTodoWarning();

#if 0
	QListWidget*		list		= m_ui->m_groupList;
	QListWidgetItem*	selection	= list->currentItem();
	//
	if( selection )
	{
		const QString id = selection->data( Qt::UserRole ).toString();
		LLC::Manager llmgr;
		llmgr.SendGroupChatLeaveRequest( Q2LS(id) );
	}
#endif
}


void MainWindow::OnActionGroupIM()
{
	QListWidgetItem* item = m_ui->m_groupList->currentItem();
	//
    if( item )
    {
		//DisplayTodoWarning();
		CreateGroupTab( item->data( Qt::UserRole ).toString() );
    }
}


void MainWindow::OnActionHelpAbout()
{
	QString version = tr("SLiteChat Version %1.%2.%3")
					.arg(slitechat_VERSION_MAJOR)
					.arg(slitechat_VERSION_MINOR)
					.arg(slitechat_VERSION_PATCH);
	//
	QString message = tr("<h1>%1</h1>").arg(version)
					+ tr("<h2>A light-weight chat client for <a href=\"http://www.secondlife.com\">Second Life</a></h2>")
					+ tr("<p>Copyright (c) 2009-2010 by <a href=\"mailto:contact@m2osw.com\">Dooglio Mode</a><br>")
					+ tr("<a href=\"http://www.slitechat.org/\">http://www.slitechat.org/</a></p>")
					+ tr("<p>Artwork by IconTexto (Bruno Maia)<br>")
					+ tr("<a href=\"http://www.icontexto.com/\">http://www.icontexto.com/</a></p>")
					+ tr("<p>Some contributions by <i>Catherine Pfeffer</i></p>")
					;
	//
	MessageDialog::MessageDialogPtr dlg( new MessageDialog(this) );
	dlg->SetHtmlLabel( message );
	dlg->setWindowTitle( version );
	dlg->Show( false );
	MessageDialog::AddDialog( dlg );
}


void MainWindow::OnActionHelpCheckForUpdates()
{
	QNetworkRequest request( QUrl("http://files.slitechat.org/release.txt") );
	request.setRawHeader( "Accept", "text/plain" );
	m_networkManager.get( request );
}


void MainWindow::OnAddFriendSignal( LLC::String agent_id )
{
	const QString qAgentId( LS2Q(agent_id.GetString()) );
	
	// Check to see if we are already in the friends list.
	//
	QTreeWidget* friendsList = m_ui->m_friendsList;
	QTreeWidgetItemIterator it( friendsList );
	for( ; *it; ++it )
	{
		if( (*it)->data( 0, Qt::UserRole ).toString() == qAgentId )
		{
			// Already in list, so just stop here
			//
			return;
		}
	}

	// Save in list
	//
	LLC::Manager llmgr;
	//
	LLC::String	 name = llmgr.GetNameFromCache( agent_id );
	const bool	online	= llmgr.IsOnline( agent_id );
	QStringList columns;
	columns
		<< (online? tr("Online"): tr("Offline"))
		<< LS2Q(name);
	QTreeWidgetItem* item = new QTreeWidgetItem( columns );
	QFont font( item->font(0) );
	font.setBold( online? true: false );
	//
	item->setIcon( 0, GetOnlineIcon( online ) );
	item->setFont( 0, font );
	item->setFont( 1, font );
	item->setData( 0, Qt::UserRole, qAgentId );
	friendsList->addTopLevelItem( item );
}


void MainWindow::OnTerminateFrienshipSignal( LLC::String agent_id )
{
	const QString qAgentId( LS2Q(agent_id.GetString()) );
	QTreeWidget* friendsList = m_ui->m_friendsList;
	QTreeWidgetItemIterator it( friendsList );
	for( ; *it; ++it )
	{
		if( (*it)->data( 0, Qt::UserRole ).toString() == qAgentId )
		{
			const int index = friendsList->indexOfTopLevelItem( *it );
			delete friendsList->takeTopLevelItem( index );
			break;
		}
	}

	// Set sort order
	//
	friendsList->sortItems( 1, Qt::AscendingOrder  );
	friendsList->sortItems( 0, Qt::DescendingOrder );

	// Tell user about termination of friendship
	//
	LLC::Manager llmgr;
	LLC::String	 name = llmgr.GetNameFromCache( agent_id );
	m_localChatWindow->AddLogEntry( tr("%1 dissolved the friendship with you.").arg( LS2Q(name) ) );
}


void MainWindow::OnCacheSignal( LLC::String id, LLC::String fullName, bool is_group )
{
	// Get full name from cache
	//
	LLC::Manager llmgr;
	//LLC::String	 name		= llmgr.GetNameFromCache( id );
	QString		 qAgentId	= LS2Q(id);

	// Find chat window if it's there. If so, then update the page to the correct name
	//
	ChatWindowPtr chatWnd = GetIMWindow( LS2Q(id), LS2Q(fullName), false /*create*/ );
	//
	if( chatWnd )
	{
		// Make sure the page is visible
		//
		m_ui->m_tabWidget->setTabText( chatWnd->GetPageIndex(), chatWnd->GetFullName() );
	}
	
	// Check to see if we are already in the friends list.
	//
	QTreeWidget* friendsList = m_ui->m_friendsList;
	QTreeWidgetItemIterator it( friendsList );
	for( ; *it; ++it )
	{
		QTreeWidgetItem* item = *it;
		if( item->data( 0, Qt::UserRole ).toString() == qAgentId )
		{
			// Update list entry
			//
			const bool	online	= llmgr.IsOnline( id );
			//
			QFont font( item->font(0) );
			font.setBold( online? true: false );
			item->setText( 0, (online? tr("Online"): tr("Offline")) );
			item->setIcon( 0, GetOnlineIcon( online ) );
			item->setFont( 0, font );
			//
			item->setText( 1, LS2Q(fullName) );
			item->setFont( 1, font );

			// Set sort order
			//
			friendsList->sortItems( 1, Qt::AscendingOrder  );
			friendsList->sortItems( 0, Qt::DescendingOrder );
		}
	}
}


void MainWindow::OnGroupCacheSignal( LLC::String id, LLC::String group_name )
{
	QString qGroupId   = LS2Q(id);
	QString qGroupName = LS2Q(group_name);
	
	QListWidget* list = m_ui->m_groupList;
	const int count = list->count();
	for( int row = 0; row < count; ++row )
	{
		QListWidgetItem* item = list->item( row );
		if( item->data( Qt::UserRole ) == qGroupId )
		{
			// Already in list, so stop here
			//
			return;
		}
	}

	// Add the new item...this automatically parents the item and adds it to the widget
	//
	QListWidgetItem* item = new QListWidgetItem( qGroupName, list );
	item->setData( Qt::UserRole, qGroupId );
	//
	QIcon icon;
	icon.addPixmap( QPixmap(QString::fromUtf8(":/mainIcon/group_icon")), QIcon::Normal, QIcon::Off );
	item->setIcon( icon );
}


void MainWindow::OnGroupChatAgentUpdateSignal	( LLC::String session_id
												, LLC::String agent_id
												, bool entering
												)
{
	ChatWindowPtr chatWnd = GetIMWindow( LS2Q(session_id), QString(), false /*create*/, true /*is_group*/ );
	if( chatWnd )
	{
		chatWnd->UpdateNamesInChat( LS2Q(agent_id), entering );
	}
}


void MainWindow::SetTabIcon( const int index, const IconType type )
{
	QString iconName;
	//
	switch( type )
	{
		case OFFLINE:		iconName = QString::fromUtf8(":/mainIcon/offline_icon");	break;
		case LOCALCHAT:		iconName = QString::fromUtf8(":/mainIcon/home_icon");		break;
		case GROUP_NORMAL:	iconName = QString::fromUtf8(":/mainIcon/group_icon");		break;
		case IM_NORMAL:		iconName = QString::fromUtf8(":/mainIcon/user_icon");		break;
		case IM_TRAFFIC:	iconName = QString::fromUtf8(":/mainIcon/alert_icon");		break;
	}

    QIcon icon;
    icon.addPixmap( QPixmap(iconName), QIcon::Normal, QIcon::Off );
	QTabWidget* tabWidget = m_ui->m_tabWidget;
	tabWidget->setTabIcon( index, icon );
}


void MainWindow::OnRequestFinished( QNetworkReply* reply )
{
	if( reply->error() == QNetworkReply::NoError )
	{
		QString currentVersion( reply->readAll().data() );
		const int crPos = currentVersion.indexOf( '\n' );
		if( crPos != -1 )
		{
			currentVersion.remove( crPos, 1 );
		}
		//
		const QString thisVersion	= QString("%1.%2.%3")
			.arg(slitechat_VERSION_MAJOR)
			.arg(slitechat_VERSION_MINOR)
			.arg(slitechat_VERSION_PATCH);
		//
#ifdef _DEBUG
		std::cout << "currentVersion = " << currentVersion.toAscii().data() << std::endl;
		std::cout << "thisVersion = " << thisVersion.toAscii().data() << std::endl;
#endif
		//
		if( currentVersion > thisVersion )
		{
			MessageDialog::MessageDialogPtr dlg( new MessageDialog( this ) );
			dlg->setWindowTitle( tr("New Version of SLiteChat Available") );
			dlg->SetHtmlLabel(
					tr("<center><h2>Your current version:<br/><b>%1</b></h2>"
						"<h2>Available version for download:<br/><b>%2</b></h2>. "
						"<p>Please go to<br/><a href=\"http://www.slitechat.org/download\">"
						"http://www.slitechat.org/download</a><br/>to get the latest copy.</p></center>")
					.arg(thisVersion)
					.arg(currentVersion)
					);
			dlg->Show( false );
			MessageDialog::AddDialog( dlg );
		}
		else if( !m_initialCheck )
		{
			MessageDialog::MessageDialogPtr dlg( new MessageDialog( this ) );
			dlg->setWindowTitle( tr("Update status") );
			dlg->SetHtmlLabel( tr("You have the latest version of <b>SLiteChat</b>.") );
			dlg->Show( false );
			MessageDialog::AddDialog( dlg );
		}
	}
	else
	{
		if( !m_initialCheck )
		{
			MessageDialog::Critical	( this
									, tr("Error!")
									, tr("There was a problem connecting to the server. Please try again later.")
									, false );
		}
	}
	//
	m_initialCheck = false;
}


void MainWindow::OnImSignal	( LLC::String id
							, LLC::String from
							, bool has_me
							, LLC::String message
							, LLC::String translated_msg
							, LLC::String detected_lang
							)
{
	ChatWindowPtr chatWnd = GetIMWindow( LS2Q(id), LS2Q(from), true /*create*/ );
	QTabWidget* tabWidget = m_ui->m_tabWidget;
	//
	if( chatWnd )
	{
		chatWnd->AddInstantMessage	( LS2Q(from)
									, has_me
									, LS2Q(message)
									, LS2Q(translated_msg)
									, LS2Q(detected_lang)
									);
		//
		IconType type	= (tabWidget->currentIndex() == chatWnd->GetPageIndex())
			? IM_NORMAL
			: IM_TRAFFIC;
		SetTabIcon( chatWnd->GetPageIndex(), type );
	}
	//
	ChatWindow* tabChatWnd = dynamic_cast<ChatWindow*>(tabWidget->widget( tabWidget->currentIndex() ));
	//
	if( tabChatWnd )
	{
		tabChatWnd->SetFocus();
	}
}


void MainWindow::OnGroupChatSignal	( LLC::String id
									, LLC::String groupName
									, LLC::String from
									, bool has_me
									, LLC::String message
									, LLC::String translated_msg
									, LLC::String detected_lang
									)
{
#ifdef _DEBUG
	std::cout
		<< "MainWindow::OnGroupChatSignal(): group: '"
		<< groupName.GetString()
		<< "', sender: '"
		<< from.GetString()
		<< "', message: "
		<< message.GetString()
		<< "'" << std::endl;
#endif

	LLC::Manager llmgr;
	if( llmgr.GetBool( IGNORE_GROUP_IMS ) )
	{
		// Ignore all group chat IMS
		//
#ifdef _DEBUG
		std::cout << "Group chat being ignored" << std::endl;
#endif
		return;
	}

	ChatWindowPtr chatWnd = GetIMWindow( LS2Q(id), LS2Q(groupName), true /*create*/, true /*is_group*/ );
	QTabWidget* tabWidget = m_ui->m_tabWidget;
	//
	if( chatWnd )
	{
		chatWnd->AddInstantMessage	( LS2Q(from)
									, has_me
									, LS2Q(message)
									, LS2Q(translated_msg)
									, LS2Q(detected_lang)
									);
		//
		IconType type	= (tabWidget->currentIndex() == chatWnd->GetPageIndex())
						? GROUP_NORMAL
						: IM_TRAFFIC;
		SetTabIcon( chatWnd->GetPageIndex(), type );
	}
	//
	ChatWindow* tabChatWnd = dynamic_cast<ChatWindow*>(tabWidget->widget( tabWidget->currentIndex() ));
	//
	if( tabChatWnd )
	{
		tabChatWnd->SetFocus();
	}
}


void MainWindow::OnLocalChatSignal	( LLC::String id
									, LLC::String from
									, LLC::String verb
									, bool has_me
									, LLC::String text
									, LLC::String translated_msg
									, LLC::String detected_lang
									)
{
	m_localChatWindow->AddLocalChatMessage	( LS2Q(from)
											, LS2Q(verb)
											, has_me
											, LS2Q(text)
											, LS2Q(translated_msg)
											, LS2Q(detected_lang)
											);
	//SetTabIcon( 0, IM_TRAFFIC );
}


void MainWindow::OnTypingSignal( LLC::String id, LLC::String from, bool start )
{
	ChatWindowPtr chatWnd = GetIMWindow( LS2Q(id), LS2Q(from), false /*create*/ );
	//
	if( chatWnd )
	{
		// Only add to an existing window
		//
		chatWnd->AddTypingStatus( QString( from.GetString() ), start );
		//
		QTabWidget* tabWidget = m_ui->m_tabWidget;
		IconType type	= (tabWidget->currentIndex() == chatWnd->GetPageIndex())
						? chatWnd->IsGroup()
							? GROUP_NORMAL
							: IM_NORMAL
						: IM_TRAFFIC;
		SetTabIcon( chatWnd->GetPageIndex(), type );
	}
}


void MainWindow::OnOnlineSignal( LLC::String id, bool online )
{
	m_localChatWindow->AddOnlineStatus( LS2Q( id ), online );

	ChatWindowPtr chatWnd = GetIMWindow( LS2Q(id), QString(), false /*create*/ );
	//
	if( chatWnd )
	{
		chatWnd->AddOnlineStatus( LS2Q( id ), online );
	}

	QString qAgentId = LS2Q(id);
	QTreeWidget* friendsList = m_ui->m_friendsList;
	QTreeWidgetItemIterator it( friendsList );
	bool friend_found = false;
	for( ; *it; ++it )
	{
		QTreeWidgetItem* item = *it;
		if( item->data( 0, Qt::UserRole ).toString() == qAgentId )
		{
			item->setText( 0, online? tr("Online"): tr("Offline") );
			//
			QFont font( item->font(0) );
			font.setBold( online? true: false );
			//
			item->setIcon( 0, GetOnlineIcon( online ) );
			item->setFont( 0, font );
			item->setFont( 1, font );
			//
			friend_found = true;
			break;
		}
	}

	if( !friend_found )
	{
		LLC::Manager mgr;

		// If we are here, then we just got a new friend. Add the new friend.
		//
		OnCacheSignal( id, mgr.GetNameFromCache(id), false );
	}

	// Set sort order
	//
	friendsList->sortItems( 1, Qt::AscendingOrder  );
	friendsList->sortItems( 0, Qt::DescendingOrder );
}


void MainWindow::OnFriendshipAcceptSignal( LLC::String agentId, LLC::String fullName, bool is_group )
{
	OnAddFriendSignal( agentId );
	//
	const QString msg = tr( "%1 has accepted your friendship request.").arg( LS2Q(fullName) );
	MessageDialog::Information( this, tr("Friendship accepted"), msg, false ); 
}


void MainWindow::OnFriendshipDeclineSignal( LLC::String agentId, LLC::String fullName, bool is_group )
{
	const QString msg = tr("%1 has declined your friendship request.").arg( LS2Q(fullName) );
	MessageDialog::Information( this, tr("Friendship rejected"), msg, false ); 
}


void MainWindow::SendConfirmFriendship( LLC::String agent_id, LLC::String session_id, LLC::String ip_and_port )
{
	// Tell the server you accept
	//
	LLC::Manager llmgr;
	llmgr.AcceptFriendship( session_id, ip_and_port );

	// Now add your new friend to the friends list
	//
	OnAddFriendSignal( agent_id );
}

		
void MainWindow::OnFriendOfferSignal( LLC::String agent_id, LLC::String full_name, LLC::String message, LLC::String session_id, bool online, LLC::String ip_and_port )
{
	LLC::Manager llmgr;

	const QString log_message = tr("Offer of friendship from %1: %2").arg(LS2Q(full_name)).arg(LS2Q(message));
	m_localChatWindow->AddLogEntry( log_message );

	QString user_msg = tr("%1 is offering to be your friend with the following message: %2\n\nDo you wish to add %1 as a friend?")
			.arg(LS2Q(full_name))
			.arg(LS2Q(message))
			;
	//
	MessageDialog::Question	( this
							, tr("Confirm Friendship")
							, user_msg
							, boost::bind( &MainWindow::SendConfirmFriendship, this, agent_id, session_id, ip_and_port )
							, boost::bind( &LLC::Manager::DeclineFriendship  , llmgr, session_id, ip_and_port )
							);
}


void MainWindow::OnMessageBoxSignal( LLC::String message )
{
	MessageDialog::Information( this, tr("Notice"), LS2Q(message), false );
}


void MainWindow::OnForcedQuitSignal( LLC::String message )
{
	OnLogoutReplySignal();
	//
	MessageDialog::Critical( this, tr("Logout Notice"), LS2Q(message), false );
	//
	m_localChatWindow->AddLogEntry( LS2Q(message) );
}


void MainWindow::OnLogoutReplySignal()
{
	m_netState = NetStateShutdown;
	killTimer( m_timerId );

	m_localChatWindow->AddLogEntry( tr("You have been logged out...") );

#if 0
	// Clear tabs
	//
	QTabWidget* tabWidget = m_ui->m_tabWidget;
	const int tabCount = tabWidget->count();
	//
	// Leave LocalChat alone but clear out all other tabs
	//
	for( int index = 1; index < tabCount; ++index )
	{
		tabWidget->removeTab( index );
	}

	//
	ChatWindowMap::iterator			iter = m_imWindowMap.begin();
	const ChatWindowMap::iterator	end  = m_imWindowMap.end();
	for( ; iter != end; ++iter )
	{
		ChatWindow* chatWnd = iter->second;
		delete chatWnd;
	}
	//
	m_imWindowMap.clear();
#endif

	// Clear friends list
	//
	QTreeWidget* friendsList = m_ui->m_friendsList;
	QTreeWidgetItemIterator it( friendsList );
	for( ; *it; ++it )
	{
		const int index = friendsList->indexOfTopLevelItem( *it );
		delete friendsList->takeTopLevelItem( index );
	}
	friendsList->clear();

	// Clear groups list
	//
	QListWidget* list = m_ui->m_groupList;
	const int count = list->count();
	for( int row = 0; row < count; ++row )
	{
		QListWidgetItem* item = list->item( row );
		list->removeItemWidget( item );
		delete item;
	}
	list->clear();

	// Notify application elements
	//
	NotifyImConnected( false );
	UpdateAppState();
}


void MainWindow::OnAgentMovementCompleteSignal()
{
	LLC::Manager llmgr;
	LLC::String region_name;
	int x, y ,z;
	llmgr.GetLM( region_name, x, y, z );
	LLC::String slurl = llmgr.GetSLURL();
	QString log_message = tr("Entering %1 (%2, %3, %4), %5")
		.arg(LS2Q(region_name))
		.arg(x).arg(y).arg(z)
		.arg(LS2Q(slurl));
	m_localChatWindow->AddLogEntry( log_message );
}


void MainWindow::OnTeleportRequestSignal( LLC::String name, LLC::String message, LLC::String lureId )
{
	LLC::Manager llmgr;
	MessageDialog::Question( this
							, tr("Teleport Request from %1")    .arg(LS2Q(name)   )
							, tr("%1\nDo you wish to teleport?").arg(LS2Q(message))
							, boost::bind( &LLC::Manager::TeleportViaLure, llmgr, lureId )
							);
}


void MainWindow::OnTeleportRequestedSignal( LLC::String slurl )
{
	m_localChatWindow->AddLogEntry( tr("Teleport requested to: \"%1\"").arg(LS2Q(slurl)) );
}


void MainWindow::OnTeleportStartSignal( bool can_cancel )
{
	m_localChatWindow->AddLogEntry( tr("Teleport started, please wait") );
}


void MainWindow::OnTeleportProgressSignal( bool can_cancel, LLC::String progress_message )
{
	m_localChatWindow->AddLogEntry( tr("Teleport in progress: %1").arg(LS2Q(progress_message)) );
}


void MainWindow::OnTeleportLocalSignal()
{
	m_localChatWindow->AddLogEntry( tr("Teleport in local") );
}


void MainWindow::OnTeleportFailedSignal( LLC::String failed_message )
{
	m_localChatWindow->AddLogEntry( tr("Teleport in failed: %1").arg(LS2Q(failed_message)) );
}


void MainWindow::OnTeleportFinishSignal()
{
	m_localChatWindow->AddLogEntry( tr("Teleport finished.") );
}


// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
