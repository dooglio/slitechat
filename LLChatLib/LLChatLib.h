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
#ifndef __LLCHATLIB_H__
#define __LLCHATLIB_H__

#include "LLChatLibExports.h"

#include <boost/smart_ptr.hpp>
#include <boost/signals.hpp>

namespace LLC
{


class LLCHATLIBEXP StringImpl;
class LLCHATLIBEXP String
{
public:
				String();
				String( const char* str );
	virtual		~String();

	const char* GetString() const;
	void        SetString( const char* str );

private:
	boost::shared_ptr<StringImpl>	m_instance;
};


class LLCHATLIBEXP AuthException
{
public:
				AuthException()	{}
				AuthException( const char* msg ) : m_message(msg) {}
	virtual		~AuthException() {}

	const char*	GetMessageText() const { return m_message.GetString(); }
	void 		SetMessageText( const char* msg ) { m_message.SetString( msg ); }

private:
	String		m_message;
};


LLCHATLIBEXP String	MungedPassword( const String& clear_text );

typedef boost::signals::connection Connection;

class LLCHATLIBEXP  ManagerImpl;
class LLCHATLIBEXP	Manager
{
public:
					Manager();
	virtual  		~Manager();

	void			Shutdown();
	
	void			StartMessagingSystem( const char* appname, const char* user_settings );

	// Persistence
	//
	struct Rect
	{
		int m_left, m_top, m_right, m_bottom;
		
		Rect() : m_left(0), m_top(0), m_right(0), m_bottom(0) {}
		Rect( int l, int t, int r, int b ) : m_left(l), m_top(t), m_right(r), m_bottom(b) {}
		
		int GetWidth() const { return m_right - m_left; }
		int GetHeight() const { return m_bottom - m_right; }
	};
	
	void			DeclareString( const String& name, const String& value, const String& description );
	void			DeclareBool( const String& name, const bool value, const String& description );
	void			DeclareInt( const String& name, const int value, const String& description );
	void			DeclareUInt( const String& name, const unsigned int value, const String& description );
	void			DeclareRect( const String& name, const Rect& value, const String& description );
	//
	void			SetString( const String& name, const String& value );
	void			SetBool( const String& name, const bool value );
	void			SetInt( const String& name, const int value );
	void			SetUInt( const String& name, const unsigned int value );
	void			SetRect( const String& name, const Rect& value );
	//
	String			GetString( const String& name ) const;
	bool			GetBool( const String& name ) const;
	int				GetInt( const String& name ) const;
	unsigned int	GetUInt( const String& name ) const;
	Rect			GetRect( const String& name ) const;

	String			GetUserSettingsPath() const;
	
	void			Authenticate( const String &login_url, const String& first_name, const String& last_name, const String& munged_password, const String& starting_slurl = "home" );
	
	bool			CheckForResponse();
	void			RequestBuddyList();
	void        	AnnounceInSim();

	bool			IsOnline();
	void			PumpMessages();
	void			RequestLogout();
	
	void			GetNameFromCache( const String& id, String& first_name, String& last_name );
	void			GetNameFromCache( const String& id, String& full_name );
	String			GetNameFromCache( const String& id );
	String			GetFullName( const String& id );
	String			LookupId( const String& fullname );
	String			LookupGroupName( const String& id );
	String			GetAgentId() const;
	bool			IsOnline( const String& id );
	bool			IsFriend( const String& id );

	String			GetAgentLanguage( const String& agentId ) const;
	void			SetAgentLanguage( const String& agentId, const String& language );
	//
	bool			GetAgentLanguageAuto( const String& agentId ) const;
	void			SetAgentLanguageAuto( const String& agentId, const bool val );

	int				GetLocalAvatarCount() const;
	String			GetLocalAvatar( const int idx ) const;

	void			SendInstantMessage( const String& to_id, const String& message, const bool to_group = false );
	void			SendTypingSignal( const String& target_id, const bool to_group, const bool typing );
	void			SendLocalChatMessage( const String& text, const int channel = 0 );
	void			SendGroupChatStartRequest( const String& group_id );
	void			SendGroupChatLeaveRequest( const String& group_session_id );
	void			OfferFriendship( const String& target_id, const String& message );
	void			AcceptFriendship( const String& sessionId, const String& senderIp );
	void			DeclineFriendship( const String& sessionId, const String& senderIp );
	void			TerminateFriendship( const String& agentId );
	void			AcceptGroupJoinOffer( const String& group_id, const String& message, const String& sessionId );
	void			DeclineGroupJoinOffer( const String& group_id, const String& message, const String& sessionId );
	void			LeaveGroupRequest( const String& groupId );
	
	void			SearchPeople( const String& fullname );
	int				GetPeopleSearchCount() const;
	String			GetPerson( const int index ) const;
	void			GetLM( String& region_name, int& x, int& y, int& z ) const;
	String			GetSLURL() const;

	String			GetLanguage() const;
	void			SetLanguage( const String& lang_id );

	void			SetTranslateMessages( const bool val );

	void			TeleportViaLure( const String& lureId );
	void			TeleportToRegion( const String& slurl );
	void			TeleportHome ();

	typedef boost::signal	< void
							( String				// agent_id
							, String				// avatar full name
							, bool					// is_group
							)
							> CacheSignal;
	typedef boost::signal	< void
							( String				// from_id
							, String				// name
							, bool					// has_me: true if started with "/me"
							, String				// message
							, String				// translated message
							, String				// detected language id
							)
							> ImSignal;
	typedef boost::signal	< void
							( String				// group_id
							, String				// group_name
							, String				// from_id
							, bool					// has_me: true if started with "/me"
							, String				// message
							, String				// translated message
							, String				// detected language id
							)
							> GroupChatSignal;
	typedef boost::signal	< void
	   						( String				// from_id
							, String				// name
							, String				// verb
							, bool					// has_me: true if started with "/me"
							, String				// message
							, String				// translated message
							, String				// detected language id
							)
							> LocalChatSignal;
	typedef boost::signal	< void
							( String				// session/group id
							, String				// agent_id
							, bool					// true=entering, false=leaving
							)
							> GroupChatAgentUpdateSignal;	// Sent to indicate entering/leaving the chat
	typedef boost::signal	< void
							( String				// from_id
							, String				// name
							, bool					// start typing
							)
							> TypingSignal;
	typedef boost::signal	< void
							( String				// name
							, bool					// is_online
							)
							> OnlineSignal;	
	typedef boost::signal	< void
							( String				// agent_id
							, String				// full_name
							, String				// message
							, String				// session_id
							, bool					// is_online
							, String				// ip_and_port
							)
							> FriendOfferSignal;
	typedef boost::signal	< void
							( String				// name
							)
							> StringSignal;
	typedef boost::signal	< void (void)
							> VoidSignal;
	typedef boost::signal	< void
							( String				// agent_id
							, String				// full_name
							)
							> SearchResultSignal;
	typedef boost::signal	< void
							( String				// group_id
							, String				// full_name
							)
							> GroupCacheSignal;
	typedef boost::signal	< void
							( String				// group_id
							, String				// full_name
							, String				// message
							, String				// session_id
							, long					// fee_amount
							)
							> GroupOfferSignal;
	typedef boost::signal	< void
							( String				// Full name of sender
							, String				// Message from agent
							, String				// UUID of lure
							)
							> TeleportRequestSignal;
	typedef boost::signal	< void
							( String				// message
							)
							> TeleportRequestedSignal;
	typedef boost::signal	< void
							( bool					// can_cancel
							)
							> TeleportStartSignal;
	typedef boost::signal	< void
							( bool					// can_cancel
							, String				// message
							)
							> TeleportProgressSignal;
	typedef boost::signal	< void
							( String				// reason
							)
							> TeleportFailedSignal;
	//
	Connection	ConnectFriendAddSignal             ( const StringSignal               ::slot_type& slot ) const;
	Connection	ConnectCacheSignal                 ( const CacheSignal                ::slot_type& slot ) const;
	Connection	ConnectGroupCacheSignal            ( const GroupCacheSignal           ::slot_type& slot ) const;
	Connection	ConnectImSignal                    ( const ImSignal                   ::slot_type& slot ) const;
	Connection	ConnectGroupChatSignal             ( const GroupChatSignal            ::slot_type& slot ) const;
	Connection	ConnectLocalChatSignal             ( const LocalChatSignal            ::slot_type& slot ) const;
	Connection	ConnectGroupChatAgentUpdateSignal  ( const GroupChatAgentUpdateSignal ::slot_type& slot ) const;
	Connection	ConnectTypingSignal                ( const TypingSignal               ::slot_type& slot ) const;
	Connection	ConnectOnlineSignal                ( const OnlineSignal               ::slot_type& slot ) const;
	Connection	ConnectTerminateFriendshipSignal   ( const StringSignal               ::slot_type& slot ) const;
	Connection	ConnectFriendOfferSignal           ( const FriendOfferSignal          ::slot_type& slot ) const;
	Connection	ConnectFriendAcceptSignal          ( const CacheSignal                ::slot_type& slot ) const;
	Connection	ConnectFriendDeclineSignal         ( const CacheSignal                ::slot_type& slot ) const;
	Connection	ConnectMessageBoxSignal            ( const StringSignal               ::slot_type& slot ) const;
	Connection	ConnectSearchResultSignal          ( const SearchResultSignal         ::slot_type& slot ) const;
	Connection	ConnectForcedQuitSignal            ( const StringSignal               ::slot_type& slot ) const;
	Connection	ConnectLogoutReplySignal           ( const VoidSignal                 ::slot_type& slot ) const;
	Connection	ConnectGroupOfferSignal            ( const GroupOfferSignal           ::slot_type& slot ) const;
	Connection	ConnectAgentMovementCompleteSignal ( const VoidSignal                 ::slot_type& slot ) const;
	Connection	ConnectLocalAgentsPresentSignal    ( const VoidSignal                 ::slot_type& slot ) const;
	//
	Connection	ConnectTeleportRequestSignal       ( const TeleportRequestSignal      ::slot_type& slot ) const;
	Connection	ConnectTeleportRequestedSignal     ( const TeleportRequestedSignal    ::slot_type& slot ) const;
	Connection	ConnectTeleportStartSignal         ( const TeleportStartSignal        ::slot_type& slot ) const;
	Connection	ConnectTeleportProgressSignal      ( const TeleportProgressSignal     ::slot_type& slot ) const;
	Connection	ConnectTeleportLocalSignal         ( const VoidSignal                 ::slot_type& slot ) const;
	Connection	ConnectTeleportFailedSignal        ( const TeleportFailedSignal       ::slot_type& slot ) const;
	Connection	ConnectTeleportFinishSignal        ( const VoidSignal                 ::slot_type& slot ) const;

private:
	boost::shared_ptr<ManagerImpl>	m_instance;
};

}
// namespace LLC

#endif // __LLCHATLIB_H__

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
