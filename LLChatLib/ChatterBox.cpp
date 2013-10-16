/** 
 * \brief Methods for the chatter box classes
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

#include "ChatterBox.h"
#include "ManagerImpl.h"



/*============================== LLViewerChatterBoxSessionStartReply ============================== */

void LLViewerChatterBoxSessionStartReply::describe(Description& desc) const
{
	desc.shortInfo("Used for receiving a reply to a request to initialize an ChatterBox session");
	desc.postAPI();
	desc.input(
		"{\"client_session_id\": UUID, \"session_id\": UUID, \"success\" boolean, \"reason\": string");
	desc.source(__FILE__, __LINE__);
}

void LLViewerChatterBoxSessionStartReply::post(ResponsePtr response,
				  const LLSD& context,
				  const LLSD& input) const
{
	std::cout << "LLViewerChatterBoxSessionStartReply::post()" << std::endl;
}



/*============================== LLViewerChatterBoxSessionEventReply ============================== */

void LLViewerChatterBoxSessionEventReply::describe(Description& desc) const
{
	desc.shortInfo("Used for receiving a reply to a ChatterBox session event");
	desc.postAPI();
	desc.input(
		"{\"event\": string, \"reason\": string, \"success\": boolean, \"session_id\": UUID");
	desc.source(__FILE__, __LINE__);
}

void LLViewerChatterBoxSessionEventReply::post(ResponsePtr response,
				  const LLSD& context,
				  const LLSD& input) const
{
	std::cout << "LLViewerChatterBoxSessionEventReply::post()" << std::endl;
}



/*============================== LLViewerForceCloseChatterBoxSession ============================== */

void LLViewerForceCloseChatterBoxSession::post(ResponsePtr response,
				  const LLSD& context,
				  const LLSD& input) const
{
	std::cout << "LLViewerForceCloseChatterBoxSession::post()" << std::endl;
}



/*============================== LLViewerChatterBoxSessionAgentListUpdates ============================== */

void LLViewerChatterBoxSessionAgentListUpdates::post(
	ResponsePtr responder,
	const LLSD& context,
	const LLSD& input) const
{
	boost::shared_ptr<LLC::ManagerImpl> mgr = LLC::ManagerImpl::GetInstance();
	//
	std::cout << "LLViewerChatterBoxSessionAgentListUpdates::post()" << std::endl;
	//
	LLSD body = input["body"];
	//
	LLUUID sessionId = body["session_id"].asUUID();
	//
	LLSD::map_const_iterator iter;
	LLSD::map_const_iterator end;
	//
	iter = body.beginMap();
	end  = body.endMap();
	std::cout << "body: " << std::endl;
	for( ; iter != end; ++iter )
	{
		std::cout
			<< "name=" << iter->first.c_str()
			<< ", type="
			<< iter->second.type()
			<< std::endl;
	}
	//
	if( body.has("agent_updates") && body["agent_updates"].isMap() )
	{
		iter = body["agent_updates"].beginMap();
		end  = body["agent_updates"].endMap();
		for( ; iter != end; ++iter )
		{
			LLUUID	agent_id( iter->first );
			LLSD 	agent_data( iter->second );
			std::cout << "\tagent_id: " << agent_id.asString().c_str() << std::endl;
			//
			if( agent_data.isMap() && agent_data.has("transition") )
			{
				{
					LLSD::map_const_iterator iter = agent_data.beginMap();
					LLSD::map_const_iterator end  = agent_data.endMap();
					std::cout << "\t\tagent_updates: " << std::endl;
					for( ; iter != end; ++iter )
					{
						std::cout
							<< "\t\t\tname=" << iter->first.c_str()
							<< ", type="
							<< iter->second.type()
							<< std::endl;
					}
				}
				{
					LLSD::map_const_iterator iter = agent_data["info"].beginMap();
					LLSD::map_const_iterator end  = agent_data["info"].endMap();
					std::cout << "\t\tagent_updates|info: " << std::endl;
					for( ; iter != end; ++iter )
					{
						std::cout
							<< "\t\t\tname=" << iter->first.c_str()
							<< ", type="
							<< iter->second.type()
							<< std::endl;
					}
				}
				//
				if( agent_data["transition"].asString() == "LEAVE" )
				{
					std::cout << "\t\tLEAVE" << std::endl;
					// left the chat
					//
					mgr->SendGroupChatAgentUpdateSignal( sessionId.getString(), agent_id.getString(), false );
				}
				else if( agent_data["transition"].asString() == "ENTER" )
				{
					std::cout << "\t\tENTER" << std::endl;
					// joining the chat
					//
					mgr->SendGroupChatAgentUpdateSignal( sessionId.getString(), agent_id.getString(), true );
				}
			}
		}
	}
	else if( body.has("updates") && body["updates"].isMap() )
	{
		iter = body["updates"].beginMap();
		end  = body["updates"].endMap();
		for( ; iter != end; ++iter )
		{
			LLUUID		agent_id( iter->first );
			std::string agent_transition( iter->second.asString() );
			//
			if( agent_transition == "LEAVE" )
			{
				// left the chat
				//
				mgr->SendGroupChatAgentUpdateSignal( sessionId.getString(), agent_id.getString(), false );
			}
			else if( agent_transition == "ENTER" )
			{
				// joining the chat
				//
				mgr->SendGroupChatAgentUpdateSignal( sessionId.getString(), agent_id.getString(), true );
			}
		}
	}
}



/*============================== LLViewerChatterBoxSessionUpdate ============================== */

void LLViewerChatterBoxSessionUpdate::post(
	ResponsePtr responder,
	const LLSD& context,
	const LLSD& input) const
{
	std::cout << "LLViewerChatterBoxSessionUpdate::post()" << std::endl;
}



/*============================== LLViewerChatterBoxInvitationAcceptResponder ============================== */

LLViewerChatterBoxInvitationAcceptResponder::LLViewerChatterBoxInvitationAcceptResponder( const LLUUID& session_id )
{
	mSessionID = session_id;
	std::cout << "LLViewerChatterBoxInvitationAcceptResponder()" << std::endl;
}

void LLViewerChatterBoxInvitationAcceptResponder::result(const LLSD& content)
{
	// TODO
	std::cout << "LLViewerChatterBoxInvitationAcceptResponder::result()" << std::endl;
}

void LLViewerChatterBoxInvitationAcceptResponder::error(U32 statusNum, const std::string& reason)
{		
	// TODO
	std::cout << "LLViewerChatterBoxInvitationAcceptResponder::error()" << std::endl;
}



/*============================== LLViewerChatterBoxInvitation ============================== */

void LLViewerChatterBoxInvitation::post(
	ResponsePtr response,
	const LLSD& context,
	const LLSD& input) const
{
	std::cout << "LLViewerChatterBoxInvitation()" << std::endl;

	//for backwards compatiblity reasons...we need to still
	//check for 'text' or 'voice' invitations...bleh
	if ( input["body"].has("instantmessage") )
	{
		LLSD message_params =
			input["body"]["instantmessage"]["message_params"];

		LLViewerRegionPtr vr = LLC::ManagerImpl::GetInstance()->GetViewerRegion();
		std::string url = vr->getCapability( "ChatSessionRequest" );
		LLUUID session_id = message_params["id"].asUUID();
		//LLUUID from_group      = message_params["from_group"].asUUID();
		//LLUUID from_id	       = message_params["from_id"].asUUID();
		std::string from_name  = message_params["from_name"].asString();
		std::string message    = message_params["message"].asString();

		LLC::ManagerImpl::GetInstance()->SendGroupChatInvite( session_id, from_name, message );

#if 0
		std::cout 
			<< "from_name=" << from_name.c_str()
			<< ", message=" << message.c_str()
			<< ", from_id=" << from_id.getString().c_str()
			<< ", from_group=" << from_group.getString().c_str()
			<< std::endl;

		{
			LLSD::map_iterator 			iter = message_params.beginMap();
			const LLSD::map_iterator 	end  = message_params.endMap();
			for( ; iter != end; ++iter )
			{
				std::string name = iter->first;
				std::cout << "message_param = " << name.c_str() << std::endl;
			}
		}
#endif

		if ( url != "" )
		{
			LLSD data;
			data["method"] = "accept invitation";
			data["session-id"] = session_id;
			LLHTTPClient::post(
				url,
				data,
				new LLViewerChatterBoxInvitationAcceptResponder(
					session_id ) );
					//, LLIMMgr::INVITATION_TYPE_INSTANT_MESSAGE));
		}
	} //end if invitation has instant message
}        


// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
