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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QCloseEvent;
class QTabWidget;

class MainWindow : public QMainWindow
{
	Q_OBJECT
	Q_DISABLE_COPY(MainWindow)

public:
	MainWindow();

public slots:
	void onAboutToQuit();

protected:
	virtual void closeEvent(QCloseEvent *event);

private:
	QTabWidget *m_tabWidget;
};


#endif

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
