/** 
 * \brief ChatWindow methods
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

#include "ChatWindow.h"
#include "LLChatLib.h"
#include "Utility.h"
#include "Common.h"
#include "Config.h"
#include "MessageDialog.h"

#include <iostream>

#include <boost/bind.hpp>

#include <QFile>
#include <QTextStream>
#include <QTextStream>
#include <QDateTime>
#include <QTime>
#include <QDate>
#include <QApplication>
#include <QRegExp>
#include <QUrl>
#include <QDesktopServices>

#ifndef QT_NO_EMIT
#undef emit
#endif

#define TIMEOUT_MSECS		1000
#define TYPING_TIMEOUT		3

#define FULLPATH_TEMPLATE	"%1/%2.txt"


namespace
{


QString GetPersistFullPath( const QString name )
{
	LLC::Manager mgr;
	QString userPath = LS2Q(mgr.GetUserSettingsPath());
	return QString(FULLPATH_TEMPLATE).arg(userPath).arg(name);
}


QString GetLanguageName( const LLC::String& agent_id )
{
	LLC::Manager llmgr;
	//
	const LLC::String lang_id	= llmgr.GetAgentLanguage( agent_id );
	const bool isAuto			= llmgr.GetAgentLanguageAuto( agent_id );
	//
	QString lng = LS2Q(lang_id);
	QString language = Common::ConvertLanguage( lng );
	if( isAuto )
	{
		language = QObject::tr("%1 (auto)", "Automatic language detection, leave %1 alone").arg(language);
	}

	return language;
}

}


ChatWindow::ChatWindow( QWidget* parent )
	: QWidget( parent )
	, m_imId("LocalChat")
	, m_connected(true)
	, m_pageIndex(-1)
	, m_isLocalChat(true)
	, m_isGroup(false)
	, m_typingMessagePending(false)
	, m_isTyping(0)
    , m_ui(0)
	, m_timerId(-1)
	, m_lastChannel(0)
{
	InitPanel( false /*isIMWindow*/, true /*showRoomList*/ );
	//
	//m_ui->m_dockWidget->setWindowTitle( "Nearby Residents (Local Chat)" );
	//
	m_ui->m_textEdit->setSource( QUrl() );
	Load();
	m_startDate = QDate::currentDate();

	QString msg = tr("Welcome to <b>SLiteChat Version %1.%2.%3</b>! Click the <i>Login</i> button to get started.")
					.arg(slitechat_VERSION_MAJOR)
					.arg(slitechat_VERSION_MINOR)
					.arg(slitechat_VERSION_PATCH);
	AddLogEntry( msg, false /*error*/ );

#if 0
	AddLogEntry( "\ntest message with url: "
				"http://slitechat.dooglio.net/files/test.exe\n"
				"and that's the end and one more:\n"
				"http://www.dooglio.net/ and now that's the end!\n"
				"Oh and not quite, here is a slurl: secondlife://Italian%20Continent%201/46/76/2001",
				false /*error*/ );
#endif
}


ChatWindow::ChatWindow( QWidget* parent, const QString& id, const QString& fullName, const QString& myName, const bool is_group )
	: QWidget( parent )
	, m_imId(id)
	, m_fullName(fullName)
	, m_myName(myName)
	, m_connected(true)
	, m_pageIndex(-1)
	, m_isLocalChat(false)
	, m_isGroup(is_group)
	, m_typingMessagePending(false)
	, m_isTyping(0)
    , m_ui(0)
	, m_timerId(-1)
	, m_lastChannel(0)
{
	InitPanel( true /*isIMWindow*/, is_group /*showRoomList*/  );
	//
#if 0
	if( is_group )
	{
		m_ui->m_dockWidget->setWindowTitle( tr("Residents in %1 chat").arg(fullName) );
	}
#endif
	//
	Load();
	m_startDate = QDate::currentDate();
	QString msg = tr("Starting conversation with %1").arg(fullName);
	AddLogEntry( msg, false /*error*/ );
}


ChatWindow::~ChatWindow()
{
	m_closeConnection			.disconnect();
	m_friendAcceptConnection	.disconnect();
	m_friendTerminateConnection	.disconnect();
	m_cacheConnection			.disconnect();
	m_localAvsConnection		.disconnect();
	m_imConnection				.disconnect();
	//
	Save();
	//
	delete m_ui;
}


bool ChatWindow::PersistConvo() const
{
	LLC::Manager llmgr;
	return llmgr.GetBool( m_isLocalChat? PERSISTLC: PERSISTIM );
}


bool ChatWindow::ShowTimestamps() const
{
	// Check to see if timestamps are turned on
	//
	LLC::Manager llmgr;
	return llmgr.GetBool( TIMESTAMPS );
}


bool ChatWindow::ShowSourceLanguage() const
{
	// Check to see if timestamps are turned on
	//
	LLC::Manager llmgr;
	return llmgr.GetBool( ECHO_SOURCE );
}


void ChatWindow::Load()
{
	if( PersistConvo() )
	{
		QString user_path = GetPersistFullPath( m_imId );
		QFile inputFile( user_path );
		if( inputFile.open( QIODevice::ReadOnly | QIODevice::Text ) )
		{
			QString htmlInput("<font color=\"gray\">");
			while( !inputFile.atEnd() )
			{
				QByteArray line		= inputFile.readLine();
				QString    strLine	= QString::fromUtf8(line.constData());
				htmlInput += QString("%1<br>").arg(strLine);
				m_history += strLine;
			}
			//
			if( m_isLocalChat )
			{
				htmlInput += tr("-- Local chat logging enabled --<br>");
			}
			else
			{
				htmlInput += tr("-- Instant message logging enabled --<br>");
			}
			//
			htmlInput += "</font>";
			//
			QTextBrowser* editor = m_ui->m_textEdit;
			editor->setHtml( htmlInput );
			//
			MoveCursorToEnd();
		}
	}
}


void ChatWindow::Save()
{
	if( PersistConvo() )
	{
		LLC::Manager mgr;
		QString user_path = GetPersistFullPath( m_imId );
		QFile outputFile( user_path );
		if( outputFile.open( QIODevice::WriteOnly | QIODevice::Text ) )
		{
#if 0
			QTextBrowser* editor = m_ui->m_textEdit;
			editor->clear();
			editor->insertHtml( m_history );
			outputFile.write( editor->toPlainText().toUtf8() );
#else
			outputFile.write( m_history.toUtf8() );
#endif
		}
	}
}


void ChatWindow::InitPanel( const bool isIMWindow, const bool showRoomList )
{
	LLC::Manager mgr;
	//
    m_ui = new Ui_ChatWindow;
	m_ui->setupUi( this );
	//
	if( showRoomList )
	{
		// TODO: Read from the settings for the splitter size
		//
#if 1
		const int default_width = 200;
			QList<int> sizes = m_ui->m_splitter->sizes();
		sizes[0] = width() - default_width;
		sizes[1] = default_width;
		m_ui->m_splitter->setSizes( sizes );
#endif
		//
		m_cacheConnection = mgr.ConnectCacheSignal( boost::bind( &ChatWindow::OnCacheSignal, this, _1, _2, _3 ) );

		m_ui->m_avList->insertAction( 0, m_ui->m_actionSendIM );
		m_ui->m_avList->insertAction( 0, m_ui->m_actionChangeLanguage );
	}
	else
	{
		m_ui->m_splitter->widget( 1 )->hide();
		//m_ui->m_dockWidget->hide();
	}
	//
	if( isIMWindow )
	{
		if( m_isGroup || mgr.IsFriend( Q2LS(m_imId) ) )
		{
			// Hide the add button if already a friend
			//
			m_ui->m_addButton->setEnabled( false );
			m_ui->m_addButton->hide();
		}
		else
		{
			m_ui->m_addButton->setEnabled( true );
		}

		if( !m_isGroup )
		{
			m_friendAcceptConnection    = mgr.ConnectFriendAcceptSignal			( boost::bind( &ChatWindow::OnFriendshipAccept    , this, _1 ) );
			m_friendTerminateConnection = mgr.ConnectTerminateFriendshipSignal	( boost::bind( &ChatWindow::OnTerminateFriendship , this, _1 ) );
		}
	}
	else
	{
		m_localAvsConnection = mgr.ConnectLocalAgentsPresentSignal( boost::bind( &ChatWindow::OnLocalAvsPresent, this ) );
		//
		const int count = m_ui->m_topButtonLayout->count();
		for( int index = 0; index < count; ++index )
		{
			QWidget* widget = m_ui->m_topButtonLayout->itemAt( index )->widget();
			if( widget )
			{
				widget->hide();
			}
		}
		//
		layout()->removeItem( m_ui->m_topButtonLayout );
		layout()->update();
	}

	m_ui->m_textInput->setFocus( Qt::OtherFocusReason );
}


void ChatWindow::MoveCursorToEnd()
{
	QTextBrowser* editor = m_ui->m_textEdit;
	QTextCursor cursor = editor->textCursor();
	cursor.movePosition( QTextCursor::End );
	editor->setTextCursor( cursor );
}


void ChatWindow::ActivateWindow()
{
	if( !m_isLocalChat )
	{
		QApplication::alert( this );
	}
}


namespace
{


QString WrapHTML( const QString& text )
{
	QString retText = text;
	//
	QRegExp rx("\\b(http|secondlife)(://[%a-zA-Z0-9-./]*)\\b");
	//
	int pos = 0;
	int last_len = -1;
	//
	while( (pos = rx.indexIn( text, pos )) > -1 )
	{
		const int len = rx.matchedLength();
		//
		if( last_len == -1 )
		{
			retText = text.left( pos );
		}
		else
		{
			retText += text.mid( last_len, pos - last_len );
		}
		//
		QString url = rx.cap( 1 ) + rx.cap( 2 );
		//
		retText += QString("<a href=\"%1\">%1</a>").arg(url);
		last_len = pos+len;
		//
		pos += len;
	}

	if( last_len > -1 )
	{
		retText += text.mid( last_len );
	}

	return retText;
}


QString EscapeMessage( const QString& text )
{
	QString retText = text;
	//
	QRegExp rx("(<|>)");
	//
	int pos = 0;
	int last_len = -1;
	//
	while( (pos = rx.indexIn( text, pos )) > -1 )
	{
		const int len = rx.matchedLength();
		//
		if( last_len == -1 )
		{
			retText = text.left( pos );
		}
		else
		{
			retText += text.mid( last_len, pos - last_len );
		}
		//
		QString bracket = rx.cap( 1 );
		//
		if( bracket == ">" )
		{
			retText += QString("&gt;");
		}
		else
		{
			retText += QString("&lt;");
		}
		//
		last_len = pos+len;
		//
		pos += len;
	}

	if( last_len > -1 )
	{
		retText += text.mid( last_len );
	}

	return retText;
}


QString CR2BR( const QString& text )
{
	QString retText = text;
	//
	QRegExp rx("\\n");
	//
	int pos = 0;
	int last_len = -1;
	//
	while( (pos = rx.indexIn( text, pos )) > -1 )
	{
		const int len = rx.matchedLength();
		//
		if( last_len == -1 )
		{
			retText = text.left( pos );
		}
		else
		{
			retText += text.mid( last_len, pos - last_len );
		}
		//
		retText += QString("<br/>");
		last_len = pos+len;
		//
		pos += len;
	}

	if( last_len > -1 )
	{
		retText += text.mid( last_len );
	}

	return retText;
}

}
// namespace


void ChatWindow::AddText( const QString& text, const bool moveCursor )
{
	QTextBrowser* editor = m_ui->m_textEdit;
	//
	if( moveCursor )
	{
		MoveCursorToEnd();
	}
	//
	if( m_startDate != QDate::currentDate() )
	{
		// Clear out text and put in history text, because it contains full date/time stamps.
		//
		m_startDate = QDate::currentDate();
		editor->clear();
		editor->insertHtml( m_history );
	}
	//
	if( ShowTimestamps() )
	{
		QString currentTime = QTime::currentTime().toString("hh:mm");
		QString timeStamp   = QString("<font color=\"black\">[%1] </font>").arg( currentTime );
		editor->insertHtml( timeStamp );
		//
		QString dateStamp   = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm");
		m_history += QString("<font color=\"black\">[%1] </font>").arg(dateStamp);
	}
	//
	editor->insertHtml( CR2BR(WrapHTML(text)) );
	editor->insertPlainText( "\n" );
	m_history += QString("%1\n").arg(text);

	if( moveCursor )
	{
		MoveCursorToEnd();
	}
	//
	ActivateWindow();
}


void ChatWindow::SetConnected( const bool connected )
{
	m_connected = connected;

	QString format( "<font color=\"gray\">%1</font>" );
	QString msg;
	//
	if( m_connected )
	{
		msg = format.arg( tr("Reconnected to server") );
	}
	else
	{
		msg = format.arg( tr("Disconnected from server") );
	}
	//
	AddText( msg );
}
	

void ChatWindow::SetFocus()
{
	setFocus( Qt::OtherFocusReason );
	m_ui->m_textInput->setFocus( Qt::OtherFocusReason );
}


void ChatWindow::RemoveTypingMessage()
{
	if( m_typingMessagePending )
	{
		m_typingMessagePending = false;

		QTextBrowser* editor = m_ui->m_textEdit;
		editor->undo();
		//
		MoveCursorToEnd();
	}
}


void ChatWindow::AddInstantMessage	( const QString& from
									, const bool has_me
									, const QString& message
									)
{
	RemoveTypingMessage();
	//
	QString color = (from == m_myName)? "orange": "blue";
	QString html_msg;
	//	
	if( has_me )
	{
		QTextStream(&html_msg)
			<< "<font color=\""
			<< color
			<< "\">* "
			<< from
			<< "</font>";
	}
	else
	{
		QTextStream(&html_msg)
				<< "<font color=\""
				<< color
				<< "\">"
				<< from
				<< "</font>";
	}
	//
	QTextStream(&html_msg)
		<< " "
		<< message;
	//
	AddText( html_msg );
}


void ChatWindow::AddInstantMessage	( const QString& from
									, const bool has_me
									, const QString& message
									, const QString& translated_msg
									, const QString& detected_lang
									)
{
	LLC::Manager mgr;
	QString lang( mgr.GetLanguage().GetString() );
	const bool is_translated = (lang != detected_lang);
	QString final_message	= is_translated
							? EscapeMessage(translated_msg)
							: EscapeMessage(message);
	QString html_msg;
	//
	if( is_translated )
	{
		QTextStream(&html_msg)
			<< " <font color=\"grey\">"
			<< "[" << detected_lang << "-&gt;" << lang << "] "
			<< final_message
			<< "</font>";
	}
	else
	{
		QTextStream(&html_msg)
			<< " "
			<< final_message;
	}
	//
	if( ShowSourceLanguage() )
	{
		AddInstantMessage( from, has_me, EscapeMessage(message) );
		//
		if( is_translated )
		{
			AddInstantMessage( from, has_me, html_msg );
		}
	}
	else
	{
		AddInstantMessage( from, has_me, html_msg );
	}

	if( m_isGroup )
	{
		// Update source language
		//
		OnCacheSignal( mgr.LookupId( Q2LS(from) ), Q2LS(from), true );
	}
}


void ChatWindow::AddLocalChatMessage ( const QString& from
									 , const QString& verb
									 , const bool has_me
									 , const QString& message
									 )
{
	RemoveTypingMessage();
	//
	QString color = (from == m_myName)? "orange": "blue";
	QString html_msg;
	//	
	if( has_me )
	{
		QTextStream(&html_msg)
			<< "<font color=\""
			<< color
			<< "\">* "
			<< from
			<< "</font>";
	}
	else
	{
		QTextStream(&html_msg)
				<< "<font color=\""
				<< color
				<< "\">"
				<< from
				<< "</font> "
				<< verb;
	}
	//
	QTextStream(&html_msg)
		<< " "
		<< message;
	//
	AddText( html_msg );
}


void ChatWindow::AddLocalChatMessage ( const QString& from
									 , const QString& verb
									 , const bool has_me
									 , const QString& message
									 , const QString& translated_msg
									 , const QString& detected_lang
									 )
{
	LLC::Manager mgr;
	QString lang( mgr.GetLanguage().GetString() );
	const bool is_translated = (lang != detected_lang);
	QString final_message	= is_translated
							? EscapeMessage(translated_msg)
							: EscapeMessage(message);
	QString html_msg;
	//
	if( is_translated )
	{
		QTextStream(&html_msg)
			<< " <font color=\"grey\">"
			<< "[" << detected_lang << "-&gt;" << lang << "] "
			<< final_message
			<< "</font>";
	}
	else
	{
		QTextStream(&html_msg)
			<< " "
			<< final_message;
	}
	//
	if( ShowSourceLanguage() )
	{
		AddLocalChatMessage( from, verb, has_me, EscapeMessage(message) );
		//
		if( is_translated )
		{
			AddLocalChatMessage( from, verb, has_me, html_msg );
		}
	}
	else
	{
		AddLocalChatMessage( from, verb, has_me, html_msg );
	}

	// Update source language
	//
	OnCacheSignal( mgr.LookupId( Q2LS(from) ), Q2LS(from), false );
}


void ChatWindow::AddTypingStatus( const QString& from, bool start )
{
	RemoveTypingMessage();
	
	if( start )
	{
		QTextBrowser* editor = m_ui->m_textEdit;
		QTextCursor cursor = editor->textCursor();
		cursor.beginEditBlock();
		QString msg;
		QTextStream(&msg)
			<< "<font color=\"green\">"
			<< from
			<< (start? tr(" started"): tr(" stopped"))
			<< tr(" typing...")
			<< "</font>";
		AddText( msg, false );
		cursor.endEditBlock();
		cursor.movePosition( QTextCursor::End );
		editor->setTextCursor( cursor );
		//
		m_typingMessagePending = true;
		//
		ActivateWindow();
	}
}


void ChatWindow::AddOnlineStatus( const QString& id, bool online )
{
	LLC::Manager llmgr;
	LLC::String name = llmgr.GetNameFromCache( Q2LS(id) );
	//
	QString msg;
	QTextStream(&msg)
		<< "<font color=\"green\">"
		<< name.GetString()
		<< (online? tr(" is online"): tr(" is offline"))
		<< "</font>";
	AddText( msg );
}

void ChatWindow::AddLogEntry( const QString& message, bool error )
{
	QString msg;
	QTextStream(&msg)
		<< "<font color=\""
		<< (error? "red": "green")
		<< "\">"
		<< message
		<< "</font>";
	AddText( msg );
}


void ChatWindow::UpdateNamesInChat( const QString& agentId, const bool entering )
{
	// Check to see if we are already in the agents list.
	//
	QTreeWidget* avList = m_ui->m_avList;
	QTreeWidgetItemIterator it( avList );
	for( ; *it; ++it )
	{
		if( (*it)->data( 0, Qt::UserRole ).toString() == agentId )
		{
			// Already in list, so just stop here
			//
			return;
		}
	}

	// Save in list
	//
	LLC::Manager llmgr;
	LLC::String	 name = llmgr.GetNameFromCache( Q2LS(agentId) );
	QStringList columns;
	columns
		<< LS2Q(name)
		<< tr("auto", "auto detect language");
	QTreeWidgetItem* item = new QTreeWidgetItem( columns );
	QFont font( item->font(0) );
	font.setBold( false );
	//
	//item->setIcon( 0, GetOnlineIcon( online ) );
	item->setFont( 0, font );
	item->setData( 0, Qt::UserRole, agentId );
	item->setFont( 1, font );
	avList->addTopLevelItem( item );

	// Set sort order
	//
	avList->sortItems( 1, Qt::AscendingOrder  );
	avList->sortItems( 0, Qt::AscendingOrder );
}


void ChatWindow::OnCacheSignal( LLC::String agent_id, LLC::String fullName, bool is_group )
{
	QString			qAgentId	= LS2Q(agent_id);
	//
	// Check to see if we are already in the friends list.
	//
	QTreeWidget* avList = m_ui->m_avList;
	QTreeWidgetItemIterator it( avList );
	for( ; *it; ++it )
	{
		QTreeWidgetItem* item = *it;
		if( item->data( 0, Qt::UserRole ).toString() == qAgentId )
		{
			// Update list entry
			//
			QFont font( item->font(0) );
			font.setBold( false );
			item->setText( 0, LS2Q(fullName));
			item->setFont( 0, font );
			item->setText( 1, GetLanguageName( agent_id ) );
			item->setFont( 1, font );

			// Set sort order
			//
			avList->sortItems( 1, Qt::AscendingOrder  );
			avList->sortItems( 0, Qt::AscendingOrder );
		}
	}
}


void ChatWindow::OnFriendshipAccept( LLC::String agentId )
{
	QString id( agentId.GetString() );
	if( id == m_imId )
	{
		m_ui->m_addButton->setEnabled( false );
		m_ui->m_addButton->hide();
		//
		AddLogEntry( "Friendship accepted!" );
	}
}


void ChatWindow::OnTerminateFriendship( LLC::String agentId )
{
	QString id( agentId.GetString() );
	if( id == m_imId )
	{
		m_ui->m_addButton->setEnabled( true );
		m_ui->m_addButton->show();
		//
		AddLogEntry( "Friendship terminated!" );
	}
}


void ChatWindow::OnLocalAvsPresent()
{
	QTreeWidget* avList = m_ui->m_avList;
	avList->clear();
	//
	LLC::Manager mgr;
	const int av_count = mgr.GetLocalAvatarCount();
	for( int idx = 0; idx < av_count; ++idx )
	{
		LLC::String agent_id = mgr.GetLocalAvatar( idx );
		UpdateNamesInChat( LS2Q(agent_id), true /*entering*/ );
	}
}


void ChatWindow::StartTimer( const bool reset, const bool start )
{
	if( start )
	{
		if( reset ) m_isTyping = TYPING_TIMEOUT;
		m_timerId = startTimer( TIMEOUT_MSECS );
	}
	else
	{
		killTimer( m_timerId );
		if( reset ) m_isTyping = 0;
	}
}


void ChatWindow::timerEvent( QTimerEvent* event )
{
	StopTimer( false /*reset*/ );
	//
	if( --m_isTyping <= 0 )
	{
		LLC::Manager llmgr;
		llmgr.SendTypingSignal( Q2LS(m_imId), m_isGroup, false /*is_typing*/ );
		//
		StopTimer( true /*reset*/ );
	}
	else
	{
		StartTimer( false /*reset*/ );
	}
}
	

void ChatWindow::OnTextInputTextEdited( QString )
{
	if( !m_isLocalChat )
	{
		if( m_ui->m_textInput->text().isEmpty() )
		{
			LLC::Manager llmgr;
			llmgr.SendTypingSignal( Q2LS(m_imId), m_isGroup, false /*is_typing*/ );
			//
			StopTimer( true /*reset*/ );
			//
			m_typingSignal( false );
		}
		else
		{
			if( m_isTyping )
			{
				// Reset the typing count
				//
				m_isTyping = TYPING_TIMEOUT;
			}
			else
			{
				LLC::Manager llmgr;
				llmgr.SendTypingSignal( Q2LS(m_imId), m_isGroup, true /*is_typing*/ );
				//
				StartTimer( true /*reset*/ );
				//
				m_typingSignal( true );
			}
		}
	}
}


// Code lifted directly from the SL viewer, altered to use QString and regular int.
//
QString ChatWindow::StripChannelNumber( const QString &mesg, int* channel )
{
	if( mesg[0] == '/' && mesg[1] == '/' )
	{
		// This is a "repeat channel send"
		*channel = m_lastChannel;
		return mesg.mid( 2, mesg.length() - 2 );
	}
	else if( (mesg[0] == '/') && (!mesg[1].isNull()) && (mesg[1].isDigit()) )
	{
		// This a special "/20" speak on a channel
		int pos = 0;

		// Copy the channel number into a string
		QString channel_string;
		QChar c;
		do
		{
			c = mesg[pos+1];
			if( c.isDigit() )
			{
				channel_string.push_back(c);
			}
			pos++;
		}
		while( !c.isNull() && (pos < 64) && c.isDigit() );
		
		// Move the pointer forward to the first non-whitespace char
		// Check isspace before looping, so we can handle "/33foo"
		// as well as "/33 foo"
		while( !c.isNull() && c.isSpace() )
		{
			c = mesg[pos+1];
			pos++;
		}
		
		m_lastChannel = channel_string.toInt();
		*channel = m_lastChannel;
		return mesg.mid(pos, mesg.length() - pos);
	}
	else
	{
		// This is normal chat.
		*channel = 0;
		return mesg;
	}
}

	
void ChatWindow::OnTextInputReturnPressed()
{
	if( !m_connected )
	{
		AddText( tr("Not connected!") );
		return;
	}

	QString line_to_say = m_ui->m_textInput->text();
	if( line_to_say.length() > 0 )
	{
		LLC::Manager llmgr;
		//
		if( m_isLocalChat )
		{
			int channel = 0;
			line_to_say = StripChannelNumber( line_to_say, &channel );
			llmgr.SendLocalChatMessage( Q2LS(line_to_say), channel );
		}
		else
		{
			llmgr.SendTypingSignal( Q2LS(m_imId), m_isGroup, false /*is_typing*/ );
			llmgr.SendInstantMessage( Q2LS(m_imId), Q2LS(line_to_say), m_isGroup );

			// Group chat automatically echos what we type
			//
			if( !m_isGroup )
			{
				bool has_me = false;
				if( line_to_say.indexOf( "/me" ) == 0 )
				{
					has_me = true;
					line_to_say.remove( 0, 3 );
				}
				//
				AddInstantMessage( m_myName, has_me, EscapeMessage(line_to_say) );			// Echo what we typed into the chat window
			}
		}
		//
		m_ui->m_textInput->clear();
	}
}


void ChatWindow::OnAddButtonClicked()
{
	// TODO: Ask for message to accompany request
	//
	QString msg = tr("Are you sure you want to offer friendship to %1?").arg(m_fullName);
	//
	LLC::Manager mgr;
	MessageDialog::Question	( this
							, tr("Friendship offer")
							, msg
							, boost::bind	( &LLC::Manager::OfferFriendship
											, mgr, Q2LS(m_imId)
											, Q2LS(tr("Will you be my friend?")) )
							);
}


void ChatWindow::OnCloseButtonClicked()
{
	LLC::Manager mgr;
	mgr.SendGroupChatLeaveRequest( Q2LS(m_imId) );
	m_closeSignal( m_imId );
}


void ChatWindow::OnAnchorClicked( QUrl url )
{
	if( url.scheme() == "secondlife" )
	{
		QString msg = tr("Are you sure you want to teleport to %1?").arg(url.toString());
		//
		LLC::Manager mgr;
		MessageDialog::Question	( this
								, tr("Teleport to region")
								, msg
								, boost::bind( &LLC::Manager::TeleportToRegion, mgr, LLC::String(url.toEncoded().data()) )
								); 
	}
	else if( url.isValid() )
	{
		QDesktopServices::openUrl( url );
	}
}


void ChatWindow::OnItemDoubleClicked( QTreeWidgetItem* item, int column )
{
    if( item )
    {
		m_imSignal( item->data( 0, Qt::UserRole ).toString(), item->text( 0 ) );
    }
}


void ChatWindow::OnSendIM()
{
	QTreeWidgetItem* item = m_ui->m_avList->currentItem();
	//
    if( item )
    {
		m_imSignal( item->data( 0, Qt::UserRole ).toString(), item->text( 0 ) );
    }
}


void ChatWindow::OnChangeLanguage()
{
	// TODO fill in
}


// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
