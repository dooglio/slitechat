/** 
 * \brief Header for ChatWindow, which handles a local chat or IM session.
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
#ifndef __CHATWINDOW_H__
#define __CHATWINDOW_H__

#include "LLChatLib.h"

#include <boost/signals.hpp>

#include <QWidget>
#include <QTimer>
#include <QDate>

#include "ui_ChatWindow.h"

#ifdef signals
#	undef signals
#endif

class ChatWindow
	: public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(ChatWindow)

public:
	explicit ChatWindow( QWidget* parent );
	explicit ChatWindow( QWidget* parent, const QString& id, const QString& fullName, const QString& myName, const bool is_group = false );
	virtual ~ChatWindow();

	void		SetConnected( const bool connected );
	void		SetFocus();

	int			GetPageIndex() const { return m_pageIndex; }
	void		SetPageIndex( const int val )	{ m_pageIndex = val; }
	QString 	GetImId() const { return m_imId; }
	QString 	GetFullName() const { return m_fullName; }
	void		SetMyName( const QString& name ) { m_myName = name; }
	bool		IsGroup() const { return m_isGroup; }

	void		AddInstantMessage	( const QString& from
									, const bool has_me
									, const QString& message
									, const QString& translated_msg
									, const QString& detected_lang
									);
	void		AddLocalChatMessage	( const QString& from
									, const QString& verb
									, const bool has_me
									, const QString& message
									, const QString& translated_msg
									, const QString& detected_lang
									);
	void		AddTypingStatus( const QString& from, bool start );
	void		AddOnlineStatus( const QString& id, bool online );
	void		AddLogEntry( const QString& message, bool error = false );

	void		UpdateNamesInChat( const QString& agentId, const bool entering );
	void		OnCacheSignal( LLC::String agent_id, LLC::String fullName, bool is_group );

	typedef boost::signal<void (bool)> BoolSignal;
	void ConnectTypingSignal( const BoolSignal::slot_type& slot ) { m_typingConnection = m_typingSignal.connect(slot); }
	//
	typedef boost::signal<void (QString)> CloseSignal;
	void ConnectCloseSignal( const CloseSignal::slot_type& slot ) { m_closeConnection = m_closeSignal.connect(slot); }
	//
	typedef boost::signal<void (QString, QString)> ImSignal;
	void ConnectImSignal( const ImSignal::slot_type& slot	)     { m_imConnection = m_imSignal.connect(slot); }

protected:
	void		timerEvent( QTimerEvent* event );

private:
	Ui_ChatWindow*  m_ui;
	bool            m_connected;
	int             m_pageIndex;
	QString         m_imId;
	QString         m_fullName;
	QString         m_myName;
	LLC::Connection m_typingConnection;
	LLC::Connection m_closeConnection;
	LLC::Connection m_friendAcceptConnection;
	LLC::Connection m_friendTerminateConnection;
	LLC::Connection m_cacheConnection;
	LLC::Connection m_localAvsConnection;
	LLC::Connection m_imConnection;
	BoolSignal      m_typingSignal;
	CloseSignal     m_closeSignal;
	ImSignal        m_imSignal;
	bool            m_isGroup;
	bool            m_isLocalChat;
	bool            m_typingMessagePending;
	int             m_isTyping;
	int             m_timerId;
	int             m_lastChannel;
	QDate           m_startDate;
	QString         m_history;

	// Private methods
	//
	void Load();
	void Save();
	void InitPanel( const bool isIMWindow, const bool showRoomList );
	bool PersistConvo() const;
	bool ShowTimestamps() const;
	bool ShowSourceLanguage() const;
	void MoveCursorToEnd();
	void ActivateWindow();
	void AddText( const QString& text, const bool moveCursor = true );
	void RemoveTypingMessage();
	void StartTimer( const bool reset, const bool start = true );
	void StopTimer( const bool reset ) { StartTimer( reset, false ); }
	QString StripChannelNumber( const QString &mesg, int* channel );

	void		AddInstantMessage	( const QString& from
									, const bool has_me
									, const QString& message
									);
	void		AddLocalChatMessage	( const QString& from
									, const QString& verb
									, const bool has_me
									, const QString& message
									);
	
	// LLC events
	//
	void OnFriendshipAccept( LLC::String agentId );
	void OnTerminateFriendship( LLC::String agentId );
	void OnLocalAvsPresent();
	
private slots:
	// GUI events
	//
	void OnTextInputReturnPressed();
	void OnTextInputTextEdited( QString );
	void OnAddButtonClicked();
	void OnCloseButtonClicked();
	void OnAnchorClicked( QUrl url );
	void OnItemDoubleClicked( QTreeWidgetItem* item, int column );
	void OnSendIM();
	void OnChangeLanguage();
};

#endif //__CHATWINDOW_H__

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
