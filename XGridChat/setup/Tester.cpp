/** 
 * \brief Online determination of owner's and group's UUIDs
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

#include "Tester.h"
#include "GridTab.h"

#include <QApplication>

#include <stdio.h>

#include <boost/bind.hpp>

// Constructor
Tester::Tester(GridTab *gridTab)
	: m_gridTab(gridTab),
	  online(false), stop_attempt(0)
{
}


// Blocking inworld authentication routine
// Returns 0 on success
int Tester::authenticate(
	const LLC::String &grid,
	const LLC::String &first,
	const LLC::String &last,
	const LLC::String &password,
	const LLC::String &location)
{
	LLC::Manager llmgr;
	int attempt;

	QApplication::processEvents();
	llmgr.Authenticate(grid, first, last, password, location);

	for (attempt = 0; attempt < 32; attempt++)
	{
		putchar('.'); fflush(stdout); usleep(500000L);
		try
		{
			if (llmgr.CheckForResponse())
			{
				online = true;
				return 0;
			}
		}
		catch (LLC::AuthException &x)
		{
			printf("\n%s\n", x.GetMessageText() );
			return -1;
		}
		QApplication::processEvents();
	}

	printf("\nTimeout.\n");
	return -2;
}


// Stop test in progress
void Tester::stop()
{
	if (online)
	{
		LLC::Manager llmgr;

		printf("Stopping the test...\n");

		if (stop_attempt < 3)
		{
			llmgr.RequestLogout();
			stop_attempt++;
		}
		else
		{
			printf("Three unsuccessful attempts to log out, unclean shutdown\n");
			online = false;
		}
	}
}


// Run test
// Pump messages until desired information has come
void Tester::runTest()
{
	LLC::Manager llmgr;

	llmgr.ConnectCacheSignal(
		boost::bind(&Tester::cacheSlot, this, _1, _2, _3));
	llmgr.ConnectGroupCacheSignal(
		boost::bind(&Tester::groupCacheSlot, this, _1, _2));
	llmgr.ConnectForcedQuitSignal(
		boost::bind(&Tester::forcedQuitSlot, this, _1));
	llmgr.ConnectLogoutReplySignal(
		boost::bind(&Tester::logoutReplySlot, this));

	llmgr.RequestBuddyList();
	llmgr.AnnounceInSim();

	while (online)
	{
#define SLEEP_VALUE 10000L
#ifdef LL_WINDOWS
		::Sleep(SLEEP_VALUE);
#else
		usleep(SLEEP_VALUE);
#endif
		QApplication::processEvents();
		llmgr.PumpMessages();
	}

	printf("Leaving...\n");
}


// Determine the owner id
void Tester::cacheSlot(LLC::String id, LLC::String full_name, bool is_group)
{
	if (is_group) return;

	if (m_gridTab->gotOwner(id.GetString(), full_name.GetString()))
	{
		if (m_gridTab->groupIDKnown())
		{
			LLC::Manager llmgr;

			llmgr.RequestLogout();
			stop_attempt++;
		}
	}
}


// Handle group cache signal
void Tester::groupCacheSlot(LLC::String id, LLC::String group_name)
{
	if (m_gridTab->gotGroup(id.GetString(), group_name.GetString()))
	{
		if (m_gridTab->ownerIDKnown())
		{
			LLC::Manager llmgr;

			llmgr.RequestLogout();
			stop_attempt++;
		}
	}
}


// Grid wants us to leave
void Tester::forcedQuitSlot(LLC::String message)
{
	LLC::Manager llmgr;

	printf("Forced quit: \"%s\"\n", message.GetString());

	if (stop_attempt < 3)
	{
		llmgr.RequestLogout();
		stop_attempt++;
	}
	else
	{
		printf("Three unsuccessful attempts to log out, unclean shutdown\n");
		online = false;
	}
}


// Logout reply received
void Tester::logoutReplySlot()
{
	online = false;
}


// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
