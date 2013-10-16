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

#include "Robot.h"
#include "version.h"
#include "errors.h"

#include <stdio.h>
#if defined(HAVE_UNISTD_H)
#	include <unistd.h>
#endif
#include <signal.h>
#if defined(HAVE_WINDOWS_H)
#include <windows.h>
#endif

// Declare the used settings
void declare_settings()
{
	LLC::Manager llmgr;

	llmgr.DeclareString(
		LLC::String("BotOwner"),
		LLC::String("00000000-0000-0000-0000-000000000000"),
		LLC::String("UUID of the avatar owning the bot"));
	llmgr.DeclareString(
		LLC::String("BotGrid"),
		LLC::String("https://login.agni.lindenlab.com/cgi-bin/login.cgi"),
		LLC::String("Login URL of the grid"));
	llmgr.DeclareString(
		LLC::String("BotFirst"),
		LLC::String(),
		LLC::String("First name of the bot"));
	llmgr.DeclareString(
		LLC::String("BotLast"),
		LLC::String(),
		LLC::String("Last name of the bot"));
	llmgr.DeclareString(
		LLC::String("BotPassword"),
		LLC::String(),
		LLC::String("Munged string for bot password"));
	llmgr.DeclareString(
		LLC::String("BotLocation"),
		LLC::String("last"),
		LLC::String("SLURL of initial inworld location of bot"));
	llmgr.DeclareString(
		LLC::String("BotGroup"),
		LLC::String("00000000-0000-0000-0000-000000000000"),
		LLC::String("UUID of the inworld chat group that the bot joins"));
	llmgr.DeclareString(
		LLC::String("BotPrefix"),
		LLC::String(),
		LLC::String("Text prepended before each message"));
	llmgr.DeclareString(
		LLC::String("BotIgnore"),
		LLC::String(),
		LLC::String("Full name of the bot"));

	llmgr.DeclareString(
		LLC::String("ListenAddress"),
		LLC::String(),
		LLC::String("Server IP address"));
	llmgr.DeclareUInt(
		LLC::String("ListenPort"),
		0,
		LLC::String("Server UDP port"));
	char name[40], comment[40];
	for (int num = 0; num < NUMCLIENTS; num++)
	{
		sprintf(name, "SendAddress%d", num + 1);
		sprintf(comment, "Client IP address %d", num + 1);
		llmgr.DeclareString(LLC::String(name), LLC::String(), LLC::String(comment));
		sprintf(name, "SendPort%d", num + 1);
		sprintf(comment, "Client UDP port %d", num + 1);
		llmgr.DeclareUInt(LLC::String(name), 0, LLC::String(comment));
	}
}


// Initialize networking with other bots
// Returns 0 if okay
int initialize_network(Robot *bot)
{
	LLC::Manager llmgr;

	if (bot->prepareListen(
		llmgr.GetString("ListenAddress"),
		llmgr.GetUInt("ListenPort") ))
			return -1;

	char address[40], port[40];
	for (int num = 0; num < NUMCLIENTS; num++)
	{
		sprintf(address, "SendAddress%d", num + 1);
		sprintf(port, "SendPort%d", num + 1);
		if (bot->prepareSend(
			num,
			llmgr.GetString(address),
			llmgr.GetUInt(port) ))
			return -2;
	}

	return 0;
}


// Logout signal handler
void signal_logout(int sig)
{
	LLC::Manager llmgr;

	signal(sig, SIG_DFL);
	printf("Logout signal catched. Leaving...\n");
	llmgr.RequestLogout();
}


// Main program
int main(int argc, char *argv[])
{
	LLC::Manager llmgr;
	printf("Cross-grid group IM bridge, version %d.%d.%d.\n",
		xgridchat_VERSION_MAJOR, xgridchat_VERSION_MINOR, xgridchat_VERSION_PATCH);
	printf("Contributed by the owner of avatar Catherine Pfeffer.\n");
	printf("Copyright (C) 2009-2010, R. Douglas Barbieri.\n");
	printf("Licensed under GNU General Public License, version 2.0.\n\n");
	if (argc != 2)
	{
		printf("Syntax: xgridchat <filename>\n");
		printf("where <filename> is the name of a robots settings file.\n");
		return SYNTAX_ERROR;
	}

	declare_settings();
	llmgr.SetTranslateMessages(false);
	llmgr.StartMessagingSystem("XGridChat", argv[1]);

	Robot bot(
		llmgr.GetString("BotOwner"),
		llmgr.GetString("BotGroup"),
		llmgr.GetString("BotPrefix"),
		llmgr.GetString("BotIgnore") );

	if (initialize_network(&bot))
		return UDP_ERROR;

	if (bot.authenticate(
		llmgr.GetString("BotGrid"),
		llmgr.GetString("BotFirst"),
		llmgr.GetString("BotLast"),
		llmgr.GetString("BotPassword"),
		llmgr.GetString("BotLocation") ))
			return LOGIN_ERROR;

	llmgr.AnnounceInSim();
	llmgr.SendGroupChatStartRequest(llmgr.GetString("BotGroup"));

	signal(SIGINT, signal_logout);

	while (bot.isOnline())
	{
#define SLEEP_VALUE 10000L
#ifdef LL_WINDOWS
		::Sleep(SLEEP_VALUE);
#else
		usleep(SLEEP_VALUE);
#endif
		bot.listenPort();
		llmgr.PumpMessages();
	}

	printf("Leaving...\n");
	llmgr.SendGroupChatLeaveRequest(llmgr.GetString("BotGroup"));
	llmgr.Shutdown();

	return 0;
}


// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
