/** 
 * @file llworld.h
 * @brief Initial test structure to organize viewer regions
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

#ifndef LL_LLWORLD_H
#define LL_LLWORLD_H

//#include "llpatchvertexarray.h"

#include <list>

// llmath
//
#include "llmath.h"
#include "xform.h"
#include "v3math.h"
#include "llmemory.h"
#include "llstring.h"
//#include "llviewerpartsim.h"
//#include "llviewerimage.h"
//#include "llvowater.h"
#include "llviewerregion.h"

//class LLViewerRegion;
class LLVector3d;
class LLMessageSystem;
class LLNetMap;
class LLHost;

//class LLViewerObject;
//class LLSurfacePatch;

//class LLCloudPuff;
//class LLCloudGroup;
//class LLVOAvatar;

// LLWorld maintains a stack of unused viewer_regions and an array of pointers to viewer regions
// as simulators are connected to, viewer_regions are popped off the stack and connected as required
// as simulators are removed, they are pushed back onto the stack

class LLWorld : public LLSingleton<LLWorld>
{
public:
	LLWorld();
	virtual ~LLWorld();
	void destroyClass();

	LLViewerRegionPtr	addRegion(const U64 &region_handle, const LLHost &host);
		// safe to call if already present, does the "right thing" if
		// hosts are same, or if hosts are different, etc...
	void			removeRegion(const LLHost &host);

	void	disconnectRegions(); // Send quit messages to all child regions

	LLViewerRegionPtr		getRegion(const LLHost &host);
	LLViewerRegionPtr		getRegionFromPosGlobal(const LLVector3d &pos);
	//LLViewerRegion*			getRegionFromPosAgent(const LLVector3 &pos);
	//BOOL					positionRegionValidGlobal(const LLVector3d& pos);			// true if position is in valid region
	//LLVector3d				clipToVisibleRegions(const LLVector3d &start_pos, const LLVector3d &end_pos);
	
	LLViewerRegionPtr		getRegionFromHandle(const U64 &handle);

	void					updateAgentOffset(const LLVector3d &offset);

	// All of these should be in the agent coordinate frame
	LLViewerRegionPtr		resolveRegionGlobal(LLVector3 &localpos, const LLVector3d &position);
	LLViewerRegionPtr		resolveRegionAgent( LLVector3 &localpos, const LLVector3 &position);
	F32						resolveLandHeightGlobal(const LLVector3d &position);
	F32						resolveLandHeightAgent(const LLVector3 &position);

	U32						getRegionWidthInPoints() const	{ return mWidth; }
	F32						getRegionScale() const			{ return mScale; }

	// region X and Y size in meters
	F32						getRegionWidthInMeters() const	{ return mWidthInMeters; }
	F32						getRegionMinHeight() const		{ return -mWidthInMeters; }
	F32						getRegionMaxHeight() const		{ return MAX_OBJECT_Z; }

	void resetStats();
	void updateNetStats(); // Update network statistics for all the regions...

	void printPacketsLost();
	void requestCacheMisses();

	// deal with map object updates in the world.
	static void processCoarseUpdate(LLMessageSystem* msg, void** user_data);

	void getAvatars	( std::vector<LLUUID>* avatar_ids
					, std::vector<LLVector3d>* positions
					, const LLVector3d& relative_to
					, F32 radius
					) const;

	void setSpaceTimeUSec(const U64 space_time_usec);
	U64 getSpaceTimeUSec() const;

	void getInfo(LLSD& info);

public:
	typedef std::list<LLViewerRegionPtr> region_list_t;
	
	region_list_t	mActiveRegionList;

	region_list_t& getRegionList() { return mActiveRegionList; }

private:
	region_list_t	mRegionList;
	region_list_t	mVisibleRegionList;
	region_list_t	mCulledRegionList;

	// Number of points on edge
	static const U32 mWidth;

	// meters/point, therefore mWidth * mScale = meters per edge
	static const F32 mScale;

	static const F32 mWidthInMeters;

	S32 mLastPacketsIn;
	S32 mLastPacketsOut;
	S32 mLastPacketsLost;

	U64 mSpaceTimeUSec;
};


void process_enable_simulator(LLMessageSystem *mesgsys, void **user_data);
void process_disable_simulator(LLMessageSystem *mesgsys, void **user_data);
void process_region_handshake(LLMessageSystem* msg, void** user_data);

#endif
