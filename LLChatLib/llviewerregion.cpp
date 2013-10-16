/** 
 * @file llviewerregion.cpp
 * @brief Implementation of the LLViewerRegion class.
 *
 * $LicenseInfo:firstyear=2000&license=viewergpl$
 * 
 * Copyright (c) 2000-2009, Linden Research, Inc.
 * 
 * Second Life Viewer Source Code
 * The source code in this file ("Source Code") is provided by Linden Lab
 * to you under the terms of the GNU General Public License, version 2.0
 * ("GPL"), unless you have obtained a separate licensing agreement
 * ("Other License"), formally executed by you and Linden Lab.  Terms of
 * the GPL can be found in doc/GPL-license.txt in this distribution, or
 * online at http://secondlifegrid.net/programs/open_source/licensing/gplv2
 * 
 * There are special exceptions to the terms and conditions of the GPL as
 * it is applied to this Source Code. View the full text of the exception
 * in the file doc/FLOSS-exception.txt in this software distribution, or
 * online at
 * http://secondlifegrid.net/programs/open_source/licensing/flossexception
 * 
 * By copying, modifying or distributing this software, you acknowledge
 * that you have read and understood your obligations described above,
 * and agree to abide by those obligations.
 * 
 * ALL LINDEN LAB SOURCE CODE IS PROVIDED "AS IS." LINDEN LAB MAKES NO
 * WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
 * COMPLETENESS OR PERFORMANCE.
 * $/LicenseInfo$
 */

// local include
//
#include "llviewerregion.h"
#include "Agent.h"
#include "llworld.h"
#include "ManagerImpl.h"

// llmessage
//
#include "message.h"
#include "llhttpclient.h"
#include "llhttpsender.h"
#include "llcircuit.h"
#include "llregionhandle.h"

#include "ManagerImpl.h"

class LLCapHTTPSender : public LLHTTPSender
{
public:
	LLCapHTTPSender(const std::string& cap);

	/** @brief Send message via UntrustedMessage capability with body,
		call response when done */ 
	virtual void send(const LLHost& host, 
					  const std::string& message, const LLSD& body, 
					  LLHTTPClient::ResponderPtr response) const;

private:
	std::string mCap;
};

LLCapHTTPSender::LLCapHTTPSender(const std::string& cap) :
	mCap(cap)
{
}

//virtual 
void LLCapHTTPSender::send(const LLHost& host, const std::string& message, 
								  const LLSD& body, 
								  LLHTTPClient::ResponderPtr response) const
{
	//llinfos << "LLCapHTTPSender::send: message " << message
	//		<< " to host " << host << llendl;
	LLSD llsd;
	llsd["message"] = message;
	llsd["body"] = body;
	LLHTTPClient::post(mCap, llsd, response);
}


class BaseCapabilitiesComplete : public LLHTTPClient::Responder
{
	LOG_CLASS(BaseCapabilitiesComplete);
public:
    BaseCapabilitiesComplete(LLViewerRegion* region)
		: mRegion(region)
    { }
	virtual ~BaseCapabilitiesComplete()
	{
		if(mRegion)
		{
			mRegion->setHttpResponderPtrNULL() ;
		}
	}

	void setRegion(LLViewerRegion* region)
	{
		mRegion = region ;
	}

    void error(U32 statusNum, const std::string& reason)
    {
        // TODO
    }

    void result(const LLSD& content)
    {
		if(!mRegion || LLHTTPClient::ResponderPtr(this) != mRegion->getHttpResponderPtr()) //region is removed or responder is not created.
		{
			return ;
		}

		LLSD::map_const_iterator iter;
		for(iter = content.beginMap(); iter != content.endMap(); ++iter)
		{
			mRegion->setCapability( iter->first, iter->second );
		}
	}

    static boost::intrusive_ptr<BaseCapabilitiesComplete> build(
								LLViewerRegion* region)
    {
		return boost::intrusive_ptr<BaseCapabilitiesComplete>(
							 new BaseCapabilitiesComplete(region));
    }

private:
	LLViewerRegion* mRegion;
};


LLViewerRegion::LLViewerRegion( U64 regionHandle
		, const LLHost &host
		, const U32 surface_grid_width
		, const U32 patch_grid_width
		, const F32 region_width_meters
		)
    : m_regionHandle(regionHandle)
    , m_host(host)
	, mWidth(region_width_meters)
{	
	mOriginGlobal = from_region_handle(regionHandle); 

	initStats();
}


LLViewerRegion::~LLViewerRegion() 
{
	m_eventPoll.reset();

	if( m_httpResponderPtr )
	{
		(static_cast<BaseCapabilitiesComplete*>(m_httpResponderPtr.get()))->setRegion(NULL);
	}
}

void LLViewerRegion::setSeedCapability(const std::string& url)
{
  if (getCapability("Seed") == url)
    {
      llwarns << "Ignoring duplicate seed capability" << llendl;
      return;
    }
	m_eventPoll.reset();
	
	m_capabilities.clear();
	setCapability("Seed", url);

	LLSD capabilityNames = LLSD::emptyArray();
	capabilityNames.append("ChatSessionRequest");
	capabilityNames.append("CopyInventoryFromNotecard");
	capabilityNames.append("DispatchRegionInfo");
	capabilityNames.append("EstateChangeInfo");
	capabilityNames.append("EventQueueGet");
	capabilityNames.append("FetchInventory");
	capabilityNames.append("WebFetchInventoryDescendents");
	capabilityNames.append("FetchLib");
	capabilityNames.append("FetchLibDescendents");
	capabilityNames.append("GroupProposalBallot");
	capabilityNames.append("HomeLocation");
	capabilityNames.append("MapLayer");
	capabilityNames.append("MapLayerGod");
	capabilityNames.append("NewFileAgentInventory");
	capabilityNames.append("ParcelPropertiesUpdate");
	capabilityNames.append("ParcelVoiceInfoRequest");
	capabilityNames.append("ProvisionVoiceAccountRequest");
	capabilityNames.append("RemoteParcelRequest");
	capabilityNames.append("RequestTextureDownload");
	capabilityNames.append("SearchStatRequest");
	capabilityNames.append("SearchStatTracking");
	capabilityNames.append("SendPostcard");
	capabilityNames.append("SendUserReport");
	capabilityNames.append("SendUserReportWithScreenshot");
	capabilityNames.append("ServerReleaseNotes");
	capabilityNames.append("StartGroupProposal");
	capabilityNames.append("UntrustedSimulatorMessage");
	capabilityNames.append("UpdateAgentLanguage");
	capabilityNames.append("UpdateGestureAgentInventory");
	capabilityNames.append("UpdateNotecardAgentInventory");
	capabilityNames.append("UpdateScriptAgent");
	capabilityNames.append("UpdateGestureTaskInventory");
	capabilityNames.append("UpdateNotecardTaskInventory");
	capabilityNames.append("UpdateScriptTask");
	capabilityNames.append("UploadBakedTexture");
	capabilityNames.append("ViewerStartAuction");
	capabilityNames.append("ViewerStats");
	// Please add new capabilities alphabetically to reduce
	// merge conflicts.

	llinfos << "posting to seed " << url << llendl;

	m_httpResponderPtr = BaseCapabilitiesComplete::build(this) ;
	LLHTTPClient::post( url, capabilityNames, m_httpResponderPtr );
}

void LLViewerRegion::setCapability(const std::string& name, const std::string& url)
{
	if(name == "EventQueueGet")
	{
		m_eventPoll.reset( new LLEventPoll( url, getHost() ) );
	}
	else if(name == "UntrustedSimulatorMessage")
	{
		LLHTTPSender::setSender( m_host, new LLCapHTTPSender(url) );
	}
	else
	{
		m_capabilities[name] = url;
	}
}

bool LLViewerRegion::isSpecialCapabilityName(const std::string &name)
{
	return name == "EventQueueGet" || name == "UntrustedSimulatorMessage";
}

std::string LLViewerRegion::getCapability(const std::string& name) const
{
	CapabilityMap::const_iterator iter = m_capabilities.find(name);
	if(iter == m_capabilities.end())
	{
		return "";
	}
	return iter->second;
}


void LLViewerRegion::initStats()
{
	mLastNetUpdate.reset();
	mPacketsIn = 0;
	mBitsIn = 0;
	mLastBitsIn = 0;
	mLastPacketsIn = 0;
	mPacketsOut = 0;
	mLastPacketsOut = 0;
	mPacketsLost = 0;
	mLastPacketsLost = 0;
	mPingDelay = 0;
	mAlive = false;					// can become false if circuit disconnects
}

void LLViewerRegion::updateNetStats()
{
	F32 dt = mLastNetUpdate.getElapsedTimeAndResetF32();

	LLCircuitData *cdp = LLMessageSystem::getInstance()->mCircuitInfo.findCircuit( m_host );
	if (!cdp)
	{
		mAlive = false;
		return;
	}

	mAlive = true;
	mDeltaTime = dt;

	mLastPacketsIn =	mPacketsIn;
	mLastBitsIn =		mBitsIn;
	mLastPacketsOut =	mPacketsOut;
	mLastPacketsLost =	mPacketsLost;

	mPacketsIn =				cdp->getPacketsIn();
	mBitsIn =					8 * cdp->getBytesIn();
	mPacketsOut =				cdp->getPacketsOut();
	mPacketsLost =				cdp->getPacketsLost();
	mPingDelay =				cdp->getPingDelay();

	mBitStat.addValue(mBitsIn - mLastBitsIn);
	mPacketsStat.addValue(mPacketsIn - mLastPacketsIn);
	mPacketsLostStat.addValue(mPacketsLost);
}

bool LLViewerRegion::isAlive()
{
	return mAlive;
}


void LLViewerRegion::setRegionNameAndZone	(const std::string& name_zone)
{
	std::string::size_type pipe_pos = name_zone.find('|');
	S32 length   = name_zone.size();
	if (pipe_pos != std::string::npos)
	{
		mName   = name_zone.substr(0, pipe_pos);
		mZoning = name_zone.substr(pipe_pos+1, length-(pipe_pos+1));
	}
	else
	{
		mName   = name_zone;
		mZoning = "";
	}

	LLStringUtil::stripNonprintable(mName);
	LLStringUtil::stripNonprintable(mZoning);
}


void LLViewerRegion::getInfo(LLSD& info)
{
	info["Region"]["Host"] = getHost().getIPandPort();
	info["Region"]["Name"] = getName();
	U32 x, y;
	from_region_handle(getHandle(), &x, &y);
	info["Region"]["Handle"]["x"] = (LLSD::Integer)x;
	info["Region"]["Handle"]["y"] = (LLSD::Integer)y;
}


void LLViewerRegion::unpackRegionHandshake()
{
	LLMessageSystem *msg = gMessageSystem;

	U32 region_flags;
	U8 sim_access;
	std::string sim_name;
	LLUUID sim_owner;
	BOOL is_estate_manager;
	F32 water_height;
	F32 billable_factor;
	LLUUID cache_id;

	msg->getU32		("RegionInfo", "RegionFlags", region_flags);
	msg->getU8		("RegionInfo", "SimAccess", sim_access);
	msg->getString	("RegionInfo", "SimName", sim_name);
	msg->getUUID	("RegionInfo", "SimOwner", sim_owner);
	msg->getBOOL	("RegionInfo", "IsEstateManager", is_estate_manager);
	msg->getF32		("RegionInfo", "WaterHeight", water_height);
	msg->getF32		("RegionInfo", "BillableFactor", billable_factor);
	msg->getUUID	("RegionInfo", "CacheID", cache_id );

#if 0
	setRegionFlags(region_flags);
	setSimAccess(sim_access);
	setOwner(sim_owner);
	setIsEstateManager(is_estate_manager);
	setWaterHeight(water_height);
	setBillableFactor(billable_factor);
	setCacheID(cache_id);
#endif

	setRegionNameAndZone(sim_name);

	LLUUID region_id;
	msg->getUUID("RegionInfo2", "RegionID", region_id);
//	setRegionID(region_id);

#if 0
	LLVLComposition *compp = getComposition();
	if (compp)
	{
		LLUUID tmp_id;

		msg->getUUID("RegionInfo", "TerrainDetail0", tmp_id);
		compp->setDetailTextureID(0, tmp_id);
		msg->getUUID("RegionInfo", "TerrainDetail1", tmp_id);
		compp->setDetailTextureID(1, tmp_id);
		msg->getUUID("RegionInfo", "TerrainDetail2", tmp_id);
		compp->setDetailTextureID(2, tmp_id);
		msg->getUUID("RegionInfo", "TerrainDetail3", tmp_id);
		compp->setDetailTextureID(3, tmp_id);

		F32 tmp_f32;
		msg->getF32("RegionInfo", "TerrainStartHeight00", tmp_f32);
		compp->setStartHeight(0, tmp_f32);
		msg->getF32("RegionInfo", "TerrainStartHeight01", tmp_f32);
		compp->setStartHeight(1, tmp_f32);
		msg->getF32("RegionInfo", "TerrainStartHeight10", tmp_f32);
		compp->setStartHeight(2, tmp_f32);
		msg->getF32("RegionInfo", "TerrainStartHeight11", tmp_f32);
		compp->setStartHeight(3, tmp_f32);

		msg->getF32("RegionInfo", "TerrainHeightRange00", tmp_f32);
		compp->setHeightRange(0, tmp_f32);
		msg->getF32("RegionInfo", "TerrainHeightRange01", tmp_f32);
		compp->setHeightRange(1, tmp_f32);
		msg->getF32("RegionInfo", "TerrainHeightRange10", tmp_f32);
		compp->setHeightRange(2, tmp_f32);
		msg->getF32("RegionInfo", "TerrainHeightRange11", tmp_f32);
		compp->setHeightRange(3, tmp_f32);

		// If this is an UPDATE (params already ready, we need to regenerate
		// all of our terrain stuff, by
		if (compp->getParamsReady())
		{
			//getLand().dirtyAllPatches();
		}
		else
		{
			compp->setParamsReady();
		}
	}
#endif

	// Now that we have the name, we can load the cache file
	// off disk.
	//loadCache();

	// After loading cache, signal that simulator can start
	// sending data.
	// TODO: Send all upstream viewer->sim handshake info here.
	LLHost host = msg->getSender();
	msg->newMessageFast( _PREHASH_RegionHandshakeReply );
	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID, LLC::ManagerImpl::GetInstance()->GetAgentId());
	msg->addUUIDFast(_PREHASH_SessionID, LLC::ManagerImpl::GetInstance()->GetSessionId());
	msg->nextBlockFast(_PREHASH_RegionInfo);
	msg->addU32Fast(_PREHASH_Flags, 0x0 );
	msg->sendReliable(host);
}

// the new TCP coarse location handler node
class CoarseLocationUpdate : public LLHTTPNode
{
public:
	virtual void post(
		ResponsePtr responder,
		const LLSD& context,
		const LLSD& input) const
	{
		LLHost host(input["sender"].asString());
		LLViewerRegionPtr region = LLWorld::getInstance()->getRegion(host);
		if( !region )
		{
			return;
		}

		S32 target_index = input["body"]["Index"][0]["Prey"].asInteger();
		S32 you_index    = input["body"]["Index"][0]["You" ].asInteger();

		LLDynamicArray<U32>* avatar_locs = &region->mMapAvatars;
		LLDynamicArray<LLUUID>* avatar_ids = &region->mMapAvatarIDs;
		avatar_locs->reset();
		avatar_ids->reset();

		//llinfos << "coarse locations agent[0] " << input["body"]["AgentData"][0]["AgentID"].asUUID() << llendl;
		//llinfos << "my agent id = " << gAgent.getID() << llendl;
		//llinfos << ll_pretty_print_sd(input) << llendl;

		LLSD 
			locs   = input["body"]["Location"],
			agents = input["body"]["AgentData"];
		LLSD::array_iterator 
			locs_it = locs.beginArray(), 
			agents_it = agents.beginArray();
		BOOL has_agent_data = input["body"].has("AgentData");

		for(int i=0; 
			locs_it != locs.endArray(); 
			i++, locs_it++)
		{
			U8 
				x = locs_it->get("X").asInteger(),
				y = locs_it->get("Y").asInteger(),
				z = locs_it->get("Z").asInteger();
			// treat the target specially for the map, and don't add you or the target
			if(i == target_index)
			{
#if 0
				LLVector3d global_pos(region->getOriginGlobal());
				global_pos.mdV[VX] += (F64)x;
				global_pos.mdV[VY] += (F64)y;
				global_pos.mdV[VZ] += (F64)z * 4.0;
				LLAvatarTracker::instance().setTrackedCoarseLocation(global_pos);
#endif
			}
			else if( i != you_index)
			{
				U32 loc = x << 16 | y << 8 | z; loc = loc;
				U32 pos = 0x0;
				pos |= x;
				pos <<= 8;
				pos |= y;
				pos <<= 8;
				pos |= z;
				avatar_locs->put(pos);
				//llinfos << "next pos: " << x << "," << y << "," << z << ": " << pos << llendl;
				if(has_agent_data) // for backwards compatibility with old message format
				{
					LLUUID agent_id(agents_it->get("AgentID").asUUID());
					//llinfos << "next agent: " << agent_id.asString() << llendl;
					avatar_ids->put(agent_id);
				}
			}
			if (has_agent_data)
			{
				agents_it++;
			}
		}
	}
};

// build the coarse location HTTP node under the "/message" URL
LLHTTPRegistration<CoarseLocationUpdate>
   gHTTPRegistrationCoarseLocationUpdate(
	   "/message/CoarseLocationUpdate");

// the deprecated coarse location handler
void LLViewerRegion::updateCoarseLocations(LLMessageSystem* msg)
{
	//llinfos << "CoarseLocationUpdate" << llendl;
	mMapAvatars.reset();
	mMapAvatarIDs.reset(); // only matters in a rare case but it's good to be safe.

	U8 x_pos = 0;
	U8 y_pos = 0;
	U8 z_pos = 0;

	U32 pos = 0x0;

	S16 agent_index;
	S16 target_index;
	msg->getS16Fast(_PREHASH_Index, _PREHASH_You, agent_index);
	msg->getS16Fast(_PREHASH_Index, _PREHASH_Prey, target_index);

	BOOL has_agent_data = msg->has(_PREHASH_AgentData);
	S32 count = msg->getNumberOfBlocksFast(_PREHASH_Location);
	for(S32 i = 0; i < count; i++)
	{
		msg->getU8Fast(_PREHASH_Location, _PREHASH_X, x_pos, i);
		msg->getU8Fast(_PREHASH_Location, _PREHASH_Y, y_pos, i);
		msg->getU8Fast(_PREHASH_Location, _PREHASH_Z, z_pos, i);
		LLUUID agent_id = LLUUID::null;
		if(has_agent_data)
		{
			msg->getUUIDFast(_PREHASH_AgentData, _PREHASH_AgentID, agent_id, i);
		}

		//llinfos << "  object X: " << (S32)x_pos << " Y: " << (S32)y_pos
		//		<< " Z: " << (S32)(z_pos * 4)
		//		<< llendl;

		// treat the target specially for the map
		if(i == target_index)
		{
#if 0
			LLVector3d global_pos(mOriginGlobal);
			global_pos.mdV[VX] += (F64)(x_pos);
			global_pos.mdV[VY] += (F64)(y_pos);
			global_pos.mdV[VZ] += (F64)(z_pos) * 4.0;
			LLAvatarTracker::instance().setTrackedCoarseLocation(global_pos);
#endif
		}
		
		//don't add you
		if( i != agent_index)
		{
			pos = 0x0;
			pos |= x_pos;
			pos <<= 8;
			pos |= y_pos;
			pos <<= 8;
			pos |= z_pos;
			mMapAvatars.put(pos);
			if(has_agent_data)
			{
				mMapAvatarIDs.put(agent_id);
			}
		}
	}

	LLC::ManagerImpl::GetInstance()->UpdateLocalAvatars();
}


namespace
{

LLVector3d unpackLocalToGlobalPosition(U32 compact_local, const LLVector3d& region_origin)
{
	LLVector3d pos_global;
	LLVector3 pos_local;
	U8 bits;

	bits = compact_local & 0xFF;
	pos_local.mV[VZ] = F32(bits) * 4.f;
	compact_local >>= 8;

	bits = compact_local & 0xFF;
	pos_local.mV[VY] = (F32)bits;
	compact_local >>= 8;

	bits = compact_local & 0xFF;
	pos_local.mV[VX] = (F32)bits;

	pos_global.setVec( pos_local );
	pos_global += region_origin;
	return pos_global;
}

}
// namespace


void LLViewerRegion::getAvatars	( LLUUIDList& avatar_ids
								, const LLVector3d& relative_to
								, F32 radius
								) const
{
	avatar_ids.clear();
	//
	const LLVector3d& origin_global = getOriginGlobal();
	S32 count = mMapAvatars.count();
	for (S32 i = 0; i < count; i++)
	{
		LLVector3d pos_global = unpackLocalToGlobalPosition( mMapAvatars.get(i), origin_global );
		const F32 distance = dist_vec( pos_global, relative_to );
		if( distance <= radius )
		{
			avatar_ids.push_back( mMapAvatarIDs.get(i) );
		}
	}
}


LLVector3d LLViewerRegion::getPosGlobalFromRegion(const LLVector3 &pos_region) const
{
	LLVector3d pos_region_d;
	pos_region_d.setVec(pos_region);
	return pos_region_d + mOriginGlobal;
}


LLVector3 LLViewerRegion::getPosAgentFromRegion(const LLVector3 &pos_region) const
{
	LLVector3d pos_global = getPosGlobalFromRegion(pos_region);

	llassert( m_agent );

	return m_agent->getPosAgentFromGlobal(pos_global);
}

// vim: ts=4 sw=4 syntax=cpp.doxygen
