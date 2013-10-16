/** 
 * \brief Header for SearchWindow, a GUI dialog searching people in the SL directory.
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

#ifndef __SEARCHWINDOW_H__
#define __SEARCHWINDOW_H__

#include <vector>
#include "LLChatLib.h"

#include <QDialog>
#include "ui_SearchWindow.h"

class SearchWindow
	: public QDialog
{
    Q_OBJECT
	Q_DISABLE_COPY(SearchWindow)

public:
	explicit SearchWindow( QWidget* parent );
	virtual ~SearchWindow();

	enum ReturnValues
	{
		  ADD = 1000, SEND_IM
	};

	QString	GetSelectedId()	   const { return m_selectedId; }
	QString	GetSelectedName()  const { return m_selectedFullName; }
	QString	GetSearchKeyword() const { return m_keyword; }
	//
	void	SetSearchKeyword( const QString& keyword );

private:
    Ui_SearchWindow*	m_ui;
	QString				m_selectedId;
	QString				m_selectedFullName;
	QString				m_keyword;
	LLC::Connection		m_searchSignal;
	
	typedef std::vector<QString> StringList;
	StringList m_agentIds;
	
	// LLC::Manager events
	//
	void OnSearchResults( LLC::String agentId, LLC::String fullName );
	
	// Other methods
	//
	void RetrieveSelection();

private slots:
	void OnSearchButtonClicked();
	void OnAddButtonClicked();
	void OnSendImButtonClicked();
	void OnKeywordEntryTextChanged( QString );
	void OnSearchResultsItemClicked( QListWidgetItem* );
	void OnSearchResultsItemDoubleClicked( QListWidgetItem* );
};

#endif // __SEARCHWINDOW_H__

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
