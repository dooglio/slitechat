/** 
 * \brief Main function
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

#if defined(WIN32)
#	pragma warning( disable:4996 )
#	include <windows.h>
#endif

#include "MainWindow.h"

#include <cstdio>
#include <iostream>
#include <sstream>

#ifdef LL_WINDOWS
#	include <conio.h>
#	include <fcntl.h>
#	include <io.h>
#endif

// Qt4
//
#include <QApplication>
#include <QSettings>

namespace
{

#if LL_WINDOWS

// This is useful for debugging purposes to create out own entry point.
//
void OpenWin32Console()
{ 
#if defined( SHOW_CONSOLE_WINDOW )
	// Open up a console window under win32
	//
	int hCrt;
	FILE *hf;
	//
	AllocConsole();
	hCrt = _open_osfhandle(
						reinterpret_cast<intptr_t>( GetStdHandle(STD_OUTPUT_HANDLE) ),
						_O_TEXT
				);
	hf = _fdopen( hCrt, "w" );
	*stdout = *hf;
	setvbuf( stdout, NULL, _IONBF, 0 ); 
	//
#ifdef _MSC_VER
#if defined(_DEBUG) || defined(DEBUG)
	// Enable the code below to stop the debugger on an allocation number,
	// or set a break point on line 355 of dbgheap.c (vc7.1).
	//
	//_CrtSetBreakAlloc( 41291 );
#endif // DEBUG
#endif // _MSC_VER

#endif // SHOW_CONSOLE_WINDOW
}

#endif // LL_WINDOWS


}
// end namespace


int main( int argc, char *argv[] )
{
#if LL_WINDOWS
	OpenWin32Console();
#endif

	// The main application object
	//
	QApplication a(argc, argv);

	// Set-up core application information
	//
	QCoreApplication::setOrganizationName  ( "Made to Order Software Corporation" );
	QCoreApplication::setOrganizationDomain( "slitechat.org" );
	QCoreApplication::setApplicationName   ( "SLiteChat" );

	// Create and show main window
	//
	MainWindow w;
	w.show();

	// Start the application
	//
	const int retval = a.exec();

	// Done, let's shutdown LLChat
	//
	LLC::Manager llmgr;
	llmgr.Shutdown();

	// Return the value from the app
	//
	return retval;
}


// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
