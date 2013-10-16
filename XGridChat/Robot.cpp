/** 
 * \brief Robot implementation
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
#include <string.h>
#include <fcntl.h>
#if (LL_DARWIN or LL_LINUX)
#	include <arpa/inet.h>
#endif

#include <boost/bind.hpp>

// Static message buffer
char Robot::buffer[BUFFLEN];


// Constructor
Robot::Robot(
	const LLC::String &bot_owner,
	const LLC::String &bot_group,
	const LLC::String &bot_prefix,
	const LLC::String &bot_ignore)
  : owner(bot_owner.GetString()),
	group(bot_group.GetString()),
	prefix(bot_prefix.GetString()),
	ignore(bot_ignore.GetString())
{
	// Set up callbacks for inworld communication
	LLC::Manager llmgr;
	llmgr.ConnectImSignal(
		boost::bind(&Robot::imSlot, this, _1, _2, _3, _4, _5, _6));
	llmgr.ConnectGroupChatSignal(
		boost::bind(&Robot::groupChatSlot, this, _1, _2, _3, _4, _5, _6, _7));
	llmgr.ConnectLogoutReplySignal(
		boost::bind(&Robot::logoutReplySlot, this));
	llmgr.ConnectForcedQuitSignal(
		boost::bind(&Robot::forcedQuitSlot, this, _1));
	llmgr.ConnectMessageBoxSignal(
		boost::bind(&Robot::messageBoxSlot, this, _1));

	// By default, we use no socket and talk to nobody
	my_socket = -1;
	for (bool *p = client; p < client + NUMCLIENTS; p++)
		*p = false;

	// We're not online yet
	online = false;
}


// Destructor
Robot::~Robot()
{
	// Close sockets possibly used
	if (my_socket != -1)
		close(my_socket);
}


// Initialize reception of messages from other bots
// Returns 0 if OK
int Robot::prepareListen(
	const LLC::String &listenAddress,
	unsigned int listenPort)
{
	const char *address = listenAddress.GetString();

	// Return if nothing to do
	if (!*address || !listenPort)
		return 0;

	// Set up nonblocking socket
	int server_flags;
	my_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (my_socket == -1)
	{
		printf("Can't open listen socket\n");
		return -1;
	}
	server_flags = fcntl(my_socket, F_GETFL);
	server_flags |= O_NONBLOCK;
	fcntl(my_socket, F_SETFL, server_flags);

	// Prepare socket address
	struct sockaddr_in si_server;
	memset((char *) &si_server, 0, sizeof(struct sockaddr_in));
	si_server.sin_family = AF_INET;
	si_server.sin_port = htons(listenPort);
	if (!inet_aton(address, &si_server.sin_addr))
	{
		printf("Invalid listen address \"%s\"\n", address);
		return -2;
	}

	// Bind to that address
	if (bind(
		my_socket,
		(const struct sockaddr *) &si_server,
		sizeof(struct sockaddr_in)) == -1)
	{
		printf("Can't open listen port\n");
		return -3;
	}

	return 0;
}


// Initialize sending of messages to other bots
// Returns 0 if OK
int Robot::prepareSend(
	int num,
	const LLC::String &sendAddress,
	unsigned int sendPort)
{
	const char *address = sendAddress.GetString();

	// Return if nothing to do
	if (!*address || !sendPort)
		return 0;

	// Prepare socket address
	struct sockaddr_in *p = si_client + num;
	memset((char *) p, 0, sizeof(struct sockaddr_in));
	p->sin_family = AF_INET;
	p->sin_port = htons(sendPort);
	if (!inet_aton(address, &(p->sin_addr)))
	{
		printf("Invalid send address \"%s\"\n", address);
		return -1;
	}

	// Register the client
	client[num] = true;

	return 0;
}


// Blocking inworld authentication routine
// Returns 0 on success
int Robot::authenticate(
	const LLC::String &grid,
	const LLC::String &first,
	const LLC::String &last,
	const LLC::String &password,
	const LLC::String &location)
{
	LLC::Manager llmgr;
	int attempt;

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
	}

	printf("\nTimeout.\n");
	return -2;
}


// Receive IMs from other bots
void Robot::listenPort() const
{
	// Return if nothing to do
	if (my_socket == -1)
		return;

	// Get message from other bot
	int length;
	struct sockaddr_in si_other;
	socklen_t other_length = sizeof(struct sockaddr_in);

	length = recvfrom(
		my_socket,
		buffer,
		BUFFLEN,
		0,
		(struct sockaddr *) &si_other,
		&other_length);

	// Return if nothing (length = -1) or
	// empty message (length = 0) received
	if (length <= 0)
		return;

	// Security: check that the data comes from one of the other robots
	int num;
	for (num = 0; num < NUMCLIENTS; num++)
	{
		if (client[num])
		{
			if (si_client[num].sin_addr.s_addr == si_other.sin_addr.s_addr &&
                si_client[num].sin_port        == si_other.sin_port)
				break;
		}
	}
	if (num == NUMCLIENTS)
	{
		printf("Received data from an unknown source: \"%s\"\n", buffer);
		return;
	}

	// Security: ensure the buffer is NUL-terminated
	if (length >= BUFFLEN)
		length = BUFFLEN - 1;
	buffer[length] = '\0';

	// Analyze data received
	switch (*buffer)
	{
		case 'D':
			dataReceived(num, buffer + 1, length - 1);
			break;
		case 'C':
			commandReceived(num, buffer + 1, length - 1);
			break;
		default:
			printf("Received invalid data: \"%s\"\n", buffer);
	}
}


// IM received
void Robot::imSlot(
	LLC::String id,
	LLC::String from,
	bool has_me,
	LLC::String message,
	LLC::String translated_msg,
	LLC::String detected_lang) const
{
	LLC::Manager llmgr;
	const char *uid = id.GetString(),
	           *msg = message.GetString();

	if (strcmp(uid, owner.GetString() ))
		return;

	if (!strcmp(msg, "Ping"))
	{
		// Ping other robots
		snprintf(buffer, BUFFLEN, "CPing");
		sendMessage();

		// Pong owner
		if (online)
			pongOwner();
	}
	else if (!strcmp(msg, "Quit"))
	{
		llmgr.RequestLogout();
	}
	// else if ... (other bot commands)
}


// Group chat received
void Robot::groupChatSlot(
	LLC::String group_id,
	LLC::String group_name,
	LLC::String from_id,
	bool has_me,
	LLC::String message,
	LLC::String translated_msg,
	LLC::String detected_lang) const
{
	const char	*frm = from_id.GetString(),
				*msg = message.GetString(),
				*pre = prefix.GetString(),
				*ign = ignore.GetString();

	// Prevent retroaction (grid A forwards to grid B
	// which forwards to grid A which...)
	if (!strcmp(frm, ign))
		return;

	// Format message before transmission
	snprintf(
		buffer, BUFFLEN,
		has_me? "D%s%s%s": "D%s%s: %s",
		pre, frm, msg);
	buffer[BUFFLEN - 1] = '\0';

	// Send message to other robots
	sendMessage();
}


// Logout reply received
void Robot::logoutReplySlot()
{
	online = false;
}


// Grid wants us to leave
void Robot::forcedQuitSlot(LLC::String message) const
{
	static int attempt = 0;
	LLC::Manager llmgr;

	printf("Forced quit: \"%s\"\n", message.GetString());
	// As there's no human to control the robot, let's better obey
	if (attempt < 3)
	{
		llmgr.RequestLogout();
		attempt++;
	}
	else
	{
		printf("Three unsuccessful attempts to log out, unclean shutdown\n");
		exit(UNCLEAN_SHUTDOWN);
	}
}


// Grids wants us to display a message
void Robot::messageBoxSlot(LLC::String message) const
{
	printf("Display of message requested: \"%s\"\n", message.GetString());
}


// Send message to other robots
void Robot::sendMessage() const
{
	int len = strlen(buffer) + 1;
	for (int num = 0; num < NUMCLIENTS; num++)
		sendMessage(num, len);
}


// Send message to another robot
void Robot::sendMessage(int other, int length) const
{
	if (my_socket == -1 || !client[other])
		return;

	if (sendto(
		my_socket,
		buffer,
	    length,
		0,
		(struct sockaddr *) (si_client + other),
		sizeof(struct sockaddr_in)) == -1)
			printf("UDP emission error: \"%s\"\n", buffer);
}


// Data has been received from another robot
void Robot::dataReceived(int other, const char *data, int length) const
{
	if (!length)
	{
		printf("Received empty message: \"%s\"\n", data);
		return;
	}

	// Send message inworld
	LLC::Manager llmgr;

	llmgr.SendInstantMessage(
		group,
		LLC::String(data),
		true);
}


// A command has been received from another robot
void Robot::commandReceived(int other, const char *command, int length) const
{
	if (!strncmp(command, "Ping", 4))
	{
		// Don't answer if already offline
		if (online)
			pongRobot(other);
	}
	else if (!strncmp(command, "Pong", 4))
	{
		LLC::Manager llmgr;

		llmgr.SendInstantMessage(owner, LLC::String(command));
	}
	// ... other inter-robot protocol
	else
	{
		printf("Received invalid command: \"%s\"\n", command);
	}
}


// Anwser a ping from another robot
void Robot::pongRobot(int other) const
{
	int length;

	snprintf(buffer, BUFFLEN, "CPong %s(xgridchat version %d.%d.%d)",
		prefix.GetString(),
	    xgridchat_VERSION_MAJOR, xgridchat_VERSION_MINOR, xgridchat_VERSION_PATCH);
	length = strlen(buffer) + 1;

	sendMessage(other, length);
}


// Answer a ping from the owner
void Robot::pongOwner() const
{
	LLC::Manager llmgr;

	snprintf(buffer, BUFFLEN, "Pong %s(xgridchat version %d.%d.%d)",
		prefix.GetString(),
	    xgridchat_VERSION_MAJOR, xgridchat_VERSION_MINOR, xgridchat_VERSION_PATCH);
	buffer[BUFFLEN - 1] = '\0';

	llmgr.SendInstantMessage(owner, LLC::String(buffer));
}


// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
