/** 
 * @file llviewerregion.h
 * @brief Description of the LLViewerRegion class.
 *
 * $LicenseInfo:firstyear=2001&license=viewergpl$
 * 
 * Copyright (c) 2001-2009, Linden Research, Inc.
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

#ifndef LL_LLVIEWERREGION_H
#define LL_LLVIEWERREGION_H

// A ViewerRegion is a class that contains a bunch of objects and surfaces
// that are in to a particular region.
#include <string>

// llcommon
//
#include "stdtypes.h"
#include "llstat.h"
#include "lldarray.h"

// local project
//
#include "lleventpoll.h"

// llmessage
//
#include "llhost.h"
#include "llhttpclient.h"

// llmath
//
#include "v3math.h"
#include "v3dmath.h"

// boost
//
#include <boost/shared_ptr.hpp>

namespace LLC
{
	class Agent;
}

class LLMessageSystem;
class CoarseLocationUpdate;
//
class LLViewerRegion;
typedef boost::shared_ptr<LLViewerRegion> LLViewerRegionPtr;

class LLViewerRegion 
{
public:
	LLViewerRegion( U64 regionHandle
		, const LLHost &host
		, const U32 surface_grid_width
		, const U32 patch_grid_width
		, const F32 region_width_meters
		);
	~LLViewerRegion();

	void setHttpResponderPtrNULL() {m_httpResponderPtr = NULL ;}
	const LLHTTPClient::ResponderPtr getHttpResponderPtr() const {return m_httpResponderPtr ;}

	void setAgent( LLC::Agent* agent ) { m_agent = agent; }

	// Get/set named capability URLs for this region.
	void        setSeedCapability(const std::string& url);
	void        setCapability(const std::string& name, const std::string& url);
	std::string getCapability(const std::string& name) const;
	static bool isSpecialCapabilityName(const std::string &name);

    const LLHost	&getHost() const			{ return m_host; }
	bool			isAlive();
	U64				getHandle() const			{ return m_regionHandle; }
	void			updateNetStats();
	void			getInfo(LLSD& info);

	const std::string& getName() const		{ return mName; }
	void			setRegionNameAndZone(const std::string& name_zone);

	LLVector3d		getPosGlobalFromRegion(const LLVector3 &pos_region) const;
	LLVector3		getPosAgentFromRegion(const LLVector3 &pos_region) const;

	void			unpackRegionHandshake();

	const LLVector3d &getOriginGlobal() const	{ return mOriginGlobal; }

	void			updateCoarseLocations( LLMessageSystem* msg );

	F32	getWidth() const						{ return mWidth; }

	// TODO: Replace public data member access with get/set methods.
	//
	LLStat			mBitStat;
	LLStat			mPacketsStat;
	LLStat			mPacketsLostStat;

	const LLDynamicArray<U32>&		GetMapAvatars() const		{ return mMapAvatars;	}
	const LLDynamicArray<LLUUID>&	GetMapAvatarIDs() const		{ return mMapAvatarIDs; }

	typedef std::vector<LLUUID> LLUUIDList;
	void getAvatars	( LLUUIDList& avatar_ids
					, const LLVector3d& relative_to
					, F32 radius
					) const;

private:
	typedef std::map<std::string, std::string> CapabilityMap;
	CapabilityMap               m_capabilities;
	LLHTTPClient::ResponderPtr  m_httpResponderPtr ;

    LLEventPollPtr  m_eventPoll;
    U64             m_regionHandle;
    LLHost          m_host;
	F32				mWidth;			// Width of region on a side (meters)

	LLVector3d		mOriginGlobal;

	LLC::Agent*		m_agent;

	// simulator name
	std::string mName;
	std::string mZoning;

	// Network statistics for the region's circuit...
	bool			mAlive;					// can become false if circuit disconnects
	LLTimer 		mLastNetUpdate;
	U32				mPacketsIn;
	U32				mBitsIn;
	U32				mLastBitsIn;
	U32				mLastPacketsIn;
	U32				mPacketsOut;
	U32				mLastPacketsOut;
	S32				mPacketsLost;
	S32				mLastPacketsLost;
	U32				mPingDelay;
	F32				mDeltaTime;				// Time since last measurement of lastPackets, Bits, etc

	// These arrays are maintained in parallel. Ideally they'd be combined into a
	// single array of an aggrigate data type but for compatibility with the old
	// messaging system in which the previous message only sends and parses the 
	// positions stored in the first array so they're maintained separately until 
	// we stop supporting the old CoarseLocationUpdate message.
	LLDynamicArray<U32> mMapAvatars;
	LLDynamicArray<LLUUID> mMapAvatarIDs;

	friend class CoarseLocationUpdate;

	void			initStats();
};


#endif


