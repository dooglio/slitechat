/** 
 * \brief XGridChat GUI setup - main program
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

#include "LLChatLib.h"
#include <QApplication>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    LLC::Manager llmgr;

	QApplication app(argc, argv);
	MainWindow mainWin;

	llmgr.SetTranslateMessages(false);
	llmgr.StartMessagingSystem("XGridChat", "settings.xml");

	QObject::connect(&app, SIGNAL(aboutToQuit()),
					 &mainWin, SLOT(onAboutToQuit()));
	mainWin.show();
	return app.exec();
}


void aboutToQuit()
{
    LLC::Manager llmgr;

	llmgr.Shutdown();
}


// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
