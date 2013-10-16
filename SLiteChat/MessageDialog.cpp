/** 
 * \brief MessageDialog methods
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

#include "MessageDialog.h"
#include "LLChatLib.h"
#include "Utility.h"
#include "Common.h"
#include "Config.h"

#include <boost/signals.hpp>
#include <boost/bind.hpp>

#include <iostream>
#include <algorithm>

#include <QTextStream>
#include <QUrl>

MessageDialog::MessageVector		MessageDialog::m_dialogs;

MessageDialog::MessageDialog( QWidget* parent )
	: QDialog( parent )
{
    m_ui = new Ui_MessageDialog;
	m_ui->setupUi( this );
	//
	SetButtons( QDialogButtonBox::Ok );
}


MessageDialog::MessageDialog( QWidget* parent, const QString& label, const QString& caption )
	: QDialog( parent )
{
    m_ui = new Ui_MessageDialog;
	m_ui->setupUi( this );
	//
	SetLabel( label );
	SetButtons( QDialogButtonBox::Ok );

	setWindowTitle( caption );
}


MessageDialog::MessageDialog( QWidget* parent, const QString& label, const QString& caption, const Buttons& buttons )
	: QDialog( parent )
{
    m_ui = new Ui_MessageDialog;
	m_ui->setupUi( this );
	//
	SetLabel( label );
	SetButtons( buttons );

	setWindowTitle( caption );
}


MessageDialog::~MessageDialog()
{
	delete m_ui;
}


void MessageDialog::SetTimer()
{
	if( Common::IsOnline() )
	{
		m_timerId = startTimer( MESSAGE_TIMEOUT_MSECS );
	}
}


QString MessageDialog::GetLabel() const
{
	return m_ui->m_textBrowser->toPlainText();
}


void MessageDialog::SetLabel( const QString& label )
{
	m_ui->m_textBrowser->setPlainText( label );
}


QString MessageDialog::GetHtmlLabel() const
{
	return m_ui->m_textBrowser->toHtml();
}


void MessageDialog::SetHtmlLabel( const QString& label )
{
	m_ui->m_textBrowser->setHtml( label );
}


MessageDialog::Buttons	MessageDialog::GetButtons() const
{
	return m_ui->m_buttonBox->standardButtons();
}


void MessageDialog::SetButtons( const Buttons& btns )
{
	m_ui->m_buttonBox->setStandardButtons( btns );
}


int MessageDialog::Show( const bool wait )
{
	if( wait )
	{
		SetTimer();
		return exec();
	}
	else
	{
		show();
	}

	return 0;
}


void MessageDialog::AddDialog( MessageDialogPtr dialog )
{
	m_dialogs.push_back( dialog );
}


void MessageDialog::PurgeHiddenDialogs()
{
	MessageVector::iterator iter;
	do
	{
		iter = std::find_if( m_dialogs.begin(), m_dialogs.end(),
				boost::bind( &MessageDialog::isHidden, _1 ) );
		if( iter != m_dialogs.end() )
		{
			m_dialogs.erase( iter );
		}
		//
		iter = std::find_if( m_dialogs.begin(), m_dialogs.end(),
				boost::bind( &MessageDialog::isHidden, _1 ) );
	}
	while( iter != m_dialogs.end() );
}


void MessageDialog::PurgeAllDialogs()
{
	m_dialogs.clear();
}


bool MessageDialog::Question( QWidget* parent
		, const QString& caption
		, const QString& message
		)
{
	MessageDialogPtr dlg( new MessageDialog( parent, message, caption, QDialogButtonBox::Yes | QDialogButtonBox::No ) );
   	return dlg->Show( true ) == QDialog::Accepted;
}


void MessageDialog::Question( QWidget* parent
		, const QString& caption
		, const QString& message
		, const VoidSignal::slot_type& accept_slot
		, const VoidSignal::slot_type& reject_slot
		)
{
	MessageDialogPtr dlg( new MessageDialog( parent, message, caption, QDialogButtonBox::Yes | QDialogButtonBox::No ) );
	dlg->ConnectAcceptedSignal( accept_slot );
	dlg->ConnectRejectedSignal( reject_slot );
	dlg->show();
	m_dialogs.push_back( dlg );
}


void MessageDialog::Question( QWidget* parent
		, const QString& caption
		, const QString& message
		, const VoidSignal::slot_type& accept_slot
		)
{
	MessageDialogPtr dlg( new MessageDialog( parent, message, caption, QDialogButtonBox::Yes | QDialogButtonBox::No ) );
	dlg->ConnectAcceptedSignal( accept_slot );
	dlg->show();
	m_dialogs.push_back( dlg );
}


void MessageDialog::Information( QWidget* parent
		, const QString& caption
		, const QString& message
		, bool wait
		)
{
	MessageDialogPtr dlg( new MessageDialog( parent, message, caption, QDialogButtonBox::Ok ) );
	dlg->Show( wait );
	if( !wait )
	{
		m_dialogs.push_back( dlg );
	}
}


void MessageDialog::Warning( QWidget* parent
		, const QString& caption
		, const QString& message
		, bool wait
		)
{
	MessageDialogPtr dlg( new MessageDialog( parent, message, caption, QDialogButtonBox::Ok ) );
	//
	dlg->Show( wait );
	if( !wait )
	{
		m_dialogs.push_back( dlg );
	}
}


void MessageDialog::Critical( QWidget* parent
		, const QString& caption
		, const QString& message
		, bool wait
		)
{
	MessageDialogPtr dlg( new MessageDialog( parent, message, caption, QDialogButtonBox::Ok ) );
	//
	dlg->Show( wait );
	if( !wait )
	{
		m_dialogs.push_back( dlg );
	}
}


void MessageDialog::timerEvent( QTimerEvent* event )
{
	killTimer( m_timerId );

	// Don't keep pumping messages if offline!
	//
	LLC::Manager llmgr;
	//
	if( llmgr.IsOnline() )
	{
		llmgr.PumpMessages();

		m_timerId = startTimer( MESSAGE_TIMEOUT_MSECS );
	}
}


void MessageDialog::Accepted()
{
	accept();
	m_acceptedSignal();
}


void MessageDialog::Rejected()
{
	reject();
	m_rejectedSignal();
}


// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
