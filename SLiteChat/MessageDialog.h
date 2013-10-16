/** 
 * \brief Header for MessageDialog, which handles a local chat or IM session.
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
#ifndef MESSAGEDIALOG_H__
#define MESSAGEDIALOG_H__

#include "LLChatLib.h"

#include <boost/signals.hpp>

#include <QDialog>
#include <QDialogButtonBox>

#include "ui_MessageDialog.h"

class MessageDialog
	: public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(MessageDialog)

public:
	typedef QDialogButtonBox::StandardButtons Buttons;

	explicit MessageDialog( QWidget* parent );
	explicit MessageDialog( QWidget* parent, const QString& label, const QString& caption );
	explicit MessageDialog( QWidget* parent, const QString& label, const QString& caption, const Buttons& buttons );
	virtual ~MessageDialog();

	QString GetLabel() const;
	void	SetLabel( const QString& label );

	QString	GetHtmlLabel() const;
	void	SetHtmlLabel( const QString& label );

	Buttons	GetButtons() const;
	void	SetButtons( const Buttons& btns );

	int		Show( const bool wait = true );

	typedef boost::signal<void ()> VoidSignal;
	LLC::Connection ConnectAcceptedSignal( const VoidSignal::slot_type& slot ) { return m_acceptedSignal.connect(slot); }
	LLC::Connection ConnectRejectedSignal( const VoidSignal::slot_type& slot ) { return m_rejectedSignal.connect(slot); }

	typedef boost::shared_ptr<MessageDialog> MessageDialogPtr;
	static void AddDialog( const MessageDialogPtr dialog );
	static void PurgeHiddenDialogs();
	static void PurgeAllDialogs();

	static bool Question( QWidget* parent
			, const QString& caption
			, const QString& message
			);

	static void Question( QWidget* parent
			, const QString& caption
			, const QString& message
			, const VoidSignal::slot_type& accept_slot
			, const VoidSignal::slot_type& reject_slot
			);

	static void Question( QWidget* parent
			, const QString& caption
			, const QString& message
			, const VoidSignal::slot_type& accept_slot
			);

	static void Information( QWidget* parent
			, const QString& caption
			, const QString& message
			, bool wait = true
			);
	
	static void Warning( QWidget* parent
			, const QString& caption
			, const QString& message
			, bool wait = true
			);

	static void Critical( QWidget* parent
			, const QString& caption
			, const QString& message
			, bool wait = true
			);

protected:
	void				timerEvent( QTimerEvent* event );


private:
	Ui_MessageDialog*	m_ui;
	int					m_timerId;
	//
	VoidSignal			m_acceptedSignal;
	VoidSignal			m_rejectedSignal;

	typedef std::vector<MessageDialogPtr> MessageVector;
	static MessageVector		m_dialogs;

	void				SetTimer();

private slots:

	// GUI events
	//
	void				Accepted();
	void				Rejected();
};

#endif //MESSAGEDIALOG_H__

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
