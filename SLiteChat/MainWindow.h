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

#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <vector>
#include <map>

#include "LLChatLib.h"
#include "ChatWindow.h"
#include "LoginWindow.h"

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QShowEvent>
#include <QCloseEvent>
#include <QNetworkAccessManager>

#include <boost/shared_ptr.hpp>

#include "ui_MainWindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
	Q_DISABLE_COPY(MainWindow)

public:
	explicit MainWindow( QWidget* w = 0 );
	virtual ~MainWindow();

protected:
    void showEvent				( QShowEvent * event );
    void closeEvent				( QCloseEvent* event );
	void timerEvent				( QTimerEvent* event );
	void OnIMWindowCloseRequest	( QString imId );

private:
	Ui_MainWindow*	m_ui;

	typedef boost::shared_ptr<ChatWindow> ChatWindowPtr;
	typedef std::map<QString,ChatWindowPtr> ChatWindowMap;
	ChatWindowMap	m_imWindowMap;
	ChatWindow*		m_localChatWindow;

	// Application state
	//
	typedef enum
	{
		NetStateLogin,
		NetStateConnected,
		NetStateShutdown
	} NetStates;

	NetStates	m_netState;
	QString		m_myName;
	QString		m_lastSearch;
	int			m_timerId;
	bool		m_initialCheck;
	bool		m_typingInIM;

	QNetworkAccessManager	m_networkManager;
	LoginWindow::LoginInfo	m_loginInfo;

	static bool	m_isOnline;

	// Private methods
	//
	void UpdateAppState			();
	void Login					();
	void Logout					();
	ChatWindowPtr GetIMWindow	( const QString& id, const QString& fullName, const bool create, const bool is_group = false );
	void NotifyImConnected		( const bool online );
	void ConnectMgrSignals		();
	void HandleFriendRemoval	( const long index );
	void DisplayTodoWarning		();
	void ClearChatTabs			();

	void CreateImTab			( const QString& agentId, const QString& fullName = QString() );
	void CreateGroupTab			( const QString& groupId, const QString& fullName = QString() );

	typedef enum { OFFLINE, LOCALCHAT, IM_NORMAL, GROUP_NORMAL, IM_TRAFFIC } IconType;
	void SetTabIcon( const int index, const IconType type );

	// LLChatLib Signals handlers
	//
	void OnAddFriendSignal			( LLC::String id );
	void OnTerminateFrienshipSignal	( LLC::String id );
	void OnCacheSignal				( LLC::String id, LLC::String fullName, bool is_group );
    void OnGroupCacheSignal			( LLC::String id, LLC::String group_name );
	void OnGroupChatAgentUpdateSignal	( LLC::String session_id
										, LLC::String agent_id
										, bool entering
										);
	void OnImSignal					( LLC::String id
									, LLC::String from
									, bool has_me
									, LLC::String message
									, LLC::String translated_msg
									, LLC::String detected_lang
									);
	void OnGroupChatSignal			( LLC::String id
									, LLC::String groupName
									, LLC::String from
									, bool has_me
									, LLC::String message
									, LLC::String translated_msg
									, LLC::String detected_lang
									);
	void OnLocalChatSignal			( LLC::String id
									, LLC::String from
									, LLC::String verb
									, bool has_me
									, LLC::String text
									, LLC::String translated_msg
									, LLC::String detected_lang
									);
	void OnTypingSignal				( LLC::String id, LLC::String from, bool start );
	void OnOnlineSignal				( LLC::String id, bool online );
	void OnFriendshipAcceptSignal	( LLC::String agentId, LLC::String fullName, bool is_group );
	void OnFriendshipDeclineSignal	( LLC::String agentId, LLC::String fullName, bool is_group );
	void OnFriendOfferSignal		( LLC::String agent_id, LLC::String full_name, LLC::String message, LLC::String session_id, bool online, LLC::String ip_and_port );
	void OnMessageBoxSignal			( LLC::String message );
	void OnForcedQuitSignal			( LLC::String message );
	void OnLogoutReplySignal		();
	void OnAgentMovementCompleteSignal();

	void SendOfferFriendship( const QString& agentId, const QString& searchKeyword );
	void SendConfirmFriendship( LLC::String agent_id, LLC::String session_id, LLC::String ip_and_port );

	// Tp support
	//
	void OnTeleportRequestSignal( LLC::String name, LLC::String message, LLC::String lureId );
	void OnTeleportRequestedSignal( LLC::String slurl );
	void OnTeleportStartSignal( bool can_cancel );
	void OnTeleportProgressSignal( bool can_cancel, LLC::String progress_message );
	void OnTeleportLocalSignal();
	void OnTeleportFailedSignal( LLC::String failed_message );
	void OnTeleportFinishSignal();

	// Private methods (gui widgets)
	//
private slots:
	void OnFriendsListItemClicked		( QTreeWidgetItem* index, int column );
	void OnFriendsListItemDoubleClicked	( QTreeWidgetItem* index, int column );
	//
	void OnGroupListItemClicked			( QListWidgetItem* index );
	void OnGroupListItemDoubleClicked	( QListWidgetItem* index );
	//
	void OnTabWidgetCurrentChanged		( int index    );
	void OnTabWindowCloseRequest		( int tabIndex );
	void OnIMTyping						( bool typing  );

	////////////////////////////////////////////
	// File menu
	//
	void OnActionFileLogin		();
	void OnActionFileLogout		();
	void OnActionFileTeleportHome();
	void OnActionFilePreferences();
	void OnActionFileExport		();
	void OnActionFileClose		();
	void OnActionFileExit		();
	//
	// Friend menu
	//
	void OnActionFriendAdd		();
	void OnActionFriendRemove	();
	void OnActionFriendIM		();
	//                         
	// Group menu
	//
	void OnActionGroupSearch	();
	void OnActionGroupLeave 	();
	void OnActionGroupIM	  	();
	//
	// Help menu
	//
	void OnActionHelpAbout		();
	void OnActionHelpCheckForUpdates();
	
	// For online update checks
	//
	void OnRequestFinished( QNetworkReply* reply );
};

#endif //__MAINWINDOW_H__

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
