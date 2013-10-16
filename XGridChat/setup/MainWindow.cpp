/** 
 * \brief Main window for the XGridChat setup program
 *
 * Copyright (c) 2010 by R. Douglas Barbieri
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

#include <QtGui>

#include "MainWindow.h"
#include "GridTab.h"

extern void aboutToQuit(); // in main.cpp


MainWindow::MainWindow()
	: QMainWindow()
{
	m_tabWidget = new QTabWidget;

	m_tabWidget->addTab(new GridTab(0), "Grid 1");
	m_tabWidget->addTab(new GridTab(1), "Grid 2");
	m_tabWidget->setTabsClosable(true);

	setCentralWidget(m_tabWidget);
	setMinimumWidth(440);
}


void MainWindow::closeEvent(QCloseEvent *event)
{
	int count = m_tabWidget->count(),
	    index = 0;

	while (index < count)
	{
		GridTab *gridTab = (GridTab *)
			m_tabWidget->widget(index++);

		if (gridTab->testInProgress())
		{
			QMessageBox msgBox;

			msgBox.setText("A test is in progress. Stop it first.");
			msgBox.exec();

			event->ignore();

			return;
		}
	}
	event->accept();
}


void MainWindow::onAboutToQuit()
{
	aboutToQuit();
}

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
