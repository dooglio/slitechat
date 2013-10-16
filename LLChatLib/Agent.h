#ifndef AGENT_H
#define AGENT_H

#include "stdtypes.h"
#include "lluserauth.h"
#include "lluuid.h"
#include "message.h"
#include "lljoint.h"
#include "llwearable.h"
#include "lluuid.h"
#include "llvoavatardefines.h"
#include "llvoavatar.h"
#include "llviewerregion.h"

// llcharacter
//
#include "llcharacter.h"

// llmath
//
#include "v3math.h"
#include "v4color.h"
#include "v3dmath.h"
#include "llcoordframe.h"

#include <boost/smart_ptr.hpp>

class LLVOAvatar;

using namespace LLVOAvatarDefines;

namespace LLC
{


class Agent
{
public:
	Agent();
	virtual ~Agent() {}

	void		SetAgentId  ( const LLUUID& id )	{ m_agentId   = id; }
	void		SetSessionId( const LLUUID& id )	{ m_sessionId = id; }

	void		AgentWearablesUpdate( LLMessageSystem* msg );
	void		AgentCachedTextureResponse( LLMessageSystem* msg );

	static void SendReliable( LLMessageSystem* msg );
	//
	LLVector3			getPosAgentFromGlobal(const LLVector3d &pos_global) const;
	LLVector3d			getPosGlobalFromAgent(const LLVector3 &pos_agent) const;
	const LLVector3d&	getPositionGlobal() const;


	void				SetAvatar( LLVOAvatar* av ) { m_avatarObject.reset(av); }
	void				setRegion( LLViewerRegionPtr regionp );

	const LLVector3&	getPositionAgent();
	void				setPositionAgent(const LLVector3 &pos_agent);

private:
	LLUUID							m_agentId;
	LLUUID							m_sessionId;
	LLJoint							m_jointsRoot;
	LLViewerRegionPtr				m_region;
	U32								m_appearenceSerialNum;
	U32								m_textureCacheQueryID;
	U32								m_numPendingQueries;
	typedef std::map<U8,S32>		U82S32;
	U82S32							m_activeCacheQueries;
	bool							m_wearablesLoaded;
	bool							m_isDummy;
	bool							m_isSelf;
	LLVector3d						m_agentOriginGlobal;				// Origin of agent coords from global coords
	mutable LLVector3d				m_positionGlobal;
	LLCoordFrame					m_frameAgent;					// Agent position and view, agent-region coordinates

	typedef boost::shared_ptr<LLVOAvatar>	LLVOAvatarPtr;
	LLVOAvatarPtr					m_avatarObject;

	typedef boost::shared_ptr<LLWearable>	LLWearablePtr;
	typedef std::map< LLAssetID, LLWearablePtr > LLAssetIdToLLWearable;
	LLAssetIdToLLWearable	m_wearableList;

	struct LLWearableEntry
	{
		LLWearableEntry() : mItemID( LLUUID::null ), mWearable( NULL ) {}

		LLUUID		mItemID;	// ID of the inventory item in the agent's inventory.
		LLWearable*	mWearable;
	};
	LLWearableEntry m_wearableEntry[ WT_COUNT ];

	LLWearable*	GetWearable( EWearableType type ) { return (type < WT_COUNT) ? m_wearableEntry[ type ].mWearable : NULL; }
	static void	OnProcessGetAssetReply( const char* filename, const LLAssetID& uuid, void* userdata, S32 status, LLExtStat ext_status );
	void		QueryWearableCache();
	void		InitialWearableAssetArrived( LLWearable* wearable, EWearableType type );
	void		SendAgentSetAppearence();
	bool		HasPendingBakedUploads();
	void		SendAgentNowWearing();
	//bool		IsTextureDefined( U8 texture_id );
	//bool		IsWearingWearableType( EWearableType type );
	void		SendAgentSetAppearance();
};


}
//namespace LLC


#endif // AGENT_H

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
