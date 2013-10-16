/** 
 * \brief Header for the chatter box stuff
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

#ifndef __CHATTERBOX_H__
#define __CHATTERBOX_H__

// llcommon
//
#include "llsd.h"
#include "stdtypes.h"

// llmessage
//
#include "llhttpclient.h"
#include "llhttpnode.h"


class LLViewerChatterBoxSessionStartReply : public LLHTTPNode
{
public:
	virtual void describe(Description& desc) const;
	virtual void post(ResponsePtr response,
					  const LLSD& context,
					  const LLSD& input) const;
};


class LLViewerChatterBoxSessionEventReply : public LLHTTPNode
{
public:
	virtual void describe(Description& desc) const;
	virtual void post(ResponsePtr response,
					  const LLSD& context,
					  const LLSD& input) const;
};


class LLViewerForceCloseChatterBoxSession: public LLHTTPNode
{
public:
	virtual void post(ResponsePtr response,
					  const LLSD& context,
					  const LLSD& input) const;
};


class LLViewerChatterBoxSessionAgentListUpdates : public LLHTTPNode
{
public:
	virtual void post(
		ResponsePtr responder,
		const LLSD& context,
		const LLSD& input) const;
};


class LLViewerChatterBoxSessionUpdate : public LLHTTPNode
{
public:
	virtual void post(
		ResponsePtr responder,
		const LLSD& context,
		const LLSD& input) const;
};



class LLViewerChatterBoxInvitationAcceptResponder :
	public LLHTTPClient::Responder
{
public:
	LLViewerChatterBoxInvitationAcceptResponder( const LLUUID& session_id );
	void result(const LLSD& content);
	void error(U32 statusNum, const std::string& reason);

private:
	LLUUID mSessionID;
};


class LLViewerChatterBoxInvitation : public LLHTTPNode
{
public:
	virtual void post(
		ResponsePtr response,
		const LLSD& context,
		const LLSD& input) const;
};

#endif // __CHATTERBOX_H__

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
