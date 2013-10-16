/** 
 * \brief Header for the C++ library interface to Linden Lab's source code
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
#pragma warning( disable:4996 )
#endif

// Local project
//
#include "LLChatLib.h"
#include "ManagerImpl.h"
#include "llworld.h"

// llxml
//
#include "llcontrol.h"

// llvfs
//
#include "lldir.h"

namespace LLC
{

/** \mainpage LLChatLib Developer Documentation
 *
 * \section Authentication
 *
 * The first thing a user must do is to authenticate using your username and password. You must first
 * munge your clear text password (see MungedPassword()). This allows you to preserve the password for later
 * use without storing it as clear text.
 *
 * CheckForResponse() allows you to check the status of your authentication. A response of "true" indicates
 * a successful auth. Exceptions can also be thrown to indicate the errors.
 *
 * Next, you then request a list of friends via RequestBuddyList(), followed by AnnounceInSim(), which
 * causes you to appear "online."
 *
 * \section Friendship
 *
 * At this point, you can SendInstantMessage() to a friend, by first looking up their UUID via LookupId().
 * If you know the UUID of the non-friend avatar, you can offer friendship via OfferFriendship(). When sent
 * an offer, you can either AcceptFriendship() or DeclineFriendship(). There are also signals that you receive
 * when you are offered friendship, or an offer is accepted or declined.
 *
 * \section Callbacks
 *
 * You can listen to various callbacks. See the individual documentation for each callback to understand what it does.
 * 
 * \li LLC::Manager::GetCacheSignal()
 * \li LLC::Manager::GetImSignal()
 * \li LLC::Manager::GetTypingSignal()
 * \li LLC::Manager::GetOnlineSignal()
 * \li LLC::Manager::GetFriendOfferSignal()
 * \li LLC::Manager::GetFriendAcceptSignal()
 * \li LLC::Manager::GetFriendDeclineSignal()
 * \li LLC::Manager::GetMessageBoxSignal()
 *
 * \sa LLC::Manager
 */


/**
 * \class Manager
 *
 * This class hides the working details of the SL codebase and provides a simple interface
 * for communicating in world via IM (a facade pattern).
 */

// Redirect into the PImpl class
//
Manager::Manager() :
	m_instance( ManagerImpl::GetInstance() )
{
	// Empty
}


Manager::~Manager()
{
	// Empty
}


/**
 * \brief Call this first to start the messaging system, otherwise nothing will work!
 *
 * \param [in] appname   		Application name, for example "SLiteChat".
 * \param [in] user_settings	Name of user settings file, for example "settings.xml".
 */
void Manager::StartMessagingSystem( const char* appname, const char* user_settings )
{
	m_instance->StartMessagingSystem(appname, user_settings);
}

/**
 * \brief Destroy all singleton and global instances and free up allocated memory.
 */
void Manager::Shutdown()
{
	ManagerImpl::Release();
}


/** \brief Declare string for persistence
 */
void Manager::DeclareString( const String& name, const String& value, const String& description )
{
	gSavedSettings.declareString( name.GetString(), value.GetString(), description.GetString() );
}


/** \brief Declare bool for persistence
 */
void Manager::DeclareBool( const String& name, const bool value, const String& description )
{
	gSavedSettings.declareBOOL( name.GetString(), value? 1: 0, description.GetString() );
}


/** \brief Declare long integer for persistence
 */
void Manager::DeclareInt( const String& name, const int value, const String& description )
{
	gSavedSettings.declareS32( name.GetString(), value, description.GetString() );
}


/** \brief Declare unsigned long integer for persistence
 */
void Manager::DeclareUInt( const String& name, const unsigned int value, const String& description )
{
	gSavedSettings.declareU32( name.GetString(), value, description.GetString() );
}

/** \brief Declare a rectangle (handy for GUI work)
 */
void Manager::DeclareRect( const String& name, const Rect& value, const String& description )
{
	LLRect llrect( value.m_left, value.m_top, value.m_right, value.m_bottom );
	gSavedSettings.declareRect( name.GetString(), llrect, description.GetString() );
}


/** \brief Set string for persistence
 */
void Manager::SetString( const String& name, const String& value )
{
	gSavedSettings.setString( name.GetString(), value.GetString() );
}


/** \brief Set bool for persistence
 */
void Manager::SetBool( const String& name, const bool value )
{
	gSavedSettings.setBOOL( name.GetString(), value? 1: 0 );
}


/** \brief Set long integer for persistence
 */
void Manager::SetInt( const String& name, const int value )
{
	gSavedSettings.setS32( name.GetString(), value );
}


/** \brief Set unsigned long integer for persistence
 */
void Manager::SetUInt( const String& name, const unsigned int value )
{
	gSavedSettings.setU32( name.GetString(), value );
}


/** \brief Set rectangle for persistence
 */
void Manager::SetRect( const String& name, const Rect& value )
{
	LLRect llrect( value.m_left, value.m_top, value.m_right, value.m_bottom );
	gSavedSettings.setRect( name.GetString(), llrect );
}


/** \brief Retrieve string from persistence
 */
String Manager::GetString( const String& name ) const
{
	return gSavedSettings.getString( name.GetString() ).c_str();
}


/** \brief Retrieve bool from persistence
 */
bool Manager::GetBool( const String& name ) const
{
	return gSavedSettings.getBOOL( name.GetString() )? true: false;
}


/** \brief Retrieve long integer from persistence
 */
int Manager::GetInt( const String& name ) const
{
	return gSavedSettings.getS32( name.GetString() );
}


/** \brief Retrieve unsigned long integer from persistence
 */
unsigned int Manager::GetUInt( const String& name ) const
{
	return gSavedSettings.getU32( name.GetString() );
}


/** \brief Retrieve rectangle from persistence
 */
Manager::Rect Manager::GetRect( const String& name ) const
{
	LLRect llrect = gSavedSettings.getRect( name.GetString() );
	return Rect( llrect.mLeft, llrect.mTop, llrect.mRight, llrect.mBottom );
}


/**
 * \brief Get the user settings path, a place to write files stored in the user area (e.g. ~/.slitechat/foo.txt).
 *
 * \return String containing full path.
 */
String Manager::GetUserSettingsPath() const
{
	return String( gDirUtilp->getExpandedFilename( LL_PATH_USER_SETTINGS, "" ).c_str() );
}


/**
 * \brief Authenticate against the SL database.
 *
 * \param [in] login_url		Login URL for the grid, for example "https://login.agni.lindenlab.com/cgi-bin/login.cgi".
 * \param [in] first_name		First name of avatar.
 * \param [in] last_name		Last name of avatar.
 * \param [in] munged_password	Munged (non-clear text) password
 * \param [in] starting_slurl	Where to appear when logged in. Defaults to "home".
 *
 * \sa MungedPassword()
 */
void Manager::Authenticate( const String& login_url, const String& first_name, const String& last_name, const String& munged_password, const String& starting_slurl )
{
	m_instance->Authenticate( login_url, first_name, last_name, munged_password, starting_slurl );
}


/**
 * \brief Non-blocking call to check for authentication server response.
 *
 * This method witll throw AuthException if there is a problem authenticating. Check the message for details.
 *
 * \return true means that login was completely successful, and false means that we are still waiting for authentication.
 *
 * \sa AuthException()
 */
bool Manager::CheckForResponse()
{
	return m_instance->CheckForResponse();
}


/**
 * \brief Sends request to the server for buddy list members.
 */
void Manager::RequestBuddyList()
{
	m_instance->RequestBuddyList();
}


/** \brief Lookup first/last name by agent ID from SL cache
 *
 * \param [in]  id			text representation of agent ID.
 * \param [out] first_name	reference to hold looked up value.
 * \param [out] last_name	reference to hold looked up value.
 *
 * \bug No failure code is reported, so if not in cache you get "(Loading...)"
 * \sa GetFullName()
 */
void Manager::GetNameFromCache( const String& id, String& first_name, String& last_name )
{
	LLUUID agent_id( id.GetString() );
	std::string first,last;
	//
	m_instance->GetNameFromCache( agent_id, first, last);

	// Return values passed back by reference
	//
	first_name.SetString( first.c_str() );
	last_name.SetString( last.c_str() );
}


/** \brief Lookup first/last name by agent ID from SL cache
 *
 * \param [in]  id			text representation of agent ID.
 * \param [out] full_name	reference to string holding first + " " + last
 *
 * \bug No failure code is reported, so if not in cache you get "(Loading...)"
 * \sa GetFullName()
 */
void Manager::GetNameFromCache( const String& id, String& full_name )
{
	LLUUID agent_id( id.GetString() );
	std::string name;
	m_instance->GetNameFromCache( agent_id, name );
	full_name.SetString( name.c_str() );
}


/** \brief Lookup full name by agent ID from SL cache
 *
 * \param [in]  id			text representation of agent ID.
 * \return string holding first + " " + last
 *
 * \bug No failure code is reported, so if not in cache you get "(Loading...)"
 * \sa GetFullName()
 */
String Manager::GetNameFromCache( const String& id )
{
	LLUUID agent_id( id.GetString() );
	std::string full_name;
	m_instance->GetNameFromCache( agent_id, full_name );
	return full_name.c_str();
}


/** \brief Lookup full name by agent ID. Deprecated.
 *
 * \param [in]	id	text representation of agent ID.
 * \return String of full name of looked up value. This contains an entry from the last cache update and is a local store.
 * \sa GetNameFromCache()
 */
String Manager::GetFullName( const String& id )
{
	return m_instance->GetFullName( id );
}


/** \brief Lookup ID by full name.
 * \param [in] fullname Pointer to full name string.
 * \return Text representation of agent ID found.
 * \sa GetName(), GetFullName()
 */
String Manager::LookupId( const String& fullname )
{
	return m_instance->LookupId( fullname );
}


/** \brief Lookup group name by UUID
 * \param[in] id Text representation of agent ID
 * \return Group name
 */
String Manager::LookupGroupName( const String& id )
{
	return m_instance->LookupGroupName( id );
}


/** \brief Get currently logged in agent id string.
 * \return LLC::String containing agent id.
 */
String Manager::GetAgentId() const
{
	LLUUID id = m_instance->GetAgentId();
	return String( id.asString().c_str() );
}


/** \brief Determine agent's online status.
 * \param [in]	id	Text representation of agent id.
 * \return true if found.
 */
bool Manager::IsOnline( const String& id )
{
	return m_instance->IsOnline( id );
}


/** \brief Check to see if id appears in our cache, if so then we have a friend.
 * \param [in] id	Agent id to check for in cache
 * \return true if in cache
 */
bool Manager::IsFriend( const String& id )
{
	return m_instance->IsFriend( id );
}


/** \brief Get the current agent language.
 * \param [in] agentId	- UUID of agent
 * \return Code of the language
 */
String Manager::GetAgentLanguage( const String& agentId ) const
{
	return m_instance->GetAgentLanguage( LLUUID(agentId.GetString()) ).c_str();
}


/** \brief Set the current agent language.
 * \param [in] agentId	- UUID of agent
 * \param [in] language	- Language code
 */
void Manager::SetAgentLanguage( const String& agentId, const String& language )
{
	m_instance->SetAgentLanguage( LLUUID(agentId.GetString()), language.GetString() );
}


/** \brief Get the auto-detect language setting for the agent
 * \param [in] agentId - Agent to query
 * \return true if auto-detect is set
 */
bool Manager::GetAgentLanguageAuto( const String& agentId ) const
{
	return m_instance->GetAgentLanguageAuto( LLUUID(agentId.GetString()) );
}


/** \brief Get the auto-detect language setting for the agent
 * \param [in] agentId - Agent to query
 * \param [in] val - on or off
 */
void Manager::SetAgentLanguageAuto( const String& agentId, const bool val )
{
	m_instance->SetAgentLanguageAuto( LLUUID(agentId.GetString()), val );
}


/** \brief Get count of local avatars in chat range.
 */
int Manager::GetLocalAvatarCount() const
{
	return m_instance->GetLocalAvatarCount();
}


/** \brief Get local avatar in range by index
 */
String Manager::GetLocalAvatar( const int idx ) const
{
	return m_instance->GetLocalAvatar( idx );
}


/** \brief Announce in simulator that the agent is now authenticated and wants to enter.
 */
void Manager::AnnounceInSim()
{
	m_instance->AnnounceInSim();
}


/** \brief Pump the incoming tcp/ip messages from server.
 */
void Manager::PumpMessages()
{
	m_instance->PumpMessages();
}


/** \brief Check to make sure we are logged int
 */
bool Manager::IsOnline()
{
	return gMessageSystem->mCircuitInfo.findCircuit( gHost ) != 0;
}


/** \brief Send to server the logout request packet. This generates a response.
 */
void Manager::RequestLogout()
{
	m_instance->RequestLogout();
}


/** \brief Send instant message to target agent.
 *
 * \param [in]	to_id		agent id to send message to.
 * \param [in]	message		string of message to send.
 * \param [in]	to_group	true if to_id is a group, not an AV (i.e. this is part of a chat session)
 */
void Manager::SendInstantMessage( const String& to_id, const String& message, const bool to_group )
{
	m_instance->SendInstantMessage( to_id, message, to_group );
}
	

/** \brief Send typing state signal
 * 
 * \param [in] target_id	agent group id to send status to
 * \param [in] to_group		true if group
 * \param [in] typing		Started or stopped typing
 */
void Manager::SendTypingSignal( const String& target_id, const bool to_group, const bool typing )
{
	m_instance->SendTypingSignal( target_id, to_group, typing );
}


/** \brief Send a message to local chat.
 *
 * \param [in]	message	string of message to send.
 */
void Manager::SendLocalChatMessage( const String& message, const int channel )
{
	m_instance->SendLocalChatMessage( message, channel );
}
	

/** \brief Request to start a group chat
 *
 * \param [in] group_id Start a group chat
 */
void Manager::SendGroupChatStartRequest( const String& group_id )
{
	m_instance->SendGroupChatStartRequest( group_id );
}


/** \brief Send a leave request to the specified group chat session
 *  \param [in] group_session_id Session to leave
 */
void Manager::SendGroupChatLeaveRequest( const String& group_session_id )
{
	m_instance->SendGroupChatLeaveRequest( group_session_id );
}


/** \brief Offer friendship to agent.
 *
 * \param [in]	target_id	Agent id to target.
 * \param [in]	message		String message to send to agent.
 * \sa AcceptFriendship(), DeclineFriendship()
 */
void Manager::OfferFriendship( const String& target_id, const String& message )
{
	m_instance->OfferFriendship( target_id, message );
}


/** \brief Accept friendship request from agent.
 *
 * \param [in]	sessionId	Session id of the request. This comes from offer signal.
 * \param [in]	senderIp	IP of sender, comes from offer signal.
 *
 * \sa FriendOfferSignal, DeclineFriendship()
 */
void Manager::AcceptFriendship( const String& sessionId, const String& senderIp )
{
	m_instance->AcceptFriendship( sessionId, senderIp );
}


/** \brief Decline friendship request from agent.
 *
 * \param [in]	sessionId	Session id of the request. This comes from offer signal.
 * \param [in]	senderIp	IP of sender, comes from offer signal.
 *
 * \sa FriendOfferSignal, AcceptFriendship()
 */
void Manager::DeclineFriendship( const String& sessionId, const String& senderIp )
{
	m_instance->DeclineFriendship( sessionId, senderIp );
}


/** \brief Terminate friendship with and existing friend
 * 
 * \param [in] agentId	Agent id of friend to terminate relationship with.
 */
void Manager::TerminateFriendship( const String& agentId )
{
	m_instance->TerminateFriendship( agentId );
}
	

/** \brief Accept an offer to join a group
 * 
 * \param [in] group_id  - Group id to join
 * \param [in] message   - An optional message
 * \param [in] sessionId - transaction ID of request session
 */
void Manager::AcceptGroupJoinOffer( const String& group_id, const String& message, const String& sessionId )
{
	m_instance->AcceptGroupJoinOffer( group_id, message, sessionId );
}


/** \brief Decline an offer to join a group
 * 
 * \param [in] group_id  - Group id to decline
 * \param [in] message   - An optional message
 * \param [in] sessionId - transaction ID of request session
 */
void Manager::DeclineGroupJoinOffer( const String& group_id, const String& message, const String& sessionId )
{
	m_instance->DeclineGroupJoinOffer( group_id, message, sessionId );
}


/** \brief Request to leave a group
 * 
 * \param [in] groupId - id of group to leave
 */
void Manager::LeaveGroupRequest( const String& groupId )
{
	m_instance->LeaveGroupRequest( groupId );
}


/** \brief Search the people database for a matching full name.
 * Sends a search request. You will get a search result signal.
 * \sa SearchResultSignal()
 */
void Manager::SearchPeople( const String& fullname )
{
	m_instance->SearchPeople( fullname );
}


/** \brief Number of hits returned in people search.
 * \sa SearchResultSignal(), SearchPeople(), GetPerson()
 */
int Manager::GetPeopleSearchCount() const
{
	return m_instance->GetPeopleSearchCount();
}


/** \brief Get a full name from search results.
 *
 * \param [in] index	A 0-based index of search result.
 * \sa GetPeopleSearchCount()
 */
String Manager::GetPerson( const int index ) const
{
	return m_instance->GetPerson( index );
}



/** \brief Get the landmark of the current sim we are in.
 * \param [out] region_name - name of the region
 * \param [out] x, y, z - coordinates
 */
void Manager::GetLM( String& region_name, int& x, int& y, int& z ) const
{
	S32 sx, sy, sz;
	std::string rname;
	m_instance->GetLM( rname, sx, sy, sz );
	region_name = rname.c_str();
	x = (int) sx;
	y = (int) sy;
	z = (int) sz;
}


/** \brief Get the SLURL of the current sim we are in.
 * \return String containing SLURL.
 */
String Manager::GetSLURL() const
{
	return String( m_instance->GetSLURL().c_str() );
}


/** \brief Teleport to the offered lure by remote agent
 */
void Manager::TeleportViaLure( const String& lureId )
{
	LLUUID uuid( lureId.GetString() );
	m_instance->TeleportViaLure( uuid );
}


/** \brief Teleport your AV to the specified SLURL.
 */
void Manager::TeleportToRegion( const String& slurl )
{
	m_instance->TeleportToRegion( slurl.GetString() );
}


/** \brief Get the default source language for the current AV
 */
String Manager::GetLanguage() const
{
	return String( m_instance->GetLanguage().c_str() );
}


/** \brief Set the default source language for the current AV
 */
void Manager::SetLanguage( const String& lang_id )
{
	m_instance->SetLanguage( lang_id.GetString() );
}


/** \brief Turn on or off language translation
 */
void Manager::SetTranslateMessages( const bool val )
{
	m_instance->SetTranslateMessages( val );
}


/** \brief Teleport home
 */
void Manager::TeleportHome()
{
	m_instance->TeleportHome();
}


/** \brief Signals which eminate to your client app as messages come in from the remote grid server.
 */

Connection	Manager::ConnectFriendAddSignal             ( const StringSignal               ::slot_type& slot ) const { return m_instance->ConnectFriendAddSignal              ( slot ); }
Connection	Manager::ConnectCacheSignal                 ( const CacheSignal                ::slot_type& slot ) const { return m_instance->ConnectCacheSignal                  ( slot ); }
Connection	Manager::ConnectGroupCacheSignal            ( const GroupCacheSignal           ::slot_type& slot ) const { return m_instance->ConnectGroupCacheSignal             ( slot ); }
Connection	Manager::ConnectImSignal                    ( const ImSignal                   ::slot_type& slot ) const { return m_instance->ConnectImSignal                     ( slot ); }
Connection	Manager::ConnectGroupChatSignal             ( const GroupChatSignal            ::slot_type& slot ) const { return m_instance->ConnectGroupChatSignal              ( slot ); }
Connection	Manager::ConnectLocalChatSignal             ( const LocalChatSignal            ::slot_type& slot ) const { return m_instance->ConnectLocalChatSignal              ( slot ); }
Connection	Manager::ConnectGroupChatAgentUpdateSignal  ( const GroupChatAgentUpdateSignal ::slot_type& slot ) const { return m_instance->ConnectGroupChatAgentUpdateSignal   ( slot ); }
Connection	Manager::ConnectTypingSignal                ( const TypingSignal               ::slot_type& slot ) const { return m_instance->ConnectTypingSignal                 ( slot ); }
Connection	Manager::ConnectOnlineSignal                ( const OnlineSignal               ::slot_type& slot ) const { return m_instance->ConnectOnlineSignal                 ( slot ); }
Connection	Manager::ConnectTerminateFriendshipSignal   ( const StringSignal               ::slot_type& slot ) const { return m_instance->ConnectTerminateFriendshipSignal    ( slot ); }
Connection	Manager::ConnectFriendOfferSignal           ( const FriendOfferSignal          ::slot_type& slot ) const { return m_instance->ConnectFriendOfferSignal            ( slot ); }
Connection	Manager::ConnectFriendAcceptSignal          ( const CacheSignal                ::slot_type& slot ) const { return m_instance->ConnectFriendAcceptSignal           ( slot ); }
Connection	Manager::ConnectFriendDeclineSignal         ( const CacheSignal                ::slot_type& slot ) const { return m_instance->ConnectFriendDeclineSignal          ( slot ); }
Connection	Manager::ConnectMessageBoxSignal            ( const StringSignal               ::slot_type& slot ) const { return m_instance->ConnectMessageBoxSignal             ( slot ); }
Connection	Manager::ConnectSearchResultSignal          ( const SearchResultSignal         ::slot_type& slot ) const { return m_instance->ConnectSearchResultSignal           ( slot ); }
Connection	Manager::ConnectForcedQuitSignal            ( const StringSignal               ::slot_type& slot ) const { return m_instance->ConnectForcedQuitSignal             ( slot ); }
Connection	Manager::ConnectLogoutReplySignal           ( const VoidSignal                 ::slot_type& slot ) const { return m_instance->ConnectLogoutReplySignal            ( slot ); }
Connection	Manager::ConnectGroupOfferSignal            ( const GroupOfferSignal           ::slot_type& slot ) const { return m_instance->ConnectGroupOfferSignal             ( slot ); }
Connection	Manager::ConnectAgentMovementCompleteSignal ( const VoidSignal                 ::slot_type& slot ) const { return m_instance->ConnectAgentMovementCompleteSignal  ( slot ); }
Connection	Manager::ConnectLocalAgentsPresentSignal    ( const VoidSignal                 ::slot_type& slot ) const { return m_instance->ConnectLocalAgentsPresentSignal     ( slot ); }
Connection	Manager::ConnectTeleportRequestSignal       ( const TeleportRequestSignal      ::slot_type& slot ) const { return m_instance->ConnectTeleportRequestSignal        ( slot ); }
Connection	Manager::ConnectTeleportRequestedSignal     ( const TeleportRequestedSignal    ::slot_type& slot ) const { return m_instance->ConnectTeleportRequestedSignal      ( slot ); }
Connection	Manager::ConnectTeleportStartSignal         ( const TeleportStartSignal        ::slot_type& slot ) const { return m_instance->ConnectTeleportStartSignal          ( slot ); }
Connection	Manager::ConnectTeleportProgressSignal      ( const TeleportProgressSignal     ::slot_type& slot ) const { return m_instance->ConnectTeleportProgressSignal       ( slot ); }
Connection	Manager::ConnectTeleportLocalSignal         ( const VoidSignal                 ::slot_type& slot ) const { return m_instance->ConnectTeleportLocalSignal          ( slot ); }
Connection	Manager::ConnectTeleportFailedSignal        ( const TeleportFailedSignal       ::slot_type& slot ) const { return m_instance->ConnectTeleportFailedSignal         ( slot ); }
Connection	Manager::ConnectTeleportFinishSignal        ( const VoidSignal                 ::slot_type& slot ) const { return m_instance->ConnectTeleportFinishSignal         ( slot ); }


}
// namespace LLC


// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
