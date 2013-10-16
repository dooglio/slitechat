/** 
 * \brief Methods for the ExportWindow class.
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

#include "LLChatLib.h"
#include "ExportWindow.h"

#include <iostream>

#include <QDir>
#include <QFileDialog>
#include <QPushButton>
#include <QTextBrowser>

#include "Common.h"
#include "Utility.h"

#include <boost/bind.hpp>

ExportWindow::ExportWindow( QWidget* parent )
	: QDialog(parent)
    , m_ui(new Ui_ExportWindow)
{
    m_ui->setupUi(this);
	//
	LLC::Manager llmgr;
	m_connection = llmgr.ConnectCacheSignal( boost::bind( &ExportWindow::OnCacheSignal, this, _1, _2, _3 ) );
	//
	FillList();
}


ExportWindow::~ExportWindow()
{
	m_connection.disconnect();
	//
    delete m_ui;
}


void ExportWindow::FillList()
{
	LLC::Manager llmgr;
	QString userPath = LS2Q(llmgr.GetUserSettingsPath());
	//
	QDir dir( userPath );
	QStringList filters;
	filters << "*.txt";
	dir.setNameFilters( filters );

	for( uint i = 0; i < dir.count(); ++i )
	{
		QString agentId = dir[i];
		agentId.remove(".txt");
		//
		QString name = LS2Q( llmgr.GetNameFromCache( Q2LS(agentId) ) );
		//
		std::cerr << "id: " << agentId.toUtf8().data() << ", name=" << name.toUtf8().data() << std::endl;
		//
		QTreeWidgetItem* item = new QTreeWidgetItem( m_ui->m_nameList );
		item->setText( 0, name );
		item->setData( 0, Qt::UserRole, agentId );
	}
}


void ExportWindow::OnItemSelectionChanged ()
{
	m_ui->m_exportButton->setEnabled( m_ui->m_nameList->selectedItems().size() > 0 );
}


void ExportWindow::OnExportButtonClicked()
{
	QString folder = QFileDialog::getExistingDirectory( this, tr("Specify directory to store conversations") );
	//
	if( !folder.isEmpty() )
	{
		LLC::Manager llmgr;
		QString userPath = LS2Q(llmgr.GetUserSettingsPath());
		//
		QTextBrowser browser;
		//
		typedef QList<QTreeWidgetItem*> ItemList;
		ItemList					list = m_ui->m_nameList->selectedItems();
		ItemList::iterator			iter = list.begin();
		const ItemList::iterator	end  = list.end();
		//
		for( ; iter != end; ++iter )
		{
			QTreeWidgetItem*	item	 = *iter;
			//
			QString				in_filename = QString("%1/%2.txt").arg(userPath).arg(item->data( 0, Qt::UserRole ).toString());
			QFile				inputFile( in_filename );
			QByteArray			inputText = inputFile.readAll();
			browser.setHtml( QString::fromUtf8( inputText.constData() ) );
			//
			QString				out_filename = QString("%1/%2.txt").arg(folder).arg( item->text( 0 ) );
			QFile				outputFile( out_filename );
			//
			outputFile.write( browser.toPlainText().toUtf8() );
		}
	}
}


void ExportWindow::OnCacheSignal( LLC::String id, LLC::String fullName, bool is_group )
{
	QString cacheId( LS2Q(id) );
	LLC::Manager llmgr;
	//
	const int count = m_ui->m_nameList->topLevelItemCount();
	//
	for( int i = 0; i < count; ++i )
	{
		QTreeWidgetItem*	item = m_ui->m_nameList->topLevelItem( i );
		QString				id   = item->data( 0, Qt::UserRole ).toString();
		//
		QString name = LS2Q( fullName );
		//
		if( name == "(???) (???)" ) // TODO: make this work on non-SL grids
		{
			name = LS2Q( llmgr.LookupGroupName( Q2LS(id) ) );
		}
		//
		item->setText( 0, name );
	}
}


// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
