#include "Agent.h"
#include <boost/lexical_cast.hpp>

// Local
//
#include "llworld.h"

// llcommon
//
#include "imageids.h"

// llxml
//
#include "llxmltree.h"

// llmath
//
#include "llquantize.h"

using namespace LLVOAvatarDefines;

namespace
{
	LLAssetType::EType TypeToAssetType( EWearableType wearable_type )
	{
		switch( wearable_type )
		{
		case WT_SHAPE:
		case WT_SKIN:
		case WT_HAIR:
		case WT_EYES:
			return LLAssetType::AT_BODYPART;
		case WT_SHIRT:
		case WT_PANTS:
		case WT_SHOES:
		case WT_SOCKS:
		case WT_JACKET:
		case WT_GLOVES:
		case WT_UNDERSHIRT:
		case WT_UNDERPANTS:
		case WT_SKIRT:
			return LLAssetType::AT_CLOTHING;
		default:
			return LLAssetType::AT_NONE;
		}
	}

	const F32 TIMEOUT_SECONDS   = 5.f;
	const S32 MAX_TIMEOUT_COUNT = 3;


	void use_circuit_callback( void**, S32 result )
	{
#if 0
		// TODO: Implement
		//
		// bail if we're quitting.
		if(LLApp::isExiting()) return;
		if( !gUseCircuitCallbackCalled )
		{
			gUseCircuitCallbackCalled = true;
			if (result)
			{
				// Make sure user knows something bad happened. JC
				LL_WARNS("AppInit") << "Backing up to login screen!" << LL_ENDL;
				gViewerWindow->alertXml("LoginPacketNeverReceived",
					login_alert_status, NULL);
				reset_login();
			}
			else
			{
				gGotUseCircuitCodeAck = true;
			}
		}
#endif
	}
}
// namespace


namespace LLC
{


Agent::Agent()
	: m_appearenceSerialNum(0)
	, m_textureCacheQueryID(0)
	, m_numPendingQueries(0)
	, m_wearablesLoaded(false)
	, m_isDummy(false)
	, m_isSelf(true)
{
}


void Agent::SendReliable( LLMessageSystem* msg )
{
	msg->sendReliable(
			gHost,
			MAX_TIMEOUT_COUNT,
			FALSE,
			TIMEOUT_SECONDS,
			use_circuit_callback,
			NULL);
}


void Agent::OnProcessGetAssetReply
	( const char* filename
	, const LLAssetID& uuid
	, void* userdata
	, S32 status
	, LLExtStat ext_status
	)
{
	Agent* agent = static_cast<Agent*>( userdata );

	BOOL isNewWearable = FALSE;
	LLWearablePtr wearable;

	if( !filename )
	{
		LL_WARNS("Wearable") << "Bad Wearable Asset: missing file." << LL_ENDL;
	}
	else if( status >= 0 )
	{
		std::cout	<< "Agent::OnProcessGetAssetReply(): filename="
					<< filename
					<< ", uuid of asset=" 
					<< uuid.asString().c_str()
					<< std::endl;

		// read the file
		LLFILE* fp = LLFile::fopen( std::string(filename), "rb" );		/*Flawfinder: ignore*/
		if( !fp )
		{
			LL_WARNS("Wearable") << "Bad Wearable Asset: unable to open file: '" << filename << "'" << LL_ENDL;
		}
		else
		{
			wearable.reset( new LLWearable(uuid) );
			bool res = wearable->importFile( fp );
			if (!res)
			{
				if (wearable->getType() == WT_COUNT)
				{
					isNewWearable = TRUE;
				}
				wearable.reset();
			}

			fclose( fp );
			if(filename)
			{
				LLFile::remove(std::string(filename));
			}
		}
	}
	else
	{
		if(filename)
		{
			LLFile::remove(std::string(filename));
		}

		LL_WARNS("Wearable") << "Wearable download failed: " << LLAssetStorage::getErrorString( status ) << " " << uuid << LL_ENDL;
		switch( status )
		{
		case LL_ERR_ASSET_REQUEST_NOT_IN_DATABASE:
			{
				// Fail
				break;
			}
		}
	}

	if (wearable.get()) // success
	{
		agent->m_wearableList[ uuid ] = wearable;
		agent->InitialWearableAssetArrived( wearable.get(), wearable->getType() );
	}
	else
	{
		// TODO: What do to on failure?
	}
}


void Agent::QueryWearableCache()
{
	if( !m_wearablesLoaded )
	{
		return;
	}

	LLMessageSystem* msg = LLMessageSystem::getInstance();

	// do onInitialWearableAssetArrived() code here from llagent.cpp
	// must do queryWearableCache()
	//
	msg->newMessageFast(_PREHASH_AgentCachedTexture);
	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID, m_agentId);
	msg->addUUIDFast(_PREHASH_SessionID, m_sessionId);
	msg->addS32Fast(_PREHASH_SerialNum, m_textureCacheQueryID);

	S32 num_queries = 0;
	for (U8 baked_index = 0; baked_index < BAKED_NUM_INDICES; baked_index++ )
	{
		const LLVOAvatarDictionary::WearableDictionaryEntry *wearable_dict = LLVOAvatarDictionary::getInstance()->getWearable((EBakedTextureIndex)baked_index);
		LLUUID hash;
		for (U8 i=0; i < wearable_dict->mWearablesVec.size(); i++)
		{
			// EWearableType wearable_type = gBakedWearableMap[baked_index][wearable_num];
			const EWearableType wearable_type = wearable_dict->mWearablesVec[i];
			const LLWearable* wearable = GetWearable(wearable_type);
			if (wearable)
			{
				hash ^= wearable->getID();
			}
		}
		if (hash.notNull())
		{
			hash ^= wearable_dict->mHashID;
			num_queries++;
			// *NOTE: make sure at least one request gets packed

			//llinfos << "Requesting texture for hash " << hash << " in baked texture slot " << baked_index << llendl;
			msg->nextBlockFast(_PREHASH_WearableData);
			msg->addUUIDFast(_PREHASH_ID, hash);
			msg->addU8Fast(_PREHASH_TextureIndex, (U8)baked_index);
		}

		m_activeCacheQueries[ baked_index ] = m_textureCacheQueryID;
	}

	llinfos << "Requesting texture cache entry for " << num_queries << " baked textures" << llendl;
	SendReliable( msg );
	m_numPendingQueries++;
	m_textureCacheQueryID++;
}


void Agent::AgentWearablesUpdate( LLMessageSystem *msg )
{
	std::cout << "Agent::AgentWearablesUpdate()" << std::endl;

	// We should only receive this message a single time.  Ignore subsequent AgentWearablesUpdates
	// that may result from AgentWearablesRequest having been sent more than once. 
	static bool first = true;
	if (!first) return;
	first = false;

	LLUUID agent_id;
	msg->getUUIDFast(_PREHASH_AgentData, _PREHASH_AgentID, agent_id );

	static U32 agentWearablesUpdateSerialNum = 0;
	if( agent_id == m_agentId )
	{
		msg->getU32Fast(_PREHASH_AgentData, _PREHASH_SerialNum, agentWearablesUpdateSerialNum );

		S32 num_wearables = msg->getNumberOfBlocksFast(_PREHASH_WearableData);
		if( num_wearables < 4 )
		{
			// Transitional state.  Avatars should always have at least their body parts (hair, eyes, shape and skin).
			// The fact that they don't have any here (only a dummy is sent) implies that this account existed
			// before we had wearables, or that the database has gotten messed up.
			return;
		}

		//lldebugs << "processAgentInitialWearablesUpdate()" << llendl;
		// Add wearables
		LLUUID item_id_array[ WT_COUNT ];
		LLUUID asset_id_array[ WT_COUNT ];
		S32 i;
		for( i=0; i < num_wearables; i++ )
		{
			U8 type_u8 = 0;
			msg->getU8Fast(_PREHASH_WearableData, _PREHASH_WearableType, type_u8, i );
			if( type_u8 >= WT_COUNT )
			{
				continue;
			}
			EWearableType type = (EWearableType) type_u8;

			LLUUID item_id;
			msg->getUUIDFast(_PREHASH_WearableData, _PREHASH_ItemID, item_id, i );

			LLUUID asset_id;
			msg->getUUIDFast(_PREHASH_WearableData, _PREHASH_AssetID, asset_id, i );
			//
			std::cout
				<< "ItemId: "
				<< item_id
				<< ", assetId: "
				<< asset_id
				<< std::endl;
			//
			if( asset_id.isNull() )
			{
				//LLWearable::removeFromAvatar( type, FALSE );
				// TODO: deal with this
			}
			else
			{
				asset_id_array[type] = asset_id;
				item_id_array[type]  = item_id;
			}
		}

		// now that we have the asset ids...request the wearable assets
		for( i = 0; i < WT_COUNT; i++ )
		{
			if( !item_id_array[i].isNull() )
			{
				gAssetStorage->getAssetData(
						asset_id_array[i],
						TypeToAssetType( (EWearableType) i ),
						Agent::OnProcessGetAssetReply,
						(void*) this,
						TRUE);
			}
		}
	}
}


void Agent::SendAgentNowWearing()
{
	LLMessageSystem* msg = LLMessageSystem::getInstance();

	// Send the AgentIsNowWearing 
	msg->newMessageFast(_PREHASH_AgentIsNowWearing);

	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID, m_agentId );
	msg->addUUIDFast(_PREHASH_SessionID, m_sessionId );

	std::cout << "sendAgentWearablesUpdate()" << std::endl;
	for(int i=0; i < WT_COUNT; ++i)
	{
		msg->nextBlockFast(_PREHASH_WearableData);

		U8 type_u8 = (U8)i;
		msg->addU8Fast(_PREHASH_WearableType, type_u8 );

		LLWearable* wearable = m_wearableEntry[ i ].mWearable;
		if( wearable )
		{
			//llinfos << "Sending wearable " << wearable->getName() << llendl;
			msg->addUUIDFast(_PREHASH_ItemID, m_wearableEntry[ i ].mItemID );
		}
		else
		{
			//llinfos << "Not wearing wearable type " << LLWearable::typeToTypeName((EWearableType)i) << llendl;
			msg->addUUIDFast(_PREHASH_ItemID, LLUUID::null );
		}

		std::cout
			<< "       "
			<< LLWearable::typeToTypeLabel((EWearableType)i)
			<< ": "
			<< (wearable ? wearable->getID() : LLUUID::null)
			<< std::endl;
	}
	//
	SendReliable( msg );
}


//-----------------------------------------------------------------------------
// getPosAgentFromGlobal()
//-----------------------------------------------------------------------------
LLVector3 Agent::getPosAgentFromGlobal(const LLVector3d &pos_global) const
{
	LLVector3 pos_agent;
	pos_agent.setVec(pos_global - m_agentOriginGlobal);
	return pos_agent;
}


//-----------------------------------------------------------------------------
// getPosGlobalFromAgent()
//-----------------------------------------------------------------------------
LLVector3d Agent::getPosGlobalFromAgent(const LLVector3 &pos_agent) const
{
	LLVector3d pos_agent_d;
	pos_agent_d.setVec(pos_agent);
	return pos_agent_d + m_agentOriginGlobal;
}


//-----------------------------------------------------------------------------
// getPositionGlobal()
//-----------------------------------------------------------------------------
const LLVector3d& Agent::getPositionGlobal() const
{
#if 0
	if (mAvatarObject.notNull() && !mAvatarObject->mDrawable.isNull())
	{
		m_positionGlobal = getPosGlobalFromAgent(mAvatarObject->getRenderPosition());
	}
	else
	{
#endif
		m_positionGlobal = getPosGlobalFromAgent( LLVector3() ); //m_frameAgent.getOrigin() );
	//}

	return m_positionGlobal;
}


void Agent::setRegion( LLViewerRegionPtr regionp )
{
	llassert(regionp);
	if (m_region != regionp)
	{
		m_avatarObject->setRegion( regionp );

		// std::string host_name;
		// host_name = regionp->getHost().getHostName();

		std::string ip = regionp->getHost().getString();
		llinfos << "Moving agent into region: " << regionp->getName()
				<< " located at " << ip << llendl;
		if (m_region)
		{
			// We've changed regions, we're now going to change our agent coordinate frame.
			m_agentOriginGlobal = regionp->getOriginGlobal();
			//m_frameAgent.setOrigin( getPosAgentFromGlobal(m_agentOriginGlobal) );
			LLVector3 pos;
			pos.setVec( m_agentOriginGlobal );
			m_frameAgent.setOrigin( pos );
			//
			LLVector3d agent_offset_global = m_region->getOriginGlobal();

			LLVector3 delta;
			delta.setVec(regionp->getOriginGlobal() - m_region->getOriginGlobal());

			setPositionAgent(getPositionAgent() - delta);

#if 0
			LLVector3 camera_position_agent = LLViewerCamera::getInstance()->getOrigin();
			LLViewerCamera::getInstance()->setOrigin(camera_position_agent - delta);

			// Update all of the regions.
			LLWorld::getInstance()->updateAgentOffset(agent_offset_global);

			// Hack to keep sky in the agent's region, otherwise it may get deleted - DJS 08/02/02
			// *TODO: possibly refactor into gSky->setAgentRegion(regionp)? -Brad
			if (gSky.mVOSkyp)
			{
				gSky.mVOSkyp->setRegion(regionp);
			}
			if (gSky.mVOGroundp)
			{
				gSky.mVOGroundp->setRegion(regionp);
			}
#endif

		}
		else
		{
			// First time initialization.
			// We've changed regions, we're now going to change our agent coordinate frame.
			m_agentOriginGlobal = regionp->getOriginGlobal();
			//m_frameAgent.setOrigin( getPosAgentFromGlobal(m_agentOriginGlobal) );
			LLVector3 pos;
			pos.setVec( m_agentOriginGlobal );
			m_frameAgent.setOrigin( pos );

			LLVector3 delta;
			delta.setVec(regionp->getOriginGlobal());

			setPositionAgent(getPositionAgent() - delta);
#if 0
			LLVector3 camera_position_agent = LLViewerCamera::getInstance()->getOrigin();
			LLViewerCamera::getInstance()->setOrigin(camera_position_agent - delta);
#endif
			// Update all of the regions.
			LLWorld::getInstance()->updateAgentOffset(m_agentOriginGlobal);
		}
	}
	m_region = regionp;

	// Must shift hole-covering water object locations because local
	// coordinate frame changed.
	//LLWorld::getInstance()->updateWaterObjects();

#if 0
	// keep a list of regions we've been too
	// this is just an interesting stat, logged at the dataserver
	// we could trake this at the dataserver side, but that's harder
	U64 handle = regionp->getHandle();
	mRegionsVisited.insert(handle);

	LLSelectMgr::getInstance()->updateSelectionCenter();
#endif
}


//-----------------------------------------------------------------------------
// getPositionAgent()
//-----------------------------------------------------------------------------
#if 0
const LLVector3 &Agent::getPositionAgent()
{
	if(m_avatarObject.get() ) //&& !m_avatarObject->mDrawable.isNull())
	{
		m_frameAgent.setOrigin(m_avatarObject->getRenderPosition());	
	}

	return LLVector3();
}
#endif


void debug_output_wearbletype( EWearableType type )
{
	char* name = (char*) "";
	switch( type )
	{
		case WT_SHAPE		: name = (char*) "WT_SHAPE"			; break;
		case WT_SKIN		: name = (char*) "WT_SKIN"			; break;
		case WT_HAIR		: name = (char*) "WT_HAIR"			; break;
		case WT_EYES		: name = (char*) "WT_EYES"			; break;
		case WT_SHIRT		: name = (char*) "WT_SHIRT"			; break;
		case WT_PANTS		: name = (char*) "WT_PANTS"			; break;
		case WT_SHOES		: name = (char*) "WT_SHOES"			; break;
		case WT_SOCKS		: name = (char*) "WT_SOCKS"			; break;
		case WT_JACKET		: name = (char*) "WT_JACKET"		; break;
		case WT_GLOVES		: name = (char*) "WT_GLOVES"		; break;
		case WT_UNDERSHIRT	: name = (char*) "WT_UNDERSHIRT"	; break;
		case WT_UNDERPANTS	: name = (char*) "WT_UNDERPANTS"	; break;
		case WT_SKIRT		: name = (char*) "WT_SKIRT"			; break;
		case WT_COUNT		: name = (char*) "WT_COUNT"			; break;
		case WT_INVALID		: name = (char*) "WT_INVALID"		; break;
	}

	std::cout << name << std::endl;
}

void Agent::InitialWearableAssetArrived( LLWearable* wearable, EWearableType type )
{
	if( wearable )
	{
		std::cout << "Agent::InitialWearableAssetArrived(): type=" << std::endl;
		debug_output_wearbletype( type );

		llassert( type == wearable->getType() );
		m_wearableEntry[ type ].mWearable = wearable;

		// disable composites if initial textures are baked
		//m_avatarObject->setupComposites();
		QueryWearableCache();

		//wearable->writeToAvatar( FALSE );
		//m_avatarObject->setCompositeUpdatesEnabled(TRUE);
		//gInventory.addChangedMask( LLInventoryObserver::LABEL, gAgent.mWearableEntry[type].mItemID );
	}
	else
	{
		// Somehow the asset doesn't exist in the database.
		//gAgent.recoverMissingWearable( type );
	}

	//gInventory.notifyObservers();

	// Have all the wearables that the avatar was wearing at log-in arrived?
	if( !m_wearablesLoaded )
	{
		m_wearablesLoaded = TRUE;
		for( S32 i = 0; i < WT_COUNT; i++ )
		{
			if( !m_wearableEntry[i].mItemID.isNull() && !m_wearableEntry[i].mWearable )
			{
				m_wearablesLoaded = FALSE;
				break;
			}
		}
	}

	if( m_wearablesLoaded )
	{
		// Make sure that the server's idea of the avatar's wearables actually match the wearables.
		SendAgentSetAppearance();

#if 0
		// Check to see if there are any baked textures that we hadn't uploaded before we logged off last time.
		// If there are any, schedule them to be uploaded as soon as the layer textures they depend on arrive.
		if( !gAgent.cameraCustomizeAvatar() )
		{
			m_avatarObject->requestLayerSetUploads();
		}
#endif
	}
}


void Agent::SendAgentSetAppearance()
{
#if 1
	LLMessageSystem* msg = LLMessageSystem::getInstance();

	// Then do SetAppearance below
	msg->newMessageFast(_PREHASH_AgentSetAppearance);
	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID, m_agentId );
	msg->addUUIDFast(_PREHASH_SessionID, m_sessionId );

	// To guard against out of order packets
	// Note: always start by sending 1.  This resets the server's count. 0 on the server means "uninitialized"
	msg->addU32Fast(_PREHASH_SerialNum, ++m_appearenceSerialNum );

	// correct for the collision tolerance (to make it look like the 
	// agent is actually walking on the ground/object)
	// NOTE -- when we start correcting all of the other Havok geometry 
	// to compensate for the COLLISION_TOLERANCE ugliness we will have 
	// to tweak this number again
	const LLVector3 body_size = m_avatarObject->mBodySize;
	msg->addVector3Fast(_PREHASH_Size, body_size);	

	llinfos << "TAT: Sending cached texture data" << llendl;
	for (U8 baked_index = 0; baked_index < BAKED_NUM_INDICES; baked_index++)
	{
		const LLVOAvatarDictionary::WearableDictionaryEntry *wearable_dict = LLVOAvatarDictionary::getInstance()->getWearable((EBakedTextureIndex)baked_index);
		LLUUID hash;
		for (U8 i=0; i < wearable_dict->mWearablesVec.size(); i++)
		{
			// EWearableType wearable_type = gBakedWearableMap[baked_index][wearable_num];
			const EWearableType wearable_type = wearable_dict->mWearablesVec[i];
			const LLWearable* wearable = GetWearable(wearable_type);
			if (wearable)
			{
				hash ^= wearable->getID();
			}
		}
		if (hash.notNull())
		{
			hash ^= wearable_dict->mHashID;
		}

		const ETextureIndex texture_index = getTextureIndex((EBakedTextureIndex)baked_index);

		msg->nextBlockFast(_PREHASH_WearableData);
		msg->addUUIDFast(_PREHASH_CacheID, hash);
		msg->addU8Fast(_PREHASH_TextureIndex, (U8)texture_index);
	}
	// TODO: this is probably important....
	msg->nextBlockFast(_PREHASH_ObjectData);
	m_avatarObject->packTEMessage( msg );

	S32 transmitted_params = 0;
	for (LLVisualParam* param = m_avatarObject->getFirstVisualParam();
		param;
		param = m_avatarObject->getNextVisualParam())
	{
		if (param->getGroup() == VISUAL_PARAM_GROUP_TWEAKABLE)
		{
			msg->nextBlockFast(_PREHASH_VisualParam );

			// We don't send the param ids.  Instead, we assume that the receiver has the same params in the same sequence.
			const F32 param_value = param->getWeight();
			const U8 new_weight = F32_to_U8(param_value, param->getMinWeight(), param->getMaxWeight());
			msg->addU8Fast(_PREHASH_ParamValue, new_weight );
			transmitted_params++;
		}
	}

	//	llinfos << "Avatar XML num VisualParams transmitted = " << transmitted_params << llendl;
	//sendReliableMessage();
	SendReliable( msg );
#endif
}


void Agent::AgentCachedTextureResponse( LLMessageSystem* msg )
{
#if 1
	// Then do Agent::processAgentCachedTextureResponse()
	//
	m_numPendingQueries--;

	S32 query_id;
	msg->getS32Fast(_PREHASH_AgentData, _PREHASH_SerialNum, query_id);

	S32 num_texture_blocks = msg->getNumberOfBlocksFast(_PREHASH_WearableData);


	S32 num_results = 0;
	for (S32 texture_block = 0; texture_block < num_texture_blocks; texture_block++)
	{
		LLUUID texture_id;
		U8 texture_index;

		msg->getUUIDFast(_PREHASH_WearableData, _PREHASH_TextureID, texture_id, texture_block);
		msg->getU8Fast(_PREHASH_WearableData, _PREHASH_TextureIndex, texture_index, texture_block);

		if (texture_id.notNull() 
			&& (S32)texture_index < BAKED_NUM_INDICES 
			&& m_activeCacheQueries[ texture_index ] == query_id
			)
		{
			llinfos << "Received cached texture " << (U32)texture_index << ": " << texture_id << llendl;
#if 0
			m_avatarObject->setCachedBakedTexture(getTextureIndex((EBakedTextureIndex)texture_index), texture_id);
			m_avatarObject->setTETexture( LLVOAvatar::sBakedTextureIndices[texture_index], texture_id );
			gAgent.m_activeCacheQueries[ texture_index ] = 0;
#endif
			num_results++;
		}
	}

	llinfos << "Received cached texture response for " << num_results << " textures." << llendl;

#if 0
	m_avatarObject->updateMeshTextures();
#endif

	if (m_numPendingQueries == 0)
	{
		// RN: not sure why composites are disabled at this point
#if 0
		m_avatarObject->setCompositeUpdatesEnabled(TRUE);
#endif
		SendAgentSetAppearance();
	}
#endif

} 


const LLVector3& Agent::getPositionAgent()
{
#if 0
	if(mAvatarObject.notNull() && !mAvatarObject->mDrawable.isNull())
	{
		m_frameAgent.setOrigin(mAvatarObject->getRenderPosition());	
	}
#endif

	return m_frameAgent.getOrigin();
}


void Agent::setPositionAgent(const LLVector3 &pos_agent)
{
	if (!pos_agent.isFinite())
	{
		llerrs << "setPositionAgent is not a number" << llendl;
	}

#if 0
	if( m_avatarObject && m_avatarObject->getParent() )
	{
		LLVector3 pos_agent_sitting;
		LLVector3d pos_agent_d;
		LLViewerObject *parent = (LLViewerObject*)m_avatarObject->getParent();

		pos_agent_sitting = m_avatarObject->getPosition() * parent->getRotation() + parent->getPositionAgent();
		pos_agent_d.setVec(pos_agent_sitting);

		m_frameAgent.setOrigin(pos_agent_sitting);
		m_positionGlobal = pos_agent_d + m_agentOriginGlobal;
	}
	else
#endif
	{
		m_frameAgent.setOrigin(pos_agent);

		LLVector3d pos_agent_d;
		pos_agent_d.setVec(pos_agent);
		m_positionGlobal = pos_agent_d + m_agentOriginGlobal;
	}
}


}
//namespace LLC


// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
