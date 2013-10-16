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
#ifndef __MANAGERIMPL_H__
#define __MANAGERIMPL_H__

// Local project
//
#include "LLChatLib.h"
#include "StringImpl.h"
#include "Agent.h"

// Boost
//
#include <boost/shared_ptr.hpp>
#include <boost/signals.hpp>

// StdC++
//
#include <map>

//============== LINDEN Libraries =====================
//
#include "stdtypes.h"
#include "lluserauth.h"
#include "lluuid.h"
#include "message.h"
#include "v3math.h"
#include "lljoint.h"
#include "llwearable.h"

class LLViewerRegion;

namespace LLC
{

//class LLUserAuth;
//class LLUUID;
//class LLMessageSystem;

class LLCHATLIBEXP	ManagerImpl // : public RefCounted
{
public:
	static boost::shared_ptr<ManagerImpl>	GetInstance();
	static void Release();
	//
	virtual  	~ManagerImpl();

	void		StartMessagingSystem( const char* appname, const char* user_settings );
	void		Authenticate( const String& login_url, const String& first_name, const String& last_name, const String& munged_password, const String& starting_slurl );

	bool		CheckForResponse();
	void		RequestBuddyList();
	void		GetNameFromCache( const LLUUID& id, std::string& first_name, std::string& last_name );
	void		GetNameFromCache( const LLUUID& id, std::string& full_name );
	String		GetFullName( const String& id );
	String		LookupId( const String& fullname );
	String		LookupGroupName( const String& id );
	bool		IsOnline( const String& id );
	bool		IsFriend( const String& id );

	void		UpdateLocalAvatars();
	int			GetLocalAvatarCount() const;
	String		GetLocalAvatar( const int idx ) const;

	void		AnnounceInSim();

	void		PumpMessages();
	void		RequestLogout();

	void		SendInstantMessage( const String& to_id, const String& message, const bool to_group );
	void		SendLocalChatMessage( const String& text, const int channel );
	void		SendGroupChatStartRequest( const String& group_id );
	void		SendTypingSignal( const String& target_id, const bool to_group, const bool typing );
	void		SendGroupChatLeaveRequest( const String& group_session_id );
	void		OfferFriendship( const String& target_id, const String& message );
	void		AcceptFriendship( const String& sessionId, const String& senderIp );
	void		DeclineFriendship( const String& sessionId, const String& senderIp );
	void		TerminateFriendship( const String& agentId );
	
	void		AcceptGroupJoinOffer( const String& group_id, const String& message, const String& sessionId );
	void		DeclineGroupJoinOffer( const String& group_id, const String& message, const String& sessionId );
	void		LeaveGroupRequest( const String& groupId );

	void		SearchPeople( const String& fullname );
	int			GetPeopleSearchCount() const;
	String		GetPerson( const int index ) const;
	
	LLUUID		GetAgentId() const { return m_agentId; }
	LLUUID		GetSessionId() const { return m_sessionId; }
	LLUUID		GetSecureSessionId() const { return m_secureSessionId; }
	void		GetLM( std::string& region_name, S32& x, S32& y, S32& z ) const;
	std::string GetSLURL() const;
    
    LLViewerRegionPtr GetViewerRegion() { return m_viewerRegion; }

	void TeleportViaLure( const LLUUID& lure_id );
	void TeleportToRegion( const std::string& slurl );
	void TeleportHome();

	void SendGroupChatInvite( const LLUUID& groupId, const std::string& fromName, const std::string& message );

	std::string	GetLanguage() const { return m_langId; }
	void		SetLanguage( const std::string& lang_id )	{ m_langId = lang_id; }
	//
	std::string	GetAgentLanguage( const LLUUID& agentId ) const;
	void		SetAgentLanguage( const LLUUID& agentId, const std::string& language );
	//
	bool		GetAgentLanguageAuto( const LLUUID& agentId ) const;
	void		SetAgentLanguageAuto( const LLUUID& agentId, const bool val );

	void SetTranslateMessages( const bool val ) { m_translateMessages = val; }

	void SendGroupChatAgentUpdateSignal( const std::string& session_id, const std::string& agent_id, const bool entering );

	Connection ConnectFriendAddSignal             ( const Manager::StringSignal               ::slot_type& slot ) { return m_friendAddSignal             .connect(slot); }
	Connection ConnectTerminateFriendshipSignal   ( const Manager::StringSignal               ::slot_type& slot ) { return m_terminateFriendshipSignal   .connect(slot); }
	Connection ConnectCacheSignal                 ( const Manager::CacheSignal                ::slot_type& slot ) { return m_cacheSignal                 .connect(slot); }
	Connection ConnectGroupCacheSignal            ( const Manager::GroupCacheSignal           ::slot_type& slot ) { return m_groupCacheSignal            .connect(slot); }
	Connection ConnectImSignal                    ( const Manager::ImSignal                   ::slot_type& slot ) { return m_imSignal                    .connect(slot); }
	Connection ConnectGroupChatSignal             ( const Manager::GroupChatSignal            ::slot_type& slot ) { return m_groupChatSignal             .connect(slot); }
	Connection ConnectLocalChatSignal             ( const Manager::LocalChatSignal            ::slot_type& slot ) { return m_localChatSignal             .connect(slot); }
	Connection ConnectGroupChatAgentUpdateSignal  ( const Manager::GroupChatAgentUpdateSignal ::slot_type& slot ) { return m_groupChatAgentUpdateSignal  .connect(slot); }
	Connection ConnectTypingSignal                ( const Manager::TypingSignal               ::slot_type& slot ) { return m_typingSignal                .connect(slot); }
	Connection ConnectOnlineSignal                ( const Manager::OnlineSignal               ::slot_type& slot ) { return m_onlineSignal                .connect(slot); }
	Connection ConnectFriendOfferSignal           ( const Manager::FriendOfferSignal          ::slot_type& slot ) { return m_friendOfferSignal           .connect(slot); }
	Connection ConnectFriendAcceptSignal          ( const Manager::CacheSignal                ::slot_type& slot ) { return m_friendAcceptSignal          .connect(slot); }
	Connection ConnectFriendDeclineSignal         ( const Manager::CacheSignal                ::slot_type& slot ) { return m_friendDeclineSignal         .connect(slot); }
	Connection ConnectMessageBoxSignal            ( const Manager::StringSignal               ::slot_type& slot ) { return m_messageBoxSignal            .connect(slot); }
	Connection ConnectSearchResultSignal          ( const Manager::SearchResultSignal         ::slot_type& slot ) { return m_searchResultSignal          .connect(slot); }
	Connection ConnectForcedQuitSignal            ( const Manager::StringSignal               ::slot_type& slot ) { return m_forceQuitSignal             .connect(slot); }
	Connection ConnectLogoutReplySignal           ( const Manager::VoidSignal                 ::slot_type& slot ) { return m_logoutReplySignal           .connect(slot); }
	Connection ConnectGroupOfferSignal            ( const Manager::GroupOfferSignal           ::slot_type& slot ) { return m_groupOfferSignal            .connect(slot); }
	Connection ConnectAgentMovementCompleteSignal ( const Manager::VoidSignal                 ::slot_type& slot ) { return m_agentMovementCompleteSignal .connect(slot); }
	Connection ConnectLocalAgentsPresentSignal    ( const Manager::VoidSignal                 ::slot_type& slot ) { return m_localAgentsPresentSignal    .connect(slot); }
	Connection ConnectTeleportRequestSignal       ( const Manager::TeleportRequestSignal      ::slot_type& slot ) { return m_teleportRequestSignal       .connect(slot); }
	Connection ConnectTeleportRequestedSignal     ( const Manager::TeleportRequestedSignal    ::slot_type& slot ) { return m_teleportRequestedSignal     .connect(slot); }
	Connection ConnectTeleportStartSignal         ( const Manager::TeleportStartSignal        ::slot_type& slot ) { return m_teleportStartSignal         .connect(slot); }
	Connection ConnectTeleportProgressSignal      ( const Manager::TeleportProgressSignal     ::slot_type& slot ) { return m_teleportProgressSignal      .connect(slot); }
	Connection ConnectTeleportLocalSignal         ( const Manager::VoidSignal                 ::slot_type& slot ) { return m_teleportLocalSignal         .connect(slot); }
	Connection ConnectTeleportFailedSignal        ( const Manager::TeleportFailedSignal       ::slot_type& slot ) { return m_teleportFailedSignal        .connect(slot); }
	Connection ConnectTeleportFinishSignal        ( const Manager::VoidSignal                 ::slot_type& slot ) { return m_teleportFinishSignal        .connect(slot); }

	void		OnCacheNameCallback( const LLUUID& id, const std::string& firstname, const std::string& lastname, BOOL is_group, void* data );
	void		OnProcessAgentMovementComplete( LLMessageSystem* msg, void **user_data );
	void		OnProcessIMCallback( LLMessageSystem* msg, void **user_data );
	void		OnProcessLocalChat( LLMessageSystem* msg, void **user_data );
	void		OnOnlineNotifyCallback( LLMessageSystem* msg, void **user_data );
	void		OnOfflineNotifyCallback( LLMessageSystem *msg, void **user_data );
	void		OnTerminateFriendshipCallback( LLMessageSystem *msg, void **user_data );
	void		OnSearchResultCallback( LLMessageSystem *msg, void **user_data );
	void		OnKickUserCallback( LLMessageSystem *msg, void **user_data );
	void		OnLogoutCallback( LLMessageSystem* msg, void **user_data );
	void		OnAgentDataUpdate( LLMessageSystem* msg, void **user_data );
	void		OnAgentGroupDataUpdate( LLMessageSystem* msg, void **user_data );
	void		OnAgentWearablesUpdate( LLMessageSystem* msg, void **user_data );
	void		OnAgentCachedTextureResponse( LLMessageSystem* msg, void **user_data );
	void        OnEnableSimulator( LLMessageSystem* msg, void **user_data );
    void        OnDisableSimulator( LLMessageSystem* msg, void **user_data );
	//
	// Teleport stuff
	//
	void		OnMapBlockReply( LLMessageSystem* msg, void **user_data );
	void		OnTeleportStart( LLMessageSystem* msg, void **user_data );
	void		OnTeleportProgress( LLMessageSystem* msg, void **user_data );
	void		OnTeleportFailed( LLMessageSystem* msg, void **user_data );
	void		OnTeleportLocal( LLMessageSystem* msg, void **user_data );
	void		OnTeleportFinish( LLMessageSystem* msg, void **user_data );

private:
	// Forbidden--this is a singleton
	//
	ManagerImpl();
	ManagerImpl( const ManagerImpl& );
	ManagerImpl& operator =( ManagerImpl& );

	static boost::shared_ptr<ManagerImpl>		m_instance;

	bool					m_started;
	const char *			m_user_settings;
	std::string				m_langId;		// Default language for this AV
	LLUserAuth*				m_llua;
	LLUUID					m_agentId;
	LLUUID					m_sessionId;
	LLUUID					m_secureSessionId;
	LLUUID					m_groupId;
	U64						m_groupPowers;
    std::string             m_groupTitle;
	std::string				m_groupName;
    LLViewerRegionPtr       m_viewerRegion;	
	LLVector3				m_agentPosition;
	//
	LLUUID					m_rootInventoryFolder;
	LLUUID					m_searchId;

	LLViewerRegion::LLUUIDList	m_nearAvatars;
	
	typedef boost::shared_ptr<LLWearable>	LLWearablePtr;
	typedef std::map<LLAssetID, LLWearablePtr> LLAssetIdToLLWearable;
	LLAssetIdToLLWearable	m_wearableList;

	typedef std::map<LLUUID, bool>	LLUUIDToBool;
	LLUUIDToBool			m_onlineStatuses;	
	LLUUIDToBool			m_cacheReceivedMap;

	typedef std::map<std::string, LLUUID>	StringToLLUUID;
	StringToLLUUID			m_nameToIdMap;
	
	typedef std::map<LLUUID, std::string>	LLUUIDToString;
	LLUUIDToString			m_idToNameMap;	// Our own cache

	typedef std::map<LLUUID, LLSD>			LLUUIDToLLSD;
	mutable LLUUIDToLLSD	m_agentLang;
    
    struct LLGroupData
    {
	    LLUUID mID;
	    LLUUID mInsigniaID;
	    U64 mPowers;
	    BOOL mAcceptNotices;
	    BOOL mListInProfile;
	    S32 mContribution;
	    std::string mName;

        LLGroupData() : mPowers(0), mAcceptNotices(FALSE), mListInProfile(FALSE), mContribution(0) {}
    };

    typedef std::map<LLUUID,LLGroupData>    LLUUIDToGroupData;
	LLUUIDToGroupData		m_groupMap;

	typedef std::vector<LLUUID>	LLUUIDList;
	LLUUIDList				m_peopleSearchResult;

	std::string					m_fullName;
	bool						m_translateMessages;

	Manager::StringSignal				m_friendAddSignal;
	Manager::CacheSignal				m_cacheSignal;
    Manager::GroupCacheSignal   		m_groupCacheSignal;
	Manager::ImSignal					m_imSignal;
	Manager::GroupChatSignal			m_groupChatSignal;
	Manager::LocalChatSignal			m_localChatSignal;
	Manager::GroupChatAgentUpdateSignal	m_groupChatAgentUpdateSignal;
	Manager::TypingSignal				m_typingSignal;
	Manager::OnlineSignal				m_onlineSignal;
	Manager::StringSignal				m_terminateFriendshipSignal;
	Manager::FriendOfferSignal			m_friendOfferSignal;
	Manager::CacheSignal				m_friendAcceptSignal;
	Manager::CacheSignal				m_friendDeclineSignal;
	Manager::StringSignal				m_messageBoxSignal;
	Manager::SearchResultSignal			m_searchResultSignal;
	Manager::StringSignal				m_forceQuitSignal;
	Manager::VoidSignal					m_logoutReplySignal;
	Manager::GroupOfferSignal			m_groupOfferSignal;
	Manager::VoidSignal					m_agentMovementCompleteSignal;
	Manager::VoidSignal					m_localAgentsPresentSignal;
	//
	Manager::TeleportRequestSignal		m_teleportRequestSignal;
	Manager::TeleportRequestedSignal	m_teleportRequestedSignal;
	Manager::TeleportStartSignal		m_teleportStartSignal;
	Manager::TeleportProgressSignal		m_teleportProgressSignal;
	Manager::VoidSignal					m_teleportLocalSignal;
	Manager::TeleportFailedSignal		m_teleportFailedSignal;
	Manager::VoidSignal					m_teleportFinishSignal;

	Agent								m_agent;
	S32									m_x, m_y, m_z;		// Request for teleport...
	std::string							m_destRegionName;	// Request for teleport...

	void		TeleportToRegion( const U64& region_handle, S32 x, S32 y, S32 z );
	void		HandleCacheUpdate( const LLUUID& id, const std::string fullName, const bool is_group = false );
	void		SendReliable( LLMessageSystem* msg );
	void		SendCompleteAgentMovement( const LLHost& sim_host );

	LLSD GetDetectQuery( const std::string& message );
	LLSD GetTranslateQuery( const std::string& sourceLangId, const std::string& message );
	void HandleGroupChatDispatch( const LLSD& params );
	void HandleGroupChatDetect( const LLSD& params );
	void HandleGroupChatTranslationResponse
									( const LLUUID& group_id
									, const LLUUID& from_id
									, const std::string& group_name
									, const std::string& agent_name
									, const bool has_me
									, const std::string& message
									);
	void HandleIMDispatch( const LLSD& params );
	void HandleIMDetect( const LLSD& params );
	void HandleIMTranslationResponse	( const LLUUID& from_id
										, const std::string& name
										, const bool has_me
										, const std::string& message
										);
	void HandleLocalChatDispatch( const LLSD& params );
	void HandleLocalChatDetect( const LLSD& params );
	void HandleLocalChatTranslationResponse
									( const LLUUID& from_id
									, const std::string& from_name
									, const std::string& verb
									, const bool has_me
									, const std::string& message
									);
};


}
// namespace LLC

#endif // __MANAGERIMPL_H__

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
