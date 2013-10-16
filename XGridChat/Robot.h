/** 
 * \brief Robot declaration
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

#ifndef ROBOT_H
#define ROBOT_H

#include "LLChatLib.h"

#include "config.h"

#if (LL_DARWIN or LL_LINUX)
#	include <netinet/in.h>
#endif
#if defined(HAVE_WINSOCK_H)
#	include <winsock.h>
#endif
#if defined(HAVE_WINSOCK2_H)
#	include <winsock2.h>
#endif

// Maximum length for a message
#define BUFFLEN 1024

// Maximum number of other robots
#define NUMCLIENTS 4

class Robot
{
public:
	Robot(
		const LLC::String &bot_owner,
		const LLC::String &bot_group,
		const LLC::String &bot_prefix,
		const LLC::String &bot_ignore);
	~Robot();

	inline bool isOnline() const { return online; }

	int prepareListen(
		const LLC::String &listenAddress,
		unsigned int listenPort);
	int prepareSend(
		int num,
		const LLC::String &sendAddress,
		unsigned int sendPort);
	int authenticate(
		const LLC::String &grid,
		const LLC::String &first,
		const LLC::String &last,
		const LLC::String &password,
		const LLC::String &location);

	void listenPort() const;

	void imSlot(
		LLC::String id,
		LLC::String from,
		bool has_me,
		LLC::String message,
		LLC::String translated_msg,
		LLC::String detected_lang) const;
        void groupChatSlot(
		LLC::String group_id,
		LLC::String group_name,
		LLC::String from_id,
		bool has_me,
		LLC::String message,
		LLC::String translated_msg,
		LLC::String detected_lang) const;
	void logoutReplySlot();
	void forcedQuitSlot(
		LLC::String message) const;
	void messageBoxSlot(
		LLC::String message) const;

private:
	void sendMessage() const;
	void sendMessage(int other, int length) const;
	void dataReceived(int other, const char *data, int length) const;
	void commandReceived(int other, const char *command, int length) const;
	void pongRobot(int other) const;
	void pongOwner() const;

	static char buffer[BUFFLEN];

	LLC::String owner, group, prefix, ignore;
	int my_socket;
	bool client[NUMCLIENTS];
	struct sockaddr_in si_client[NUMCLIENTS];
	bool online;
};

#endif

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
