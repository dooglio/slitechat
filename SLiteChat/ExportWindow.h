/** 
 * \brief Header for ExportWindow, a GUI dialog for selecting conversations to export.
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

#ifndef __EXPORTWINDOW_H__
#define __EXPORTWINDOW_H__

#include <LLChatLib.h>

#include <QDialog>

#include "ui_ExportWindow.h"

class ExportWindow
	: public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(ExportWindow)

public:
	explicit ExportWindow( QWidget *parent = 0 );
	virtual ~ExportWindow();

private:
    Ui_ExportWindow*	m_ui;
	LLC::Connection		m_connection;

	void FillList();
	void OnCacheSignal( LLC::String id, LLC::String fullName, bool is_group );

private slots:
	void OnItemSelectionChanged();
	void OnExportButtonClicked();
};

#endif // __EXPORTWINDOW_H__

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
