/** 
 * \brief Methods for the SearchWindow class.
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

#include "SearchWindow.h"
#include "LLChatLib.h"

#include <boost/bind.hpp>

#define FAKE_ENTRY -1


SearchWindow::SearchWindow( QWidget* parent )
	: QDialog( parent )
    , m_ui(new Ui_SearchWindow)
{ 
    m_ui->setupUi(this);

	// Information must be added first
	//
	m_ui->m_keywordEntry->setFocus( Qt::OtherFocusReason );
	
	LLC::Manager mgr;
	m_searchSignal = mgr.ConnectSearchResultSignal( boost::bind( &SearchWindow::OnSearchResults, this, _1, _2 ) );
}


SearchWindow::~SearchWindow()
{
	delete m_ui;
	//
	m_searchSignal.disconnect();
}


void SearchWindow::SetSearchKeyword( const QString& keyword )
{
	m_keyword = keyword;
	//
	if( !keyword.isEmpty() )
	{
		m_ui->m_keywordEntry->setText( keyword );
	}
}
	

void SearchWindow::OnSearchResults( LLC::String agentId, LLC::String fullName )
{
	m_agentIds.push_back( agentId.GetString() );
	m_ui->m_searchResults->addItem( fullName.GetString() );
}


void SearchWindow::OnSearchButtonClicked()
{
	if( m_ui->m_keywordEntry->text().isEmpty() )
	{
		// We should never get this
		return;
	}
	
	m_ui->m_searchResults->clear();
	m_agentIds.clear();
	QString keywords = m_ui->m_keywordEntry->text();
	
	LLC::Manager mgr;
	mgr.SearchPeople( keywords.toAscii().data() );
}


void SearchWindow::OnAddButtonClicked()
{
	RetrieveSelection();
	done( ADD );
}


void SearchWindow::OnSendImButtonClicked()
{
	RetrieveSelection();
	done( SEND_IM );
}


void SearchWindow::OnKeywordEntryTextChanged( QString text )
{
	const bool enable_button = !text.isEmpty();
	m_ui->m_searchButton->setEnabled( enable_button );
	
	if( enable_button )
	{
		m_keyword = text;
	}
	else
	{
		m_keyword.clear();
	}
}


void SearchWindow::OnSearchResultsItemClicked( QListWidgetItem* item )
{
	bool enable_button = true;
	//
	if( enable_button )
	{
		// Eliminate the case of the phony search entry inviting the user to enter a search term
		//
		enable_button = ( item->data( Qt::UserRole ).toInt() != FAKE_ENTRY );
	}
	
	m_ui->m_addButton->setEnabled( enable_button );
	m_ui->m_sendImButton->setEnabled( enable_button );
}


void SearchWindow::OnSearchResultsItemDoubleClicked( QListWidgetItem* )
{
	RetrieveSelection();
	done( SEND_IM );
}


void SearchWindow::RetrieveSelection()
{
	QListWidgetItem* item = m_ui->m_searchResults->currentItem();
	if( !item )
	{
		// Shouldn't ever happen!
		return;
	}
	const size_t agentIdIdx = (size_t) item->data( Qt::UserRole ).toInt();
	//
	m_selectedId	   = m_agentIds[agentIdIdx];
	m_selectedFullName = item->text();
}

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
