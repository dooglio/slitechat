/** 
 * \brief Header for the C++ library interface to Linden Lab's source code
 *
 * Copyright (c) 2009-2010 by R. Douglas Barbieri
 * Copyright (c) 2004-2010, Linden Research, Inc.
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
 *
 * Google API key for slitechat.dooglio.net:
 * ABQIAAAAGUEOxhujjY-VWquFTRbTWBQSeCpE_9msdrqpT5NdIC7cv9X-JhQWcGFoVQ9jJILbnHYfRy0uoeevbw
 *
 * Google detect:
 * http://ajax.googleapis.com/ajax/services/language/detect?v=1.0&q=" + messageESC + "&key=" + apikey;
 *
 * Google lang translate:
 * http://ajax.googleapis.com/ajax/services/language/translate?v=1.0&q=hello%20world&langpair=en%7Cit&key=BLAH
 */
#if defined(WIN32)
#pragma warning( disable:4996 )
#endif

// Local project
//
#include "LLChatLib.h"
#include "ManagerImpl.h"
#include "StringImpl.h"
#include "llviewerregion.h"
#include "llworld.h"
#include "lljoint.h"
#include "SLUrlUtils.h"
#include "GoogleTranslate.h"

// Std libraries
//
#include <cstdlib>
#include <sstream>
#include <iostream>

// boost
//
#include <boost/any.hpp>

//============== LINDEN Libraries =====================
//
// llcommon
//
#include "stdtypes.h"
#include "llversionviewer.h"
#include "lluuid.h"
#include "llmemory.h"
#include "llmd5.h"
#include "lldefs.h"
#include "lldarray.h"
#include "llchat.h"
#include "llsdserialize.h"
//
// llinventory
//
#include "lluserrelations.h"
//
// llxml
//
#include "llcontrol.h"
#include "llxmltree.h"
//
// llvfs.h
//
#include "lldir.h"
#include "llfile.h"
#include "llvfs.h"
#include "llvfile.h"
//
// llmessage
//
#include "message.h"
#include "llpumpio.h"
#include "llares.h"
#include "llcachename.h"
#include "llinstantmessage.h"
#include "message_prehash.h"
#include "llregionhandle.h"
#include "mean_collision_data.h"
#include "llqueryflags.h"
#include "llmessageconfig.h"
#include "llassetstorage.h"
#include "llxfermanager.h"
#include "llteleportflags.h"
#include "llbufferstream.h"
//
// llmath
//
#include "v3math.h"
#include "llquaternion.h"
#include "llcoordframe.h"

// Local includes lifed from newview
//
#include "lluserauth.h"
#include "llviewernetwork.h"
#include "llsrv.h"
#include "llwearable.h"

// Ugh. Global variables.
//
std::string				gCurrentVersion;
S32 					gTotalDAlloc;
S32 					gTotalDAUse;
S32 					gDACount;
extern LLAssetStorage *	gAssetStorage;

const std::string AVATAR_DEFAULT_CHAR = "avatar";

#define MAP_SIM_IMAGE_TYPES 3
#define GOOGLE_XLATE_CONFIDENCE_CODE 0.15f

#if LL_WINDOWS || LL_MINGW32
#	include <tchar.h>
#endif

#if LL_DARWIN
#	include <Carbon/Carbon.h>
#endif


#include "ChatterBox.h"

LLHTTPRegistration<LLViewerChatterBoxSessionStartReply>
   gHTTPRegistrationMessageChatterboxsessionstartreply(
	   "/message/ChatterBoxSessionStartReply");

LLHTTPRegistration<LLViewerChatterBoxSessionEventReply>
   gHTTPRegistrationMessageChatterboxsessioneventreply(
	   "/message/ChatterBoxSessionEventReply");

LLHTTPRegistration<LLViewerForceCloseChatterBoxSession>
    gHTTPRegistrationMessageForceclosechatterboxsession(
		"/message/ForceCloseChatterBoxSession");

LLHTTPRegistration<LLViewerChatterBoxSessionAgentListUpdates>
    gHTTPRegistrationMessageChatterboxsessionagentlistupdates(
	    "/message/ChatterBoxSessionAgentListUpdates");

LLHTTPRegistration<LLViewerChatterBoxSessionUpdate>
    gHTTPRegistrationMessageChatterBoxSessionUpdate(
	    "/message/ChatterBoxSessionUpdate");

LLHTTPRegistration<LLViewerChatterBoxInvitation>
    gHTTPRegistrationMessageChatterBoxInvitation(
		"/message/ChatterBoxInvitation");


namespace LLC
{


namespace
{
	void _OnCacheNameCallback( const LLUUID& id, const std::string& firstname, const std::string& lastname, BOOL is_group, void* data )
	{
		ManagerImpl::GetInstance()->OnCacheNameCallback( id, firstname, lastname, is_group, data );
	}

	void _OnProcessAgentMovementComplete( LLMessageSystem* msg, void **user_data )
	{
		ManagerImpl::GetInstance()->OnProcessAgentMovementComplete( msg, user_data );
	}

	void _OnProcessIMCallback( LLMessageSystem* msg, void **user_data )
	{
		ManagerImpl::GetInstance()->OnProcessIMCallback( msg, user_data );
	}

	void _OnProcessLocalChat( LLMessageSystem* msg, void **user_data )
	{
		ManagerImpl::GetInstance()->OnProcessLocalChat( msg, user_data );
	}

	void _OnOnlineNotifyCallback( LLMessageSystem* msg, void **user_data )
	{
		ManagerImpl::GetInstance()->OnOnlineNotifyCallback( msg, user_data );
	}

	void _OnOfflineNotifyCallback( LLMessageSystem* msg, void **user_data )
	{
		ManagerImpl::GetInstance()->OnOfflineNotifyCallback( msg, user_data );
	}

	void _OnTerminateFriendshipCallback( LLMessageSystem* msg, void **user_data )
	{
		ManagerImpl::GetInstance()->OnTerminateFriendshipCallback( msg, user_data );
	}

	void _OnSearchResultCallback( LLMessageSystem* msg, void **user_data )
	{
		ManagerImpl::GetInstance()->OnSearchResultCallback( msg, user_data );
	}

	void _OnKickUserCallback( LLMessageSystem* msg, void **user_data )
	{
		ManagerImpl::GetInstance()->OnKickUserCallback( msg, user_data );
	}

	void _OnLogoutCallback( LLMessageSystem* msg, void **user_data )
	{
		ManagerImpl::GetInstance()->OnLogoutCallback( msg, user_data );
	}

	void _OnAgentDataUpdate( LLMessageSystem* msg, void **user_data )
	{
		ManagerImpl::GetInstance()->OnAgentDataUpdate( msg, user_data );
	}

	void _OnAgentGroupDataUpdate( LLMessageSystem* msg, void **user_data )
	{
		ManagerImpl::GetInstance()->OnAgentGroupDataUpdate( msg, user_data );
	}

	void _OnAgentWearablesUpdate( LLMessageSystem* msg, void **user_data )
	{
		ManagerImpl::GetInstance()->OnAgentWearablesUpdate( msg, user_data );
	}

	void _OnAgentCachedTextureResponse( LLMessageSystem* msg, void **user_data )
	{
		ManagerImpl::GetInstance()->OnAgentCachedTextureResponse( msg, user_data );
	}

	void _OnMapBlockReply( LLMessageSystem* msg, void **user_data )
	{
		ManagerImpl::GetInstance()->OnMapBlockReply( msg, user_data );
	}

	void _OnTeleportStart( LLMessageSystem* msg, void **user_data )
	{
		ManagerImpl::GetInstance()->OnTeleportStart( msg, user_data );
	}

	void _OnTeleportProgress( LLMessageSystem* msg, void **user_data )
	{
		ManagerImpl::GetInstance()->OnTeleportProgress( msg, user_data );
	}

	void _OnTeleportFailed( LLMessageSystem* msg, void **user_data )
	{
		ManagerImpl::GetInstance()->OnTeleportFailed( msg, user_data );
	}

	void _OnTeleportLocal( LLMessageSystem* msg, void **user_data )
	{
		ManagerImpl::GetInstance()->OnTeleportLocal( msg, user_data );
	}

	void _OnTeleportFinish( LLMessageSystem* msg, void **user_data )
	{
		ManagerImpl::GetInstance()->OnTeleportFinish( msg, user_data );
	}
	

#if LL_WINDOWS || LL_MINGW32
	std::string _GenerateSerialNumberWin32()
	{
		char serial_md5[MD5HEX_STR_SIZE];		// Flawfinder: ignore
		serial_md5[0] = 0;

		DWORD serial = 0;
		DWORD flags = 0;
		BOOL success = GetVolumeInformation(
				_T("C:\\"),
				NULL,		// volume name buffer
				0,			// volume name buffer size
				&serial,	// volume serial
				NULL,		// max component length
				&flags,		// file system flags
				NULL,		// file system name buffer
				0);			// file system name buffer size
		if (success)
		{
			LLMD5 md5;
			md5.update( (unsigned char*)&serial, sizeof(DWORD));
			md5.finalize();
			md5.hex_digest(serial_md5);
		}
		else
		{
			throw AuthException( "GetVolumneInformation failed!" );
		}
		return serial_md5;
	}
#endif

#if LL_LINUX
	std::string _GenerateSerialNumberLinux()
	{
		char serial_md5[MD5HEX_STR_SIZE];		// Flawfinder: ignore
		serial_md5[0] = 0;
		// TODO: Implement
		return serial_md5;
	}
#endif

#if LL_DARWIN
	std::string _GenerateSerialNumberMacOSX()
	{
		char serial_md5[MD5HEX_STR_SIZE];		// Flawfinder: ignore
		serial_md5[0] = 0;

		// JC: Sample code from http://developer.apple.com/technotes/tn/tn1103.html
		CFStringRef serialNumber = NULL;
		io_service_t    platformExpert = IOServiceGetMatchingService(kIOMasterPortDefault,
				IOServiceMatching("IOPlatformExpertDevice"));
		if (platformExpert) {
			serialNumber = (CFStringRef) IORegistryEntryCreateCFProperty(platformExpert,
					CFSTR(kIOPlatformSerialNumberKey),
					kCFAllocatorDefault, 0);		
			IOObjectRelease(platformExpert);
		}

		if (serialNumber)
		{
			char buffer[MAX_STRING];		// Flawfinder: ignore
			if (CFStringGetCString(serialNumber, buffer, MAX_STRING, kCFStringEncodingASCII))
			{
				LLMD5 md5( (unsigned char*)buffer );
				md5.hex_digest(serial_md5);
			}
			CFRelease(serialNumber);
		}

		return serial_md5;
	}
#endif

	std::string _GenerateSerialNumber()
	{
#if defined(LL_WINDOWS) || defined(LL_MINGW32)
		return _GenerateSerialNumberWin32();
#elif defined(LL_LINUX)
		return _GenerateSerialNumberLinux();
#elif defined(LL_DARWIN)
		return _GenerateSerialNumberMacOSX();
#else
#	error TODO: Add non-win32 support!
#endif
	}
}
//namespace LLC

boost::shared_ptr<ManagerImpl>	ManagerImpl::m_instance;


/** \brief MD5 munge a clear text password.
 * \param [in] clear_text	password in clear text to munge
 * \return LLC::String containing the munged password.
 * \sa Manager::Authenticate()
 */
LLCHATLIBEXP String MungedPassword( const String& clear_text )
{
	LLMD5 password( (unsigned char*) clear_text.GetString() );
	char munged_password[MD5HEX_STR_SIZE];
	password.hex_digest(munged_password);

	return String(munged_password);
}

ManagerImpl::ManagerImpl()
	: m_llua(0)
	, m_started(false)
	, m_langId("en")
	, m_translateMessages(true)
	, m_user_settings(NULL)
{
	gSavedSettings.resetToDefaults();
}


void ManagerImpl::StartMessagingSystem(const char *appname, const char *settings)
{
	ll_init_apr();

	gDirUtilp->initAppDirs( appname );

	LLHTTPClient::setPump(*gServicePump);
	LLCurl::setCAFile(gDirUtilp->getCAFile());

	std::string message_template_path = gDirUtilp->getExpandedFilename( LL_PATH_APP_SETTINGS, "message_template.msg" );
	//
	if (!LLFile::isfile(message_template_path.c_str())) 
	{
		std::stringstream ss;
		ss << "Cannot find message_template.msg! The path '" << message_template_path.c_str() << "' is not valid!" << std::ends;
		std::cout << ss.str().c_str() << std::endl;
		throw AuthException( ss.str().c_str() );
	}

	m_user_settings = settings;
	std::string user_settings = gDirUtilp->getExpandedFilename( LL_PATH_USER_SETTINGS, m_user_settings );
	if( gDirUtilp->fileExists( user_settings ) )
	{
		gSavedSettings.loadFromFile( user_settings );
	}

	U32 port = gSavedSettings.getU32("UserConnectionPort");
	const LLUseCircuitCodeResponder* responder = NULL;
	bool failure_is_fatal = true;
	const F32 circuit_heartbeat_interval = 5;
	const F32 circuit_timeout = 100;

	if(!start_messaging_system(
		message_template_path,
		port,
		LL_VERSION_MAJOR,
		LL_VERSION_MINOR,
		LL_VERSION_PATCH,
		FALSE,
		std::string(),
		responder,
		failure_is_fatal,
		circuit_heartbeat_interval,
		circuit_timeout))
	{
		std::stringstream ss;
		ss 
			<< "Cannot start messaging system! Error number: " << gMessageSystem->getErrorCode()
			<< ", message_template_path=" << message_template_path.c_str()
			<< std::ends;
		std::cout << ss.str().c_str() << std::endl;
		throw AuthException( ss.str().c_str() );
	}

	// Load message.xml file--critical for message templates
	//
	std::string message_path = gDirUtilp->getExpandedFilename( LL_PATH_APP_SETTINGS, "message.xml" );
	if (!LLFile::isfile(message_path.c_str())) 
	{
		std::stringstream ss;
		ss << "Cannot find message.xml! The path '" << message_path.c_str() << "' is not valid!" << std::ends;
		std::cout << ss.str().c_str() << std::endl;
		throw AuthException( ss.str().c_str() );
	}
	//
	LLMessageConfig::initClass( "viewer", gDirUtilp->getExpandedFilename(LL_PATH_APP_SETTINGS, "") );

	//
	gMessageSystem->setHandlerFuncFast( _PREHASH_ImprovedInstantMessage		, _OnProcessIMCallback    			);
	gMessageSystem->setHandlerFuncFast( _PREHASH_AgentMovementComplete		, _OnProcessAgentMovementComplete  	);
	gMessageSystem->setHandlerFuncFast( _PREHASH_ChatFromSimulator			, _OnProcessLocalChat    			);
	gMessageSystem->setHandlerFuncFast( _PREHASH_OnlineNotification			, _OnOnlineNotifyCallback 			);
	gMessageSystem->setHandlerFuncFast( _PREHASH_OfflineNotification		, _OnOfflineNotifyCallback			);
	gMessageSystem->setHandlerFuncFast( _PREHASH_TerminateFriendship		, _OnTerminateFriendshipCallback	);
	gMessageSystem->setHandlerFuncFast( _PREHASH_DirPeopleReply				, _OnSearchResultCallback			);
	gMessageSystem->setHandlerFuncFast( _PREHASH_KickUser					, _OnKickUserCallback				);
	gMessageSystem->setHandlerFuncFast( _PREHASH_LogoutReply				, _OnLogoutCallback					);
	gMessageSystem->setHandlerFuncFast( _PREHASH_AgentDataUpdate			, _OnAgentDataUpdate				);
	gMessageSystem->setHandlerFuncFast( _PREHASH_AgentGroupDataUpdate		, _OnAgentGroupDataUpdate			);
	gMessageSystem->setHandlerFuncFast( _PREHASH_AgentWearablesUpdate		, _OnAgentWearablesUpdate			);
	gMessageSystem->setHandlerFuncFast( _PREHASH_AgentCachedTextureResponse	, _OnAgentCachedTextureResponse		);
	//
	// Handled in lmworld.h
	//
	gMessageSystem->setHandlerFuncFast( _PREHASH_EnableSimulator			, process_enable_simulator			);
	gMessageSystem->setHandlerFuncFast( _PREHASH_DisableSimulator			, process_disable_simulator			);
	gMessageSystem->setHandlerFuncFast( _PREHASH_RegionHandshake			, process_region_handshake			);
	//
	// Teleport handling
	//
	gMessageSystem->setHandlerFuncFast( _PREHASH_MapBlockReply				, _OnMapBlockReply				  	);
	gMessageSystem->setHandlerFuncFast( _PREHASH_TeleportStart				, _OnTeleportStart				  	);
	gMessageSystem->setHandlerFuncFast( _PREHASH_TeleportProgress			, _OnTeleportProgress			  	);
	gMessageSystem->setHandlerFuncFast( _PREHASH_TeleportFailed				, _OnTeleportFailed				  	);
	gMessageSystem->setHandlerFuncFast( _PREHASH_TeleportLocal				, _OnTeleportLocal				  	);
	gMessageSystem->setHandlerFuncFast( _PREHASH_TeleportFinish				, _OnTeleportFinish				  	);
	//
	// Other
	//
	gMessageSystem->setHandlerFuncFast(_PREHASH_CoarseLocationUpdate,		LLWorld::processCoarseUpdate, NULL);


	// Initialize messaging stuff
	//
	ll_init_ares();
	m_llua = LLUserAuth::getInstance();

	m_started = true;

	// Init VFS and asset storage
	//
	const S32 MB = 1024*1024;
	S64 cache_size = (S64)(gSavedSettings.getU32("CacheSize")) * MB;
	const S64 MAX_CACHE_SIZE = 1024*MB;
	cache_size = llmin(cache_size, MAX_CACHE_SIZE);
	const S64 texture_cache_size = ((cache_size * 8)/10);
	//
	S64 vfs_size = cache_size - texture_cache_size;
	const S64 MAX_VFS_SIZE = 1024 * MB; // 1 GB
	vfs_size = llmin(vfs_size, MAX_VFS_SIZE);
	vfs_size = (vfs_size / MB) * MB; // make sure it is MB aligned
	const U32 vfs_size_u32 = (U32)vfs_size;
	//
	const U32 new_salt = rand();
	const char *VFS_DATA_FILE_BASE = "data.db2.x.";
	const char *VFS_INDEX_FILE_BASE = "index.db2.x.";
	std::string new_vfs_data_file  = gDirUtilp->getExpandedFilename(LL_PATH_CACHE, VFS_DATA_FILE_BASE)  + llformat("%u",new_salt);
	std::string new_vfs_index_file = gDirUtilp->getExpandedFilename(LL_PATH_CACHE, VFS_INDEX_FILE_BASE) + llformat("%u",new_salt);
	//
	gVFS = new LLVFS(new_vfs_index_file, new_vfs_data_file, false, vfs_size_u32, false);
	//
	if( gVFS->isValid() )
	{
		LLVFile::initClass();
	}
	else
	{
		throw AuthException( "Cannot start VFS file thread!" );
	}
	//
	const S32 VIEWER_MAX_XFER = 3;
	start_xfer_manager(gVFS);
	gXferManager->setMaxIncomingXfers(VIEWER_MAX_XFER);
	const F32 xfer_throttle_bps = gSavedSettings.getF32("XferThrottle");
	if (xfer_throttle_bps > 1.f)
	{
		gXferManager->setUseAckThrottling(TRUE);
		gXferManager->setAckThrottleBPS(xfer_throttle_bps);
	}
	gAssetStorage = new LLAssetStorage(gMessageSystem, gXferManager, gVFS);

	LLXmlTree sXMLTree;
	std::string xmlFile;

	xmlFile = gDirUtilp->getExpandedFilename(LL_PATH_CHARACTER,AVATAR_DEFAULT_CHAR) + "_lad.xml";
	BOOL success = sXMLTree.parseFile( xmlFile, FALSE );
	if (!success)
	{
		llerrs << "Problem reading avatar configuration file:" << xmlFile << llendl;
		throw AuthException( "Problem reading avatar configuration file!" );
	}

	// now sanity check xml file
	LLXmlTreeNode* root = sXMLTree.getRoot();
	if (!root) 
	{
		llerrs << "No root node found in avatar configuration file: " << xmlFile << llendl;
		throw AuthException( "No root node found in avatar configuration file!" );
	}

	//-------------------------------------------------------------------------
	// <linden_avatar version="1.0"> (root)
	//-------------------------------------------------------------------------
	if( !root->hasName( "linden_avatar" ) )
	{
		llerrs << "Invalid avatar file header: " << xmlFile << llendl;
		throw AuthException( "Invalid avatar file header!" );
	}
	
	std::string version;
	static LLStdStringHandle version_string = LLXmlTree::addAttributeString("version");
	if( !root->getFastAttributeString( version_string, version ) || (version != "1.0") )
	{
		llerrs << "Invalid avatar file version: " << version << " in file: " << xmlFile << llendl;
		throw AuthException( "Invalid avatar file version!" );
	}

	S32 wearable_def_version = 1;
	static LLStdStringHandle wearable_definition_version_string = LLXmlTree::addAttributeString("wearable_definition_version");
	root->getFastAttributeS32( wearable_definition_version_string, wearable_def_version );
	LLWearable::setCurrentDefinitionVersion( wearable_def_version );

}


ManagerImpl::~ManagerImpl()
{
	if( m_started )
	{
		// Save settings
		//
		if (m_user_settings != NULL)
		{
			std::string user_settings = gDirUtilp->getExpandedFilename( LL_PATH_USER_SETTINGS, m_user_settings );
			gSavedSettings.saveToFile( user_settings, false /*nondefault_only*/ );
		}

		m_viewerRegion.reset();
	
		end_messaging_system();
	
		LLUserAuth::Release();
	
		ll_release_ares();
		//
		LLCacheName::Release();
		LLPumpIO::Release();
		LLControlGroup::Release();
		LLHost::Release();
	
		ll_cleanup_apr();
	}
}


boost::shared_ptr<ManagerImpl>	ManagerImpl::GetInstance()
{
	if( m_instance == 0 )
	{
		m_instance.reset( new ManagerImpl );
	}
	//
	return m_instance;
}


void ManagerImpl::Release()
{
	m_instance.reset();
}


void ManagerImpl::Authenticate( const String& login_url, const String& first_name, const String& last_name, const String& munged_password, const String& starting_slurl ) //, Manager::SaveMethod saveMethod )
{
	std::vector<const char*> requested_options;
	requested_options.push_back("buddy-list");
	requested_options.push_back("inventory-root");
	//
	LLUUID::getNodeID(gMACAddress);
	char hashed_mac_string[MD5HEX_STR_SIZE];		/* Flawfinder: ignore */
	LLMD5 hashed_mac;
	hashed_mac.update( gMACAddress, MAC_ADDRESS_BYTES );
	hashed_mac.finalize();
	hashed_mac.hex_digest(hashed_mac_string);
	
	// Blow away the old cache and create it again, adding the observer again
	// This works around the bug where we no longer get cache messages when we
	// log out and back in again.
	//
	LLCacheName::Release();
	//
	LLCacheName* llcash = LLCacheName::getInstance();
	llcash->addObserver( _OnCacheNameCallback );

    // Remind the avatar name for later use
    m_fullName = first_name.GetString() + std::string(" ") + last_name.GetString();

	std::cout << "Logging in to " << login_url.GetString()
		  << " as " << first_name.GetString()
		  << " " << last_name.GetString() << "." << std::endl;

	// Determine starting place
	std::stringstream start;
	std::string sim_name;
	S32 mX,mY,mZ;
	if( UrlParse( starting_slurl.GetString(), &sim_name, &mX, &mY, &mZ ) )
	{
		// a startup URL was specified
		std::stringstream unescaped_start;
		unescaped_start << "uri:" 
						<< sim_name << "&" 
						<< mX << "&" 
						<< mY << "&" 
						<< mZ;
		start << xml_escape_string(unescaped_start.str());
		
	}
	else
	{
		start << starting_slurl.GetString();
	}
	//
	m_llua->reset();
	m_llua->authenticate(
			login_url.GetString(),
			"login_to_simulator",
			first_name.GetString(),
			last_name.GetString(),
			munged_password.GetString(),
			start.str(),
			TRUE,
			TRUE,
			TRUE,
			FALSE,
			requested_options,
			hashed_mac_string,
			_GenerateSerialNumber() );
}


bool ManagerImpl::CheckForResponse()
{
	bool success = false;
	LLUserAuth::UserAuthcode error = LLUserAuth::getInstance()->authResponse();
	std::stringstream ss;
	switch( error )
	{
		case LLUserAuth::E_NO_RESPONSE_YET:
		case LLUserAuth::E_DOWNLOADING:
			// Just keep waiting
			//
			break;

		case LLUserAuth::E_OK:
			{
				std::string login_response = LLUserAuth::getInstance()->getResponse("login");
				if( login_response == "true" )
				{
					std::cout << "Successful login!" << std::endl;
					success = true;
				}
				else
				{
					std::string reason_response  = LLUserAuth::getInstance()->getResponse("reason");
					std::string message_response = LLUserAuth::getInstance()->getResponse("message");
					ss	<< "Login failure: "
						<< reason_response.c_str()
						<< ", "
						<< message_response.c_str()
						<< std::ends;
					throw AuthException( ss.str().c_str() );
				}
			}
			break;

		case LLUserAuth::E_COULDNT_RESOLVE_HOST:
			ss << "Can't resolve host" << std::ends;
			throw AuthException( ss.str().c_str() );

		case LLUserAuth::E_SSL_PEER_CERTIFICATE:
			ss << "SSL Peer Cert error" << std::ends;
			throw AuthException( ss.str().c_str() );

		case LLUserAuth::E_SSL_CACERT:
			ss << "SSL_Cert error" << std::ends;
			throw AuthException( ss.str().c_str() );

		case LLUserAuth::E_SSL_CONNECT_ERROR:
			ss << "SSL connect error" << std::ends;
			throw AuthException( ss.str().c_str() );

		case LLUserAuth::E_UNHANDLED_ERROR:
			ss << "Unhandled error" << std::ends;
			throw AuthException( ss.str().c_str() );

		default:
			ss << "Unknown error number " << error << "!" << std::ends;
			throw AuthException( ss.str().c_str() );
	}

	return success;
}


namespace
{

void LocalPumpMessages()
{
	const S64 frame_count = 32;  // U32->S64
	gMessageSystem->checkAllMessages( frame_count, gServicePump );
	gMessageSystem->processAcks();
}

}


void ManagerImpl::RequestBuddyList()
{
	LLUserAuth::options_t options;
	options.clear();
	//
	typedef std::map<LLUUID, LLRelationship*> buddy_map_t;
	buddy_map_t list;
	//
	if( LLUserAuth::getInstance()->getOptions( "buddy-list", options ) )
	{
		LLUserAuth::options_t::iterator it = options.begin();
		const LLUserAuth::options_t::iterator end = options.end();
		LLUUID agent_id;
		S32 has_rights = 0, given_rights = 0;
		for (; it != end; ++it)
		{
			LLUserAuth::response_t::const_iterator option_it;
			option_it = (*it).find("buddy_id");
			if(option_it != (*it).end())
			{
				std::string id = (*option_it).second;
				agent_id.set( id );
			}
			option_it = (*it).find("buddy_rights_has");
			if(option_it != (*it).end())
			{
				has_rights = atoi((*option_it).second.c_str());
			}
			option_it = (*it).find("buddy_rights_given");
			if(option_it != (*it).end())
			{
				given_rights = atoi((*option_it).second.c_str());
			}
			LLRelationship* rel = new LLRelationship(given_rights, has_rights, false);
			list[agent_id] = rel;
			//
			std::string name;
			gCacheName->getFullName( agent_id, name );
			std::cout << "Cache name request for " << agent_id.asString().c_str() << std::endl;

			// Send notice to GUI
			//
			m_friendAddSignal( String(agent_id.asString().c_str()) );

			// This may be necessary for really long friends lists that cause us to get kicked out.
			//
			//LocalPumpMessages();
		}
	}

	// Get inventory root folder
	//
	options.clear();
	if( LLUserAuth::getInstance()->getOptions( "inventory-root", options ) )
	{
		std::cout << "Parsing inventory" << std::endl;
		LLUserAuth::options_t::iterator			it  = options.begin();
		const LLUserAuth::options_t::iterator	end = options.end();
		//
		for (; it != end; ++it)
		{
			LLUserAuth::response_t::const_iterator option_it;
			option_it = (*it).find("folder_id");
			if(option_it != (*it).end())
			{
				std::string id = (*option_it).second;
				m_rootInventoryFolder.set( id );
				break;
			}

			// This may be necessary for really long friends lists that cause us to get kicked out.
			//
			//LocalPumpMessages();
		}
	}

	LocalPumpMessages();
}


void ManagerImpl::SendReliable( LLMessageSystem* msg )
{
	Agent::SendReliable( msg );
}


void ManagerImpl::SendCompleteAgentMovement( const LLHost& sim_host )
{
	LLMessageSystem* msg = gMessageSystem;

	// send_complete_agent_movement(gHost)
	//
	msg->newMessageFast(_PREHASH_CompleteAgentMovement);
	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID, m_agentId);
	msg->addUUIDFast(_PREHASH_SessionID, m_sessionId);
	msg->addU32Fast(_PREHASH_CircuitCode, msg->mOurCircuitCode);
	msg->sendReliable( sim_host );
}


void ManagerImpl::AnnounceInSim()
{
	LLMessageSystem* msg = gMessageSystem;
	//
	std::string agent_id_str = LLUserAuth::getInstance()->getResponse("agent_id");
	if(!agent_id_str.empty())
	{
		m_agentId.set( agent_id_str );
		m_agent.SetAgentId( m_agentId );
	}
	//
	std::string session_id_str = LLUserAuth::getInstance()->getResponse("session_id");
	if(!session_id_str.empty())
	{
		m_sessionId.set( session_id_str );
		m_agent.SetSessionId( m_sessionId );
	}
	//
	std::string secure_session_id_str = LLUserAuth::getInstance()->getResponse("secure_session_id");
	if(!secure_session_id_str.empty()) m_secureSessionId.set( secure_session_id_str );
	//
	std::string circuit_code = LLUserAuth::getInstance()->getResponse("circuit_code");
	if(!circuit_code.empty())
	{
		msg->mOurCircuitCode = strtoul(circuit_code.c_str(), NULL, 10);
	}
	//
	std::string sim_ip_str   = LLUserAuth::getInstance()->getResponse("sim_ip");
	std::string sim_port_str = LLUserAuth::getInstance()->getResponse("sim_port");
	if(!sim_ip_str.empty() && !sim_port_str.empty())
	{
		U32 sim_port = strtoul(sim_port_str.c_str(), NULL, 10);
		gHost.set(sim_ip_str, sim_port);
		if (gHost.isOk())
		{
			msg->enableCircuit(gHost, TRUE);
		}
	}
    //
	msg->enableCircuit( gHost, TRUE );
	//
	std::string start_location = LLUserAuth::getInstance()->getResponse("start_location");
    //
    U64 first_sim_handle = 0;
	std::string region_x_str = LLUserAuth::getInstance()->getResponse("region_x");
	std::string region_y_str = LLUserAuth::getInstance()->getResponse("region_y");
	if(!region_x_str.empty() && !region_y_str.empty())
	{
		U32 region_x = strtoul(region_x_str.c_str(), NULL, 10);
		U32 region_y = strtoul(region_y_str.c_str(), NULL, 10);
		first_sim_handle = to_region_handle(region_x, region_y);
	}
    //
    std::string first_sim_seed_cap = LLUserAuth::getInstance()->getResponse("seed_capability");
    m_viewerRegion = LLWorld::getInstance()->addRegion( first_sim_handle, gHost );
    m_viewerRegion->setSeedCapability( first_sim_seed_cap );

	LLVOAvatar* av = new LLVOAvatar( m_agentId, LL_PCODE_LEGACY_AVATAR, m_viewerRegion, &m_agent );
	m_agent.SetAvatar( av );
	m_agent.setRegion( m_viewerRegion );

	// now, use the circuit info to tell simulator about us!
	//std::cout << "viewer: UserLoginLocationReply() Enabling " << gHost << " with code " << msg->mOurCircuitCode << std::endl;
	msg->newMessageFast(_PREHASH_UseCircuitCode);
	msg->nextBlockFast(_PREHASH_CircuitCode);
	msg->addU32Fast(_PREHASH_Code, msg->mOurCircuitCode);
	msg->addUUIDFast(_PREHASH_SessionID, m_sessionId);
	msg->addUUIDFast(_PREHASH_ID, m_agentId);
	SendReliable( msg );

	SendCompleteAgentMovement( gHost );

	gAssetStorage->setUpstream( gHost );
	gCacheName   ->setUpstream( gHost );

	msg->newMessageFast(_PREHASH_EconomyDataRequest);
	SendReliable( msg );

	msg->newMessageFast(_PREHASH_AgentDataUpdateRequest);
	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID, m_agentId);
	msg->addUUIDFast(_PREHASH_SessionID, m_sessionId);
	SendReliable( msg );

	// Ask for any pending messages
	//
	msg->newMessageFast( _PREHASH_RetrieveInstantMessages );
	msg->nextBlockFast( _PREHASH_AgentData );
	msg->addUUIDFast( _PREHASH_AgentID, m_agentId );
	msg->addUUIDFast( _PREHASH_SessionID, m_sessionId );
	SendReliable( msg );

	msg->newMessageFast(_PREHASH_AgentWearablesRequest);
	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID, m_agentId );
	msg->addUUIDFast(_PREHASH_SessionID, m_sessionId );
	SendReliable( msg );

	LocalPumpMessages();
}


void ManagerImpl::RequestLogout()
{
	LLMessageSystem* msg = gMessageSystem;
	msg->newMessageFast(_PREHASH_LogoutRequest);
	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID, m_agentId );
	msg->addUUIDFast(_PREHASH_SessionID, m_sessionId );
	SendReliable( msg );
}


void ManagerImpl::PumpMessages()
{
	LLFrameTimer::updateFrameTime();
	gAres->process();
	gServicePump->pump();
	gServicePump->callback();
	gCacheName->processPending();

	LocalPumpMessages();

	gXferManager->retransmitUnackedPackets();
	gAssetStorage->checkForTimeouts();

	LLCircuitData *cdp = gMessageSystem->mCircuitInfo.findCircuit( gHost );
	if (!cdp)
	{
		// We lost connection--alert the client!
		//
		m_forceQuitSignal( LLC::String( "Lost connection to server!") );
	}
}


void ManagerImpl::SendInstantMessage( const String& target_id, const String& message, const bool to_group )
{
	LLUUID to_uuid( target_id.GetString() );
	LLUUID im_session_id = to_group? to_uuid: to_uuid ^ m_agentId;

	std::cout << "ManagerImpl::SendInstantMessage()" << std::endl;

	LLMessageSystem* msg = gMessageSystem;
	pack_instant_message(
			msg,
			m_agentId,
			FALSE,
			m_sessionId,
			to_uuid,
			m_fullName.c_str(),
			message.GetString(),
			IM_ONLINE,
			to_group? IM_SESSION_SEND: IM_NOTHING_SPECIAL,
			im_session_id);
	SendReliable( msg );
}


void ManagerImpl::SendTypingSignal( const String& target_id, const bool to_group, const bool typing )
{
	LLUUID to_uuid( target_id.GetString() );
	LLUUID im_session_id = to_group? to_uuid: to_uuid ^ m_agentId;

	std::cout << "ManagerImpl::SendTypingSignal()" << std::endl;

	LLMessageSystem* msg = gMessageSystem;
	pack_instant_message(
			msg,
			m_agentId,
			FALSE,
			m_sessionId,
			to_uuid,
			m_fullName.c_str(),
			"typing",
			IM_ONLINE,
			typing? IM_TYPING_START: IM_TYPING_STOP,
			im_session_id 
			);
	SendReliable( msg );
}


void ManagerImpl::SendLocalChatMessage( const String& text, const int channel )
{
	LLMessageSystem* msg = gMessageSystem;
	//
	msg->newMessageFast	( _PREHASH_ChatFromViewer			);
	msg->nextBlockFast	( _PREHASH_AgentData				);
	msg->addUUIDFast	( _PREHASH_AgentID, m_agentId		);
	msg->addUUIDFast	( _PREHASH_SessionID, m_sessionId	);
	msg->nextBlockFast	( _PREHASH_ChatData					);
	msg->addStringFast	( _PREHASH_Message, text.GetString());
	msg->addU8Fast		( _PREHASH_Type, CHAT_TYPE_NORMAL	);
	msg->addS32Fast		( _PREHASH_Channel, channel			);
	//
	SendReliable( msg );
}


void ManagerImpl::SendGroupChatStartRequest( const String& group_id )
{
	LLUUID to_uuid( group_id.GetString() );
	//
	LLMessageSystem *msg = gMessageSystem;
	msg->newMessageFast( _PREHASH_ImprovedInstantMessage  				);
	msg->nextBlockFast ( _PREHASH_AgentData               				);
	msg->addUUIDFast   ( _PREHASH_AgentID, m_agentId      				);
	msg->addUUIDFast   ( _PREHASH_SessionID, m_sessionId  				);
	//
	msg->nextBlockFast ( _PREHASH_MessageBlock            				);
	msg->addBOOLFast   ( _PREHASH_FromGroup, FALSE        				);
	msg->addUUIDFast   ( _PREHASH_ToAgentID, to_uuid     				);
	msg->addU8Fast     ( _PREHASH_Offline, IM_ONLINE      				);
	msg->addU8Fast     ( _PREHASH_Dialog, IM_SESSION_GROUP_START		);
	msg->addUUIDFast   ( _PREHASH_ID, to_uuid		     				);
	msg->addU32Fast    ( _PREHASH_Timestamp, NO_TIMESTAMP 				); // no timestamp necessary
	//
	msg->addStringFast ( _PREHASH_FromAgentName, m_fullName				);
	msg->addStringFast ( _PREHASH_Message, LLStringUtil::null			);
	msg->addU32Fast    ( _PREHASH_ParentEstateID, 0						);
	msg->addUUIDFast   ( _PREHASH_RegionID, LLUUID::null				);
	msg->addVector3Fast( _PREHASH_Position, m_agentPosition				);
	msg->addBinaryDataFast(	_PREHASH_BinaryBucket,
							EMPTY_BINARY_BUCKET,
							EMPTY_BINARY_BUCKET_SIZE );
	SendReliable( msg );
}


void ManagerImpl::SendGroupChatLeaveRequest( const String& group_session_id )
{
	LLUUID group_uuid( group_session_id.GetString() );

	LLMessageSystem* msg = gMessageSystem;
	pack_instant_message(
		msg,
		m_agentId,
		FALSE,
		m_sessionId,
		group_uuid,
		m_fullName.c_str(), 
		LLStringUtil::null,
		IM_ONLINE,
		IM_SESSION_LEAVE,
		group_uuid );
	SendReliable( msg );
}


void ManagerImpl::OfferFriendship( const String& target_id, const String& message )
{
	LLMessageSystem* msg = gMessageSystem;
	pack_instant_message(
			msg,
			m_agentId,
			FALSE,
			m_sessionId,
			LLUUID(target_id.GetString()),
			m_fullName.c_str(),
			message.GetString(),
			IM_ONLINE,
			IM_FRIENDSHIP_OFFERED,
			m_rootInventoryFolder );	// TODO: Need correct calling card folder!
	SendReliable( msg );
}


void ManagerImpl::AcceptFriendship( const String& sessionId, const String& senderIp )
{
	LLMessageSystem* msg = gMessageSystem;
	//
	msg->newMessageFast( _PREHASH_AcceptFriendship			);
	msg->nextBlockFast ( _PREHASH_AgentData					);
	msg->addUUIDFast   ( _PREHASH_AgentID, m_agentId		);
	msg->addUUIDFast   ( _PREHASH_SessionID, m_sessionId	);
	msg->nextBlockFast ( _PREHASH_TransactionBlock			);
	msg->addUUIDFast   ( _PREHASH_TransactionID, LLUUID(sessionId.GetString()) );
	msg->nextBlockFast ( _PREHASH_FolderData			   	);
	msg->addUUIDFast   ( _PREHASH_FolderID, m_rootInventoryFolder  );	// TODO: need correct Calling Card folder!
	msg->sendReliable  ( LLHost( senderIp.GetString() ) );

	// Tell GUI to add to list
	//
	//m_friendAddSignal( String(m_agentId.asString().c_str()) );
}


void ManagerImpl::DeclineFriendship( const String& sessionId, const String& senderIp )
{
	LLMessageSystem* msg = gMessageSystem;
	//
	msg->newMessageFast( _PREHASH_DeclineFriendship			);
	msg->nextBlockFast ( _PREHASH_AgentData					);
	msg->addUUIDFast   ( _PREHASH_AgentID, m_agentId		);
	msg->addUUIDFast   ( _PREHASH_SessionID, m_sessionId	);
	msg->nextBlockFast ( _PREHASH_TransactionBlock			);
	msg->addUUIDFast   ( _PREHASH_TransactionID, LLUUID(sessionId.GetString()) );
	msg->sendReliable  ( LLHost( senderIp.GetString() ) );
}


void ManagerImpl::TerminateFriendship( const String& agentId )
{
	LLMessageSystem* msg = gMessageSystem;
	//
	msg->newMessageFast( _PREHASH_TerminateFriendship );
	msg->nextBlockFast( _PREHASH_AgentData );
	msg->addUUIDFast( _PREHASH_AgentID, m_agentId );
	msg->addUUIDFast( _PREHASH_SessionID, m_sessionId );
	msg->nextBlockFast( _PREHASH_ExBlock );
	msg->addUUIDFast( _PREHASH_OtherID, LLUUID( agentId.GetString() ) );
	//
	SendReliable( msg );
}


void ManagerImpl::AcceptGroupJoinOffer( const String& group_id, const String& message, const String& sessionId )
{
	LLUUID		groupId( group_id.GetString() );
	LLUUID		transactionId( sessionId.GetString() );
	//
	LLMessageSystem* msg = gMessageSystem;
	pack_instant_message(
			msg,
			m_agentId,
			FALSE,
			m_sessionId,
			groupId,
			m_fullName.c_str(),
			message.GetString(),
			IM_ONLINE,
			IM_GROUP_INVITATION_ACCEPT,
			transactionId 
		);
	SendReliable( msg );
}


void ManagerImpl::DeclineGroupJoinOffer( const String& group_id, const String& message, const String& sessionId )
{
	LLUUID		groupId( group_id.GetString() );
	LLUUID		transactionId( sessionId.GetString() );
	//	
	LLMessageSystem* msg = gMessageSystem;
	pack_instant_message(
			msg,
			m_agentId,
			FALSE,
			m_sessionId,
			groupId,
			m_fullName.c_str(),
			message.GetString(),
			IM_ONLINE,
			IM_GROUP_INVITATION_DECLINE,
			transactionId 
		);
	SendReliable( msg );
}


void ManagerImpl::LeaveGroupRequest( const String& group_id )
{
	LLUUID groupId( group_id.GetString() );
	LLMessageSystem *msg = gMessageSystem;
	//
	msg->newMessageFast( _PREHASH_LeaveGroupRequest        );
	msg->nextBlockFast ( _PREHASH_AgentData                );
	msg->addUUIDFast   ( _PREHASH_AgentID, m_agentId       );
	msg->addUUIDFast   ( _PREHASH_SessionID, m_sessionId   );
	msg->nextBlockFast ( _PREHASH_GroupData                );
	msg->addUUIDFast   ( _PREHASH_GroupID, groupId         );
	SendReliable( msg );
}


void ManagerImpl::SearchPeople( const String& fullname )
{
	m_peopleSearchResult.clear();
	m_searchId.generate();

	LLMessageSystem* msg = gMessageSystem;
	msg->newMessageFast	( _PREHASH_DirFindQuery				);
	msg->nextBlockFast	( _PREHASH_AgentData				);
	msg->addUUIDFast	( _PREHASH_AgentID, m_agentId		);
	msg->addUUIDFast   	( _PREHASH_SessionID, m_sessionId	);
	msg->nextBlockFast	( _PREHASH_QueryData				);
	msg->addUUIDFast	( _PREHASH_QueryID, m_searchId		);
	msg->addStringFast	( _PREHASH_QueryText, fullname.GetString() );
	msg->addU32Fast		( _PREHASH_QueryFlags, DFQ_PEOPLE	);
	msg->addS32Fast		( _PREHASH_QueryStart, 0			); // Start from first record of result set
	//
	SendReliable( msg );
}


int ManagerImpl::GetPeopleSearchCount() const
{
	return (int) m_peopleSearchResult.size();
}


String ManagerImpl::GetPerson( const int index ) const
{
	if( index > (int) m_peopleSearchResult.size() )
	{
		return "None found";
	}

	return String( m_peopleSearchResult[index].getString().c_str() );
}


void ManagerImpl::HandleCacheUpdate( const LLUUID& id, const std::string fullName, const bool is_group )
{
	m_cacheReceivedMap[id] = true;
	//
	if( is_group )
	{
		LLGroupData data;
		data.mID   = id;
		data.mName = fullName;
		m_groupMap[id] = data;
	}
	else
	{
		m_nameToIdMap[fullName] = id;
		m_idToNameMap[id] = fullName;
	}
	//
	m_cacheSignal	( String( id.getString().c_str() )
						, String( fullName.c_str() )
						, is_group
						);
}


void ManagerImpl::OnCacheNameCallback( const LLUUID& id, const std::string& firstname, const std::string& lastname, BOOL is_group, void* data )
{
	std::string fullName = firstname + " " + lastname;
	std::cout << "Cache arrived for " << id.asString().c_str() << ", name=" << fullName.c_str() << std::endl;
	HandleCacheUpdate( id, fullName, is_group );
}


void ManagerImpl::GetLM( std::string& region_name, S32& x, S32& y, S32& z ) const
{
	x = llround( (F32)fmod( (F64) m_agentPosition.mV[VX], (F64)REGION_WIDTH_METERS ) );
	y = llround( (F32)fmod( (F64) m_agentPosition.mV[VY], (F64)REGION_WIDTH_METERS ) );
	z = llround( (F32)m_agentPosition.mV[VZ] );
	region_name = m_viewerRegion->getName();
}


std::string ManagerImpl::GetSLURL() const
{
	std::string slurl;
	if( m_viewerRegion )
	{
		std::string region_name;
		S32 x, y, z;
		GetLM( region_name, x, y, z );
		slurl = buildSLURL(m_viewerRegion->getName(), x, y, z);
	}
	return slurl;
}


void ManagerImpl::TeleportViaLure( const LLUUID& lure_id )
{
	LLMessageSystem* msg = LLMessageSystem::getInstance();
	msg->newMessageFast( _PREHASH_TeleportLureRequest );
	msg->nextBlockFast( _PREHASH_Info );
	msg->addUUIDFast( _PREHASH_AgentID  , m_agentId );
	msg->addUUIDFast( _PREHASH_SessionID, m_sessionId );
	msg->addUUIDFast( _PREHASH_LureID   , lure_id);
	//
	// teleport_flags is a legacy field, now derived sim-side:
	//
	msg->addU32Fast( _PREHASH_TeleportFlags, TELEPORT_FLAGS_VIA_LURE );
	//
	Agent::SendReliable( msg );
}


void ManagerImpl::TeleportToRegion( const U64& region_handle, S32 x, S32 y, S32 z )
{
	LLVector3 pos_local( (F32) x, (F32) y, (F32) z );
	//
	LLMessageSystem* msg = gMessageSystem;
	msg->newMessageFast( _PREHASH_TeleportLocationRequest );
	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast( _PREHASH_AgentID, m_agentId );
	msg->addUUIDFast( _PREHASH_SessionID, m_sessionId );
	msg->nextBlockFast(_PREHASH_Info);
	msg->addU64Fast(_PREHASH_RegionHandle, region_handle);
	msg->addVector3Fast(_PREHASH_Position, pos_local);
	LLVector3 look_at(0,1,0);
	msg->addVector3(_PREHASH_LookAt, look_at);
	//
	SendReliable( msg );
}


void ManagerImpl::TeleportToRegion( const std::string& slurl )
{
	std::string sim_string = stripProtocol( slurl );
	m_x = 128;
	m_y = 128;
	m_z = 0;
	parse( sim_string, m_destRegionName, &m_x, &m_y, &m_z );

	std::cout << "Teleporting to " << m_destRegionName.c_str()
		<< ", (" << m_x << ", " << m_y << ", " << m_z << ")"
		<< std::endl;

	// We have to request a lookup from the server for the region name
	// to region code
	//
	LLMessageSystem* msg = LLMessageSystem::getInstance();
	msg->newMessageFast	( _PREHASH_MapNameRequest			);
	msg->nextBlockFast	( _PREHASH_AgentData				);
	msg->addUUIDFast	( _PREHASH_AgentID, m_agentId		);
	msg->addUUIDFast	( _PREHASH_SessionID, m_sessionId	);
	msg->addU32Fast		( _PREHASH_Flags, 0					);	// Not sure what is supposed to go there
	msg->addU32Fast		( _PREHASH_EstateID, 0				); // Filled in on sim
	msg->addBOOLFast	( _PREHASH_Godlike, FALSE			); // Filled in on sim
	msg->nextBlockFast	( _PREHASH_NameData					);
	msg->addStringFast	( _PREHASH_Name, m_destRegionName	);
	SendReliable( msg );

	m_teleportRequestedSignal( String(sim_string.c_str()) );
}


void ManagerImpl::TeleportHome()
{
	LLMessageSystem* msg = LLMessageSystem::getInstance();
	msg->newMessageFast(_PREHASH_TeleportLandmarkRequest);
	msg->nextBlockFast(_PREHASH_Info);
	msg->addUUIDFast(_PREHASH_AgentID, m_agentId );
	msg->addUUIDFast(_PREHASH_SessionID, m_sessionId );
	msg->addUUIDFast(_PREHASH_LandmarkID, LLUUID::null);
	SendReliable( msg );

	m_teleportRequestedSignal( String("Home") );
}


void ManagerImpl::OnMapBlockReply( LLMessageSystem* msg, void **user_data )
{
	U32 agent_flags;
	msg->getU32Fast(_PREHASH_AgentData, _PREHASH_Flags, agent_flags);

	if ((S32)agent_flags < 0 || agent_flags >= MAP_SIM_IMAGE_TYPES)
	{
		llwarns << "Invalid map image type returned! " << agent_flags << llendl;
		return;
	}

	std::cout << "Got MapBlockReply, waiting for name=" << m_destRegionName.c_str() << std::endl;

	S32 num_blocks = msg->getNumberOfBlocksFast(_PREHASH_Data);

	bool found_null_sim = false;

	for (S32 block=0; block<num_blocks; ++block)
	{
		U16 x_regions;
		U16 y_regions;
		std::string name;
		U8 accesscode;
		U32 region_flags;
		U8 water_height;
		U8 agents;
		LLUUID image_id;
		msg->getU16Fast(_PREHASH_Data, _PREHASH_X, x_regions, block);
		msg->getU16Fast(_PREHASH_Data, _PREHASH_Y, y_regions, block);
		msg->getStringFast(_PREHASH_Data, _PREHASH_Name, name, block);
		msg->getU8Fast(_PREHASH_Data, _PREHASH_Access, accesscode, block);
		msg->getU32Fast(_PREHASH_Data, _PREHASH_RegionFlags, region_flags, block);
		msg->getU8Fast(_PREHASH_Data, _PREHASH_WaterHeight, water_height, block);
		msg->getU8Fast(_PREHASH_Data, _PREHASH_Agents, agents, block);
		msg->getUUIDFast(_PREHASH_Data, _PREHASH_MapImageID, image_id, block);

		U32 x_meters = x_regions * REGION_WIDTH_UNITS;
		U32 y_meters = y_regions * REGION_WIDTH_UNITS;

		U64 handle = to_region_handle(x_meters, y_meters);

		if (accesscode == 255)
		{
			found_null_sim = true;
			std::cout << "Null sim" << std::endl;
		}
		else
		{
			if( stricmp( m_destRegionName.c_str(), name.c_str() ) == 0 )
			{
				TeleportToRegion( handle, m_x, m_y, m_z );
				break;
			}
		}
	}
}


void ManagerImpl::OnTeleportStart( LLMessageSystem* msg, void **user_data )
{
	U32 teleport_flags = 0x0;
	msg->getU32("Info", "TeleportFlags", teleport_flags);
	//
	bool can_cancel = true;
	if (teleport_flags & TELEPORT_FLAGS_DISABLE_CANCEL)
	{
		can_cancel = false;
	}

	m_teleportStartSignal( can_cancel );
}


void ManagerImpl::OnTeleportProgress( LLMessageSystem* msg, void **user_data )
{
	LLUUID agent_id;
	msg->getUUID("AgentData", "AgentID", agent_id);
	if( m_agentId != agent_id )
	{
		//LL_WARNS("Messaging") << "Unexpected teleport progress message." << LL_ENDL;
		return;
	}

	U32 teleport_flags = 0x0;
	msg->getU32("Info", "TeleportFlags", teleport_flags);
	bool can_cancel = true;
	if (teleport_flags & TELEPORT_FLAGS_DISABLE_CANCEL)
	{
		can_cancel = false;
	}

	std::string buffer;
	msg->getString("Info", "Message", buffer);
	//LL_DEBUGS("Messaging") << "teleport progress: " << buffer << LL_ENDL;

	String s_buffer( buffer.c_str() );
	m_teleportProgressSignal( can_cancel, s_buffer );
}


void ManagerImpl::OnTeleportFailed( LLMessageSystem* msg, void **user_data )
{
	std::string reason;
	msg->getStringFast( _PREHASH_Info, _PREHASH_Reason, reason );

	String s_buffer( reason.c_str() );
	m_teleportFailedSignal( s_buffer );
}


void ManagerImpl::OnTeleportLocal( LLMessageSystem* msg, void **user_data )
{
	LLUUID agent_id;
	msg->getUUIDFast(_PREHASH_Info, _PREHASH_AgentID, agent_id);
	if( agent_id != m_agentId )
	{
		//LL_WARNS("Messaging") << "Got teleport notification for wrong agent!" << LL_ENDL;
		return;
	}

	U32 location_id;
	LLVector3 /*pos,*/ look_at;
	U32 teleport_flags;
	msg->getU32Fast(_PREHASH_Info, _PREHASH_LocationID, location_id);
	msg->getVector3Fast(_PREHASH_Info, _PREHASH_Position, m_agentPosition);
	msg->getVector3Fast(_PREHASH_Info, _PREHASH_LookAt, look_at);
	msg->getU32Fast(_PREHASH_Info, _PREHASH_TeleportFlags, teleport_flags);

	SendCompleteAgentMovement( gHost );

	m_teleportLocalSignal();
}


void ManagerImpl::OnTeleportFinish( LLMessageSystem* msg, void **user_data )
{
	std::cout << "Got teleport location message" << std::endl;

	LLUUID agent_id;
	msg->getUUIDFast(_PREHASH_Info, _PREHASH_AgentID, agent_id);
	if( agent_id != m_agentId )
	{
		//LL_WARNS("Messaging") << "Got teleport notification for wrong agent!" << LL_ENDL;
		return;
	}

	// Get new sim info
	//
	U32 location_id;
	U32 sim_ip;
	U16 sim_port;
	LLVector3 /*pos,*/ look_at;
	U64 region_handle;
	//
	msg->getU32Fast(_PREHASH_Info, _PREHASH_LocationID, location_id);
	msg->getIPAddrFast(_PREHASH_Info, _PREHASH_SimIP, sim_ip);
	msg->getIPPortFast(_PREHASH_Info, _PREHASH_SimPort, sim_port);
	msg->getVector3Fast(_PREHASH_Info, _PREHASH_Position, m_agentPosition);
	msg->getVector3Fast(_PREHASH_Info, _PREHASH_LookAt, look_at);
	msg->getU64Fast(_PREHASH_Info, _PREHASH_RegionHandle, region_handle);
	U32 teleport_flags;
	msg->getU32Fast(_PREHASH_Info, _PREHASH_TeleportFlags, teleport_flags);
	
	std::string seedCap;
	msg->getStringFast(_PREHASH_Info, _PREHASH_SeedCapability, seedCap);

	LLHost sim_host( sim_ip, sim_port );

	// Viewer trusts the simulator.
	//
	msg->enableCircuit( sim_host, TRUE );
	//
	LLViewerRegionPtr regionp = LLWorld::getInstance()->addRegion( region_handle, sim_host );

	// now, use the circuit info to tell simulator about us!
	//
	std::cout << "process_teleport_finish() Enabling " << sim_host << " with code " << msg->mOurCircuitCode << std::endl;
	msg->newMessageFast(_PREHASH_UseCircuitCode);
	msg->nextBlockFast(_PREHASH_CircuitCode);
	msg->addU32Fast(_PREHASH_Code, msg->getOurCircuitCode());
	msg->addUUIDFast(_PREHASH_SessionID, m_sessionId );
	msg->addUUIDFast(_PREHASH_ID, m_agentId );
	msg->sendReliable(sim_host);

	regionp->setSeedCapability( seedCap );

	gHost.set( sim_ip, sim_port );
	//
	gAssetStorage->setUpstream( gHost );
	gCacheName   ->setUpstream( gHost );
	//
	m_viewerRegion = regionp;
	m_agent.setRegion( regionp );

	SendCompleteAgentMovement( gHost );

	m_teleportFinishSignal();
}


void ManagerImpl::OnProcessAgentMovementComplete( LLMessageSystem *msg, void **user_data )
{
	LLUUID agent_id;
	msg->getUUIDFast(_PREHASH_AgentData, _PREHASH_AgentID, agent_id);

	LLUUID session_id;
	msg->getUUIDFast(_PREHASH_AgentData, _PREHASH_SessionID, session_id);

	msg->getVector3Fast(_PREHASH_Data, _PREHASH_Position, m_agentPosition);

	LLVector3 look_at;
	msg->getVector3Fast(_PREHASH_Data, _PREHASH_LookAt, look_at);

	U64 region_handle;
	msg->getU64Fast(_PREHASH_Data, _PREHASH_RegionHandle, region_handle);

#if 0
	// WARNING!!!! This wrecks OpenSIM...there is something I am doing 
	// here that causes OpenSIM to crash other AV sessions if you are in the SIM
	// and they attempt to TP in. This is a serious bug with the server software
	// and at some point needs to be address. Right now, leave this remarked out
	// until the problem is resolved.
	//
	// Reply with AgentUpdate
	//
	LLCoordFrame frameAgent;
	frameAgent.setOrigin( 10.f, 10.f, 10.f );
	//
	LLVector3 look_dir;// ( /*LLViewerCamera::getInstance()->getAtAxis()*/ );
	LLVector3 up = look_dir % frameAgent.getLeftAxis();
	LLVector3 left = up % look_dir;
	LLQuaternion rot( look_dir, left, up );

	msg->newMessageFast(_PREHASH_AgentUpdate);
	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID, m_agentId);
	msg->addUUIDFast(_PREHASH_SessionID, m_sessionId);
	msg->addQuatFast(_PREHASH_BodyRotation, frameAgent.getQuaternion() );
	msg->addQuatFast(_PREHASH_HeadRotation, rot );
	//
	// TODO: Real values here please!
	//
	msg->addU8Fast(_PREHASH_State, 0);
	msg->addU8Fast(_PREHASH_Flags, 0);
	msg->addVector3Fast(_PREHASH_CameraCenter, LLVector3()   );	// It's possible that since I send (0,0,0)
	msg->addVector3Fast(_PREHASH_CameraAtAxis, LLVector3()   );	// that this could really screw up
	msg->addVector3Fast(_PREHASH_CameraLeftAxis, LLVector3() );	// OpenSIM.
	msg->addVector3Fast(_PREHASH_CameraUpAxis, LLVector3()   );
	msg->addF32Fast(_PREHASH_Far, 0.0 );
	msg->addU32Fast(_PREHASH_ControlFlags, 0 );
	SendReliable( msg );
#endif

	// Notify outside world
	//
	m_agentMovementCompleteSignal();
}


struct LLOfferInfo
{
	LLOfferInfo()
		: mIM(IM_NOTHING_SPECIAL)
		, mFromGroup(FALSE)
		, mFromObject(FALSE)
		, mType(LLAssetType::AT_NONE)
	{};

	//LLOfferInfo(const LLSD& sd);
	//void forceResponse(InventoryOfferResponse response);

	EInstantMessage mIM;
	LLUUID mFromID;
	BOOL mFromGroup;
	BOOL mFromObject;
	LLUUID mTransactionID;
	LLUUID mFolderID;
	LLUUID mObjectID;
	LLAssetType::EType mType;
	std::string mFromName;
	std::string mDesc;
	LLHost mHost;

	//LLSD asLLSD();
	//bool inventory_offer_callback(const LLSD& notification, const LLSD& response);
};


LLSD ManagerImpl::GetDetectQuery( const std::string& message )
{
	LLSD query;
	query["key"]		= GoogleTranslateResponder::GetApiKey();
	query["q"]			= message;
	query["v"]			= "1.0";

	return query;
}


LLSD ManagerImpl::GetTranslateQuery( const std::string& sourceLangId, const std::string& message )
{
	LLSD query;
	query["key"]		= GoogleTranslateResponder::GetApiKey();
	query["langpair"]	= sourceLangId + "|" + m_langId;
	query["q"]			= message;
	query["v"]			= "1.0";

	return query;
}


void ManagerImpl::HandleGroupChatDispatch( const LLSD& params )
{
	LLUUID group_id			= params["group_id"];
	std::string group_name	= params["group_name"];
	std::string agent_name	= params["agent_name"];
	bool has_me				= params["has_me"];
	std::string message		= params["source"];
	std::string translated	= params["message"];
	std::string language	= params["language"];

	String str_group_id( group_id.getString().c_str() );
	String str_name( group_name.c_str() );
	String str_agent( agent_name.c_str() );
	String str_message( message.c_str() );
	String str_translated( translated.c_str() );
	String str_language( language.c_str() );
	//
	m_groupChatSignal( str_group_id, str_name, str_agent, has_me, str_message, str_translated, str_language );
}


void ManagerImpl::HandleGroupChatDetect( const LLSD& params )
{
	if( params["isReliable"].asBoolean()
		&& (params["confidence"].asReal() > GOOGLE_XLATE_CONFIDENCE_CODE) )
	{
		const LLUUID	agent_id = params["from_id"].asUUID();
		std::string		language = params["language"].asString();
		if( GetAgentLanguageAuto( agent_id ) )
		{
			std::string full_name;
			GetNameFromCache( agent_id, full_name );
			//
			std::cout	<< "Setting "		<< language.c_str()
						<< " for agent "	<< agent_id.asString().c_str()
						<< ", "				<< full_name.c_str()
						<< std::endl;

			// Set the language for this agent, since it's reliable
			//
			SetAgentLanguage( agent_id, language );
		}
		else
		{
			language = GetAgentLanguage( agent_id );
		}
		//
		GoogleTranslateResponder* responder = new GoogleTranslateResponder( params );
		responder->ConnectArrivalSignal( boost::bind( &ManagerImpl::HandleGroupChatDispatch, this, _1 ) );
		//
		LLHTTPClient::get	( GoogleTranslateResponder::GetTranslateUrl()
							, GetTranslateQuery( language, params["source"].asString() )
							, responder
							);
	}
	else
	{
		LLSD _params( params );
		_params["language"]	= m_langId;
		_params["message"]	= params["source"];
		//
		HandleGroupChatDispatch( params );
	}
}


void ManagerImpl::HandleGroupChatTranslationResponse
								( const LLUUID& group_id
								, const LLUUID& from_id
								, const std::string& group_name
								, const std::string& agent_name
								, const bool has_me
								, const std::string& message
								)
{
	static int serialId = 0;

	LLSD params;
	params["group_id"]	= group_id;
	params["from_id"]	= from_id;
	params["group_name"]= group_name;
	params["agent_name"]= agent_name;
	params["has_me"]	= has_me;
	params["source"]	= message;
	params["serial_id"]	= serialId++;
	//
	GoogleDetectResponder* responder = new GoogleDetectResponder( params );
	responder->ConnectArrivalSignal( boost::bind( &ManagerImpl::HandleGroupChatDetect, this, _1 ) );
	//
	LLHTTPClient::get	( GoogleResponderBase::GetDetectUrl()
						, GetDetectQuery( message )
						, responder
						);
}


void ManagerImpl::HandleIMDispatch( const LLSD& params )
{
	LLUUID from_id			= params["from_id"];
	std::string name		= params["name"];
	bool has_me				= params["has_me"];
	std::string message		= params["source"];
	std::string translated	= params["message"];
	std::string language	= params["language"];

	String str_id( from_id.getString().c_str() );
	String str_name( name.c_str() );
	String str_message( message.c_str() );
	String str_translated( translated.c_str() );
	String str_language( language.c_str() );
	//
	m_imSignal( str_id, str_name, has_me, str_message, str_translated, str_language );
}


void ManagerImpl::HandleIMDetect( const LLSD& params )
{
	const bool isReliable = params["isReliable"].asBoolean();
	const F32 confidence  = params["confidence"].asReal();
	if( isReliable && (confidence > GOOGLE_XLATE_CONFIDENCE_CODE))
	{
		const LLUUID	agent_id = params["from_id"].asUUID();
		std::string		language = params["language"].asString();
		if( GetAgentLanguageAuto( agent_id ) )
		{
			std::string full_name;
			GetNameFromCache( agent_id, full_name );
			//
			std::cout	<< "Setting "		<< language.c_str()
						<< " for agent "	<< agent_id.asString().c_str()
						<< ", "				<< full_name.c_str()
						<< std::endl;

			// Set the language for this agent, since it's reliable
			//
			SetAgentLanguage( agent_id, language );
		}
		else
		{
			language = GetAgentLanguage( agent_id );
		}
		//
		GoogleTranslateResponder* responder = new GoogleTranslateResponder( params );
		responder->ConnectArrivalSignal( boost::bind( &ManagerImpl::HandleIMDispatch, this, _1 ) );
		//
		LLHTTPClient::get	( GoogleResponderBase::GetTranslateUrl()
							, GetTranslateQuery( language, params["source"] )
							, responder
							);
	}
	else
	{
		LLSD _params( params );
		_params["language"]	= m_langId;
		_params["message"]	= params["source"];
		//
		HandleIMDispatch( params );
	}
}


void ManagerImpl::HandleIMTranslationResponse	( const LLUUID& from_id
												, const std::string& name
												, const bool has_me
												, const std::string& message
												)
{
	static int serialId = 0;

	LLSD params;
	params["from_id"]	= from_id;
	params["name"]		= name;
	params["has_me"]	= has_me;
	params["source"]	= message;
	params["serial_id"]	= serialId++;
	//
	GoogleDetectResponder* responder = new GoogleDetectResponder( params );
	responder->ConnectArrivalSignal( boost::bind( &ManagerImpl::HandleIMDetect, this, _1 ) );
	//
	LLHTTPClient::get	( GoogleResponderBase::GetDetectUrl()
						, GetDetectQuery( message )
						, responder
						);
}


void ManagerImpl::HandleLocalChatDispatch( const LLSD& params )
{
	LLUUID from_id			= params["from_id"];
	std::string from_name	= params["from_name"];
	std::string verb		= params["verb"];
	bool has_me				= params["has_me"];
	std::string message		= params["source"];
	std::string translated	= params["message"];
	std::string language	= params["language"];

	String str_from_id( from_id.getString().c_str() );
	String str_name( from_name.c_str() );
	String str_verb( verb.c_str() );
	String str_message( message.c_str() );
	String str_translated( translated.c_str() );
	String str_language( language.c_str() );
	//
	m_localChatSignal( str_from_id, str_name, str_verb, has_me, str_message, str_translated, str_language );
}


void ManagerImpl::HandleLocalChatDetect( const LLSD& params )
{
	if( params["isReliable"].asBoolean()
		&& (params["confidence"].asReal() > GOOGLE_XLATE_CONFIDENCE_CODE) )
	{ 
		const LLUUID	agent_id = params["from_id"].asUUID();
		std::string		language = params["language"].asString();
		if( GetAgentLanguageAuto( agent_id ) )
		{
			std::string full_name;
			GetNameFromCache( agent_id, full_name );
			//
			std::cout	<< "Setting "		<< language.c_str()
						<< " for agent "	<< agent_id.asString().c_str()
						<< ", "				<< full_name.c_str()
						<< std::endl;

			// Set the language for this agent, since it's reliable
			//
			SetAgentLanguage( agent_id, language );
		}
		else
		{
			language = GetAgentLanguage( agent_id );
		}
		//
		GoogleTranslateResponder* responder = new GoogleTranslateResponder( params );
		responder->ConnectArrivalSignal( boost::bind( &ManagerImpl::HandleLocalChatDispatch, this, _1 ) );
		//
		LLHTTPClient::get	( GoogleResponderBase::GetTranslateUrl()
							, GetTranslateQuery( language, params["source"] )
							, responder
							);
	}
	else
	{
		LLSD _params( params );
		_params["language"]	= m_langId;
		_params["message"]	= params["source"];
		//
		HandleLocalChatDispatch( params );
	}
}


void ManagerImpl::HandleLocalChatTranslationResponse
								( const LLUUID& from_id
								, const std::string& from_name
								, const std::string& verb
								, const bool has_me
								, const std::string& message
								)
{
	static int serialId = 0;

	LLSD params;
	params["from_id"]	= from_id;
	params["from_name"] = from_name;
	params["verb"]		= verb;
	params["has_me"]	= has_me;
	params["source"]	= message;
	params["serial_id"]	= serialId++;
	//
	GoogleDetectResponder* responder = new GoogleDetectResponder( params );
	responder->ConnectArrivalSignal( boost::bind( &ManagerImpl::HandleLocalChatDetect, this, _1 ) );
	//
	LLHTTPClient::get	( GoogleResponderBase::GetDetectUrl()
						, GetDetectQuery( message )
						, responder
						);
}


/** /brief if string starts with "/me" then remove and return true
 * /param [in,out] str -- string with "/me" removed if found
 * /return true if starts with "/me"
 */
bool HasMe( std::string& str )
{
	bool has_me = false;
	const std::string me_str( "/me" );
	if( str.substr( 0, me_str.length() ) == me_str )
	{
		str = str.substr( me_str.length() );
		has_me = true;
	}
	//
	return has_me;
}


void ManagerImpl::OnProcessIMCallback( LLMessageSystem *msg, void **user_data )
{
	LLUUID from_id;
	BOOL from_group;
	LLUUID to_id;
	U8 offline;
	U8 d = 0;
	LLUUID session_id;
	U32 t;
	std::string name;
	std::string message;
	U32 parent_estate_id = 0;
	LLUUID region_id;
	LLVector3 position;
	U8 binary_bucket[MTUBYTES];
	S32 binary_bucket_size;
	LLChat chat;
	std::string buffer;

	msg->getUUIDFast(_PREHASH_AgentData, _PREHASH_AgentID, from_id);
	msg->getBOOLFast(_PREHASH_MessageBlock, _PREHASH_FromGroup, from_group);
	msg->getUUIDFast(_PREHASH_MessageBlock, _PREHASH_ToAgentID, to_id);
	msg->getU8Fast(  _PREHASH_MessageBlock, _PREHASH_Offline, offline);
	msg->getU8Fast(  _PREHASH_MessageBlock, _PREHASH_Dialog, d);
	msg->getUUIDFast(_PREHASH_MessageBlock, _PREHASH_ID, session_id);
	msg->getU32Fast( _PREHASH_MessageBlock, _PREHASH_Timestamp, t);
	msg->getStringFast(_PREHASH_MessageBlock, _PREHASH_FromAgentName, name);
	msg->getStringFast(_PREHASH_MessageBlock, _PREHASH_Message,		message);
	msg->getU32Fast(_PREHASH_MessageBlock, _PREHASH_ParentEstateID, parent_estate_id);
	msg->getUUIDFast(_PREHASH_MessageBlock, _PREHASH_RegionID, region_id);
	msg->getVector3Fast(_PREHASH_MessageBlock, _PREHASH_Position, m_agentPosition);
	msg->getBinaryDataFast(  _PREHASH_MessageBlock, _PREHASH_BinaryBucket, binary_bucket, 0, 0, MTUBYTES);
	binary_bucket_size = msg->getSizeFast(_PREHASH_MessageBlock, _PREHASH_BinaryBucket);
	EInstantMessage dialog = (EInstantMessage)d;
	//time_t timestamp = (time_t)t;

	switch( dialog )
	{
	case IM_NOTHING_SPECIAL:
		{
			const bool has_me = HasMe( message );
			//
			if( m_translateMessages )
			{
				HandleIMTranslationResponse( from_id, name, has_me, message );
			}
			else
			{
				LLSD params;
				params["from_id"] 	= from_id;
				params["name"]		= name;
				params["source"]	= message;
				params["has_me"]	= has_me;
				params["message"] 	= message;
				params["language"]	= m_langId;
				//
				HandleIMDispatch( params );
			}
		}
		break;

	case IM_SESSION_SEND:
		{
			const bool has_me = HasMe( message );
			std::string group_name = ll_safe_string( (char*) binary_bucket);
			// This is group chat
			//
			if( m_translateMessages && (from_id != m_agentId) )
			{
				HandleGroupChatTranslationResponse
					( session_id
					, from_id
					, group_name
					, name
					, has_me
					, message );
			}
			else
			{
				LLSD params;
				params["group_id"] 		= session_id;
				params["group_name"]	= group_name;
				params["agent_name"]	= name;
				params["has_me"]		= has_me;
				params["source"]		= message;
				params["message"] 		= message;
				params["language"]		= m_langId;
				//
				HandleGroupChatDispatch( params );
			}
		}
		break;

	case IM_TYPING_START:
		{
			String str_id( from_id.getString().c_str() );
			String str_name( name.c_str() );
			m_typingSignal( str_id, str_name, true );
		}
		break;

	case IM_TYPING_STOP:
		{
			String str_id( from_id.getString().c_str() );
			String str_name( name.c_str() );
			m_typingSignal( str_id, str_name, false );
		}
		break;

	case IM_FRIENDSHIP_OFFERED:
		{
			String str_id( from_id.getString().c_str() );
			String str_name( name.c_str() );
			String str_message( message.c_str() );
			String str_sessid( session_id.getString().c_str() );
			String str_ipport( msg->getSender().getIPandPort().c_str() );
			m_friendOfferSignal( str_id, str_name, str_message, str_sessid, offline == IM_ONLINE, str_ipport );
		}
		break;

	case IM_FRIENDSHIP_ACCEPTED:
		{
			// Send friendship signal
			//
			String str_id( from_id.getString().c_str() );
			String str_name( name.c_str() );
			m_friendAcceptSignal( str_id, str_name, false );

			// Now lookup the cache to get the cache signal back
			//
			std::string cache_name;
			gCacheName->getFullName( from_id, cache_name );
		}
		break;

	case IM_FRIENDSHIP_DECLINED:
		{
			String str_id( from_id.getString().c_str() );
			String str_name( name.c_str() );
			m_friendDeclineSignal( str_id, str_name, false );
		}
		break;

	case IM_MESSAGEBOX:
		{
			String str_message( message.c_str() );
			m_messageBoxSignal( str_message );
		}
		break;

	case IM_SESSION_INVITE:
	case IM_SESSION_P2P_INVITE:
	case IM_SESSION_GROUP_START:
	case IM_SESSION_CONFERENCE_START:
	case IM_SESSION_LEAVE:
		std::cout << "IM_SESSION_*" << std::endl;
		break;

	case IM_GROUP_INVITATION:
		{
			// Read the binary bucket for more information.
			struct invite_bucket_t
			{
				S32 membership_fee;
				LLUUID role_id;
			}* invite_bucket;
			//
			// Make sure the binary bucket is the correct size.
			if( binary_bucket_size != sizeof(invite_bucket_t) )
			{
				std::cerr << "Malformed group invite binary bucket" << std::endl;
				break;
			}
			//
			invite_bucket = (struct invite_bucket_t*) &binary_bucket[0];
			S32 membership_fee = ntohl(invite_bucket->membership_fee);

			String str_groupId( from_id.getString().c_str() );
			String str_sessid( session_id.getString().c_str() );
			String str_name( name.c_str() );
			String str_message( message.c_str() );

			// group_id, full_name, message, session id, fee_amount
			m_groupOfferSignal( str_groupId, str_name, str_message, str_sessid, membership_fee );
		}
		break;

	case IM_INVENTORY_OFFERED:
	case IM_TASK_INVENTORY_OFFERED:
		// Someone has offered us some inventory.
		{
			LLOfferInfo info;

			if (IM_INVENTORY_OFFERED == dialog)
			{
				struct offer_agent_bucket_t
				{
					S8		asset_type;
					LLUUID	object_id;
				}* bucketp;

				if (sizeof(offer_agent_bucket_t) != binary_bucket_size)
				{
					LL_WARNS("Messaging") << "Malformed inventory offer from agent" << LL_ENDL;
					break;
				}
				bucketp = (struct offer_agent_bucket_t*) &binary_bucket[0];
				info.mType = (LLAssetType::EType) bucketp->asset_type;
				info.mObjectID = bucketp->object_id;
			}
			else
			{
				if (sizeof(S8) != binary_bucket_size)
				{
					LL_WARNS("Messaging") << "Malformed inventory offer from object" << LL_ENDL;
					break;
				}
				info.mType = (LLAssetType::EType) binary_bucket[0];
				info.mObjectID = LLUUID::null;
			}

			info.mIM = dialog;
			info.mFromID = from_id;
			info.mFromGroup = from_group;
			info.mTransactionID = session_id;
			//info.mFolderID = gInventory.findCategoryUUIDForType(info.mType);

			if (dialog == IM_TASK_INVENTORY_OFFERED)
			{
				info.mFromObject = TRUE;
			}
			else
			{
				info.mFromObject = FALSE;
			}
			info.mFromName = name;
			info.mDesc = message;
			info.mHost = msg->getSender();
#if 0
			//if (((is_busy && !is_owned_by_me) || is_muted))
			if ( is_muted )
			{
				// Same as closing window
				info.forceResponse(IOR_DECLINE);
			}
			else
			{
				inventory_offer_handler(info, dialog == IM_TASK_INVENTORY_OFFERED);
			}
#endif
			//m_inventoryOfferedSignal( info.mFromName, info.mDesc );
		}
		break;

	case IM_GROUP_NOTICE:
	case IM_GROUP_NOTICE_REQUESTED:
		{
#if 0
			LL_INFOS("Messaging") << "Received IM_GROUP_NOTICE message." << LL_ENDL;
			// Read the binary bucket for more information.
			struct notice_bucket_header_t
			{
				U8 has_inventory;
				U8 asset_type;
				LLUUID group_id;
			};
			struct notice_bucket_full_t
			{
				struct notice_bucket_header_t header;
				U8 item_name[DB_INV_ITEM_NAME_BUF_SIZE];
			}* notice_bin_bucket;

			// Make sure the binary bucket is big enough to hold the header 
			// and a null terminated item name.
			if ( (binary_bucket_size < (S32)((sizeof(notice_bucket_header_t) + sizeof(U8))))
				|| (binary_bucket[binary_bucket_size - 1] != '\0') )
			{
				LL_WARNS("Messaging") << "Malformed group notice binary bucket" << LL_ENDL;
				break;
			}

			notice_bin_bucket = (struct notice_bucket_full_t*) &binary_bucket[0];
			U8 has_inventory = notice_bin_bucket->header.has_inventory;
			U8 asset_type = notice_bin_bucket->header.asset_type;
			LLUUID group_id = notice_bin_bucket->header.group_id;
			std::string item_name = ll_safe_string((const char*) notice_bin_bucket->item_name);

			// If there is inventory, give the user the inventory offer.
			LLOfferInfo* info = NULL;

			if (has_inventory)
			{
				info = new LLOfferInfo;

				info->mIM = IM_GROUP_NOTICE;
				info->mFromID = from_id;
				info->mFromGroup = from_group;
				info->mTransactionID = session_id;
				info->mType = (LLAssetType::EType) asset_type;
				info->mFolderID = gInventory.findCategoryUUIDForType(info->mType);
				std::string from_name;

				from_name += "A group member named ";
				from_name += name;

				info->mFromName = from_name;
				info->mDesc = item_name;
				info->mHost = msg->getSender();
			}

			std::string str(message);

			// Tokenize the string.
			// TODO: Support escaped tokens ("||" -> "|")
			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
			boost::char_separator<char> sep("|","",boost::keep_empty_tokens);
			tokenizer tokens(str, sep);
			tokenizer::iterator iter = tokens.begin();

			std::string subj(*iter++);
			std::string mes(*iter++);

			// Send the notification down the new path.
			// For requested notices, we don't want to send the popups.
			if (dialog != IM_GROUP_NOTICE_REQUESTED)
			{
				LLSD payload;
				payload["subject"] = subj;
				payload["message"] = mes;
				payload["sender_name"] = name;
				payload["group_id"] = group_id;
				payload["inventory_name"] = item_name;
				payload["inventory_offer"] = info ? info->asLLSD() : LLSD();

				LLSD args;
				args["SUBJECT"] = subj;
				args["MESSAGE"] = mes;
				LLNotifications::instance().add(LLNotification::Params("GroupNotice").substitutions(args).payload(payload).timestamp(timestamp));
			}

			// Also send down the old path for now.
			if (IM_GROUP_NOTICE_REQUESTED == dialog)
			{
				LLFloaterGroupInfo::showNotice(subj,mes,group_id,has_inventory,item_name,info);
			}
#endif
		}
		break;

	case IM_LURE_USER:
		m_teleportRequestSignal	( String( name.c_str() )
										, String( message.c_str() )
										, String( session_id.asString().c_str() )	// Session id is the LURE id
										);
		break;

	case IM_LURE_ACCEPTED:
	case IM_LURE_DECLINED:
		break;

	case IM_INVENTORY_ACCEPTED:
	case IM_INVENTORY_DECLINED:
	case IM_TASK_INVENTORY_ACCEPTED:
	case IM_TASK_INVENTORY_DECLINED:
	case IM_GROUP_VOTE:
	case IM_GROUP_MESSAGE_DEPRECATED:
	case IM_NEW_USER_DEFAULT:
	case IM_FROM_TASK:
	case IM_BUSY_AUTO_RESPONSE:
	case IM_CONSOLE_AND_CHAT_HISTORY:
	case IM_GODLIKE_LURE_USER:
	case IM_YET_TO_BE_USED:
	case IM_GROUP_ELECTION_DEPRECATED:
	case IM_GOTO_URL:
	case IM_FROM_TASK_AS_ALERT:
	case IM_GROUP_NOTICE_INVENTORY_ACCEPTED:
	case IM_GROUP_NOTICE_INVENTORY_DECLINED:
	case IM_GROUP_INVITATION_ACCEPT:
	case IM_GROUP_INVITATION_DECLINE:
	case IM_COUNT:
		// Not supported (yet)
		std::cout << "OTHER" << std::endl;
		break;
	}
}



void ManagerImpl::OnProcessLocalChat( LLMessageSystem *msg, void **user_data )
{
	std::string		mesg;
	std::string		from_name;
	U8				source_temp;
	U8				type_temp;
	U8				audible_temp;
	LLUUID			from_id;
	LLUUID			owner_id;

	msg->getStringFast(_PREHASH_ChatData, _PREHASH_FromName, from_name);
	msg->getUUIDFast(_PREHASH_ChatData, _PREHASH_SourceID, from_id);
	msg->getUUIDFast(_PREHASH_ChatData, _PREHASH_OwnerID, owner_id);

	msg->getU8Fast(_PREHASH_ChatData, _PREHASH_SourceType, source_temp);
	EChatSourceType	sourceType = (EChatSourceType)source_temp;

	msg->getU8Fast(_PREHASH_ChatData, _PREHASH_ChatType, type_temp);
	EChatType chatType = (EChatType)type_temp;

	msg->getU8Fast(_PREHASH_ChatData, _PREHASH_Audible, audible_temp);
	EChatAudible audibleType = (EChatAudible)audible_temp;

	// Audible
	//
	if( audibleType == CHAT_AUDIBLE_FULLY )
	{
		msg->getStringFast(_PREHASH_ChatData, _PREHASH_Message, mesg);

		bool publish = true;
		std::string verb;
		switch( chatType )
		{
		case CHAT_TYPE_WHISPER:
			verb = " whispers ";
			break;
		case CHAT_TYPE_DEBUG_MSG:
		case CHAT_TYPE_OWNER:
		case CHAT_TYPE_NORMAL:
			verb = ": ";
			break;
		case CHAT_TYPE_SHOUT:
			verb = " shouts ";
			break;
		case CHAT_TYPE_START:
		case CHAT_TYPE_STOP:
			//LL_WARNS("Messaging") << "Got chat type start/stop in main chat processing." << LL_ENDL;
			publish = false;
			break;
		default:
			//LL_WARNS("Messaging") << "Unknown type " << chat.mChatType << " in chat!" << LL_ENDL;
			//verb = " say, ";
			publish = false;
			break;
		}

		if( publish )
		{
			const bool has_me = HasMe( mesg );
			//
			if( m_translateMessages
				&& (sourceType == CHAT_SOURCE_AGENT)
				&& (from_id != m_agentId)
				)
			{
				HandleLocalChatTranslationResponse( from_id, from_name, verb, has_me, mesg );
			}
			else
			{
				LLSD params;
				params["from_id"] 	= from_id;
				params["from_name"]	= from_name;
				params["verb"]		= verb;
				params["has_me"]	= has_me;
				params["source"] 	= mesg;
				params["message"] 	= mesg;
				params["language"]	= m_langId;
				//
				HandleLocalChatDispatch( params );
			}
		}
	}
}


void ManagerImpl::OnOnlineNotifyCallback( LLMessageSystem *msg, void **user_data )
{
	S32 count = msg->getNumberOfBlocksFast( _PREHASH_AgentBlock );

	for(S32 i = 0; i < count; ++i)
	{
		LLUUID agent_id;
		msg->getUUIDFast( _PREHASH_AgentBlock, _PREHASH_AgentID, agent_id, i );

		m_onlineStatuses[agent_id] = true;
		if( m_cacheReceivedMap[agent_id] )
		{
			// Only send if we got the cache first
			//
			m_onlineSignal( agent_id.getString().c_str(), true );
		}
	}
}


void ManagerImpl::OnOfflineNotifyCallback( LLMessageSystem *msg, void **user_data )
{
	S32 count = msg->getNumberOfBlocksFast( _PREHASH_AgentBlock );

	for(S32 i = 0; i < count; ++i)
	{
		LLUUID agent_id;
		msg->getUUIDFast( _PREHASH_AgentBlock, _PREHASH_AgentID, agent_id, i );

		m_onlineStatuses[agent_id] = false;
		if( m_cacheReceivedMap[agent_id] )
		{
			// Only send if we got the cache first
			//
			m_onlineSignal( agent_id.getString().c_str(), false );
		}
	}
}


void ManagerImpl::OnTerminateFriendshipCallback( LLMessageSystem *msg, void **user_data )
{
	LLUUID id;
	msg->getUUIDFast( _PREHASH_ExBlock, _PREHASH_OtherID, id );
	m_terminateFriendshipSignal( id.getString().c_str() );

	// Pull these out of our local cache
	//
	m_cacheReceivedMap[id] = false;
	std::string fullName;
	//
	LLUUIDToString::iterator id_iter = m_idToNameMap.find( id );
	if( id_iter != m_idToNameMap.end() )
	{
		fullName = id_iter->second;
		//
		m_idToNameMap.erase( id_iter );
		//
		StringToLLUUID::iterator name_iter = m_nameToIdMap.find( fullName );
		if( name_iter != m_nameToIdMap.end() )
		{
			m_nameToIdMap.erase( name_iter );
		}
	}
}


void ManagerImpl::OnSearchResultCallback( LLMessageSystem *msg, void **user_data )
{
	LLUUID query_id;
	msg->getUUIDFast( _PREHASH_QueryData, _PREHASH_QueryID, m_searchId );

	S32 count = msg->getNumberOfBlocksFast( _PREHASH_QueryReplies );
	//
	for( S32 i = 0; i < count; ++i )
	{
		LLUUID agent_id;
		std::string first_name, last_name;
		//
		msg->getStringFast( _PREHASH_QueryReplies,	_PREHASH_FirstName,	first_name, i );
		msg->getStringFast( _PREHASH_QueryReplies,	_PREHASH_LastName,	last_name,	i );
		msg->getUUIDFast  ( _PREHASH_QueryReplies,	_PREHASH_AgentID,	   agent_id,	i );

		// Save it into our search result list
		//
		m_peopleSearchResult.push_back( agent_id );

		// Shove it into our cache for later
		//
		std::string fullname = first_name + " " + last_name;
		//m_nameToIdMap[fullname] = agent_id;

		// Send the signal that we got this one
		//
		m_searchResultSignal( String( agent_id.getString().c_str() ), String( fullname.c_str() ) );
	}

	// Signal that the search result is in
	//
	//m_searchResultSignal();
}


void ManagerImpl::OnLogoutCallback( LLMessageSystem *msg, void **user_data )
{
	std::cout << "Log out reply received..." << std::endl;

	LLUUID agent_id;
	msg->getUUIDFast( _PREHASH_AgentData, _PREHASH_AgentID, agent_id );
	//
	LLUUID session_id;
	msg->getUUIDFast( _PREHASH_AgentData, _PREHASH_SessionID, session_id );

	const S32 count = msg->getNumberOfBlocksFast( _PREHASH_InventoryData );
	for(S32 i = 0; i < count; ++i)
	{
		LLUUID item_id;
		msg->getUUIDFast(_PREHASH_InventoryData, _PREHASH_ItemID, item_id, i);
	}

	m_logoutReplySignal();
}


void ManagerImpl::OnAgentDataUpdate( LLMessageSystem *msg, void **user_data )
{
	LLUUID	agent_id;

	msg->getUUIDFast( _PREHASH_AgentData, _PREHASH_AgentID, agent_id );
	//
	if (agent_id != m_agentId)
	{
		return;
	}

	msg->getStringFast( _PREHASH_AgentData, _PREHASH_GroupTitle, m_groupTitle );
	LLUUID active_id;
	msg->getUUIDFast( _PREHASH_AgentData, _PREHASH_ActiveGroupID, active_id );

	if( active_id.notNull() )
	{
		m_groupId = active_id;
		msg->getU64( _PREHASH_AgentData, "GroupPowers", m_groupPowers );
		msg->getString( _PREHASH_AgentData, _PREHASH_GroupName, m_groupName );
	}
	else
	{
		m_groupId.setNull();
		m_groupPowers = 0;
		m_groupName.clear();
	}		

	//update_group_floaters(active_id);
}


void ManagerImpl::OnAgentGroupDataUpdate( LLMessageSystem *msg, void **user_data )
{
	LLUUID	agent_id;

	std::cout << "ManagerImpl::OnAgentGroupDataUpdate()" << std::endl;

	msg->getUUIDFast( _PREHASH_AgentData, _PREHASH_AgentID, agent_id );

	if( agent_id != m_agentId )
	{
		llwarns << "processAgentGroupDataUpdate for agent other than me" << llendl;
		return;
	}	

	S32 count = msg->getNumberOfBlocksFast( _PREHASH_GroupData );
	LLGroupData group;
	//S32 index = -1;
	//bool need_floater_update = false;
	for(S32 i = 0; i < count; ++i)
	{
		msg->getUUIDFast( _PREHASH_GroupData, _PREHASH_GroupID, group.mID, i );
		msg->getUUIDFast( _PREHASH_GroupData, _PREHASH_GroupInsigniaID, group.mInsigniaID, i );
		msg->getU64( _PREHASH_GroupData, "GroupPowers", group.mPowers, i );
		msg->getBOOL( _PREHASH_GroupData, "AcceptNotices", group.mAcceptNotices, i );
		msg->getS32( _PREHASH_GroupData, "Contribution", group.mContribution, i );
		msg->getStringFast( _PREHASH_GroupData, _PREHASH_GroupName, group.mName, i );

		if(group.mID.notNull())
		{
			std::cout << "Belongs to group " << group.mName.c_str() << std::endl;
			m_groupMap[group.mID] = group;

			m_groupCacheSignal( String(group.mID.getString().c_str()), String(group.mName.c_str()) );
		}
	}
}


void ManagerImpl::OnAgentWearablesUpdate( LLMessageSystem *msg, void **user_data )
{
	std::cout << "ManagerImpl::OnAgentWearablesUpdate()" << std::endl;
	//
	// WARNING!!!! This causes big problems with OpenSIM...don't use this!
	//
	//m_agent.AgentWearablesUpdate( msg );
}


void ManagerImpl::OnAgentCachedTextureResponse( LLMessageSystem* msg, void **user_data )
{
	std::cout << "ManagerImpl::OnAgentCachedTextureResponse()" << std::endl;
	//
	// WARNING!!!! This causes big problems with OpenSIM...don't use this!
	//
	//m_agent.AgentCachedTextureResponse( msg );
}


void ManagerImpl::OnKickUserCallback( LLMessageSystem *msg, void **user_data )
{
	std::cout << "Kicked from sim!" << std::endl;

	std::string message;
	msg->getStringFast( _PREHASH_UserInfo, _PREHASH_Reason, message );

	m_forceQuitSignal( LLC::String( message.c_str() ) );
}


void ManagerImpl::GetNameFromCache( const LLUUID& agent_id, std::string& first_name, std::string& last_name )
{
	gCacheName->getName( agent_id, first_name, last_name );
}


void ManagerImpl::GetNameFromCache( const LLUUID& id, std::string& full_name )
{
	std::string first_name, last_name;
	GetNameFromCache( id, first_name, last_name );
	//
	full_name = first_name + std::string(" ") + last_name;
}


String ManagerImpl::GetFullName( const String& id )
{
	LLUUID agent_id( id.GetString() );
	std::string name;
	name = m_idToNameMap[agent_id];
	return String( name.c_str() );
}


String ManagerImpl::LookupId( const String& fullname )
{
	std::string id = m_nameToIdMap[fullname.GetString()].getString();
	return String( id.c_str() );
}


String ManagerImpl::LookupGroupName( const String& id )
{
	LLUUID group_id( id.GetString() );
	std::string groupName = m_groupMap[group_id].mName;
	if( groupName.empty() )
	{
		gCacheName->getGroupName( group_id, groupName );
		m_groupMap[group_id].mName = groupName;
	}
	//
	return String( groupName.c_str() );
}


bool ManagerImpl::IsOnline( const String& id )
{
	LLUUID agent_id( id.GetString() );
	return m_onlineStatuses[agent_id];
} 

bool ManagerImpl::IsFriend( const String& id )
{
	LLUUID agent_id( id.GetString() );
	return m_cacheReceivedMap[agent_id];
}


void ManagerImpl::UpdateLocalAvatars()
{
	LLViewerRegion::LLUUIDList	nearAvs;
	m_viewerRegion->getAvatars( nearAvs, m_agent.getPosGlobalFromAgent( m_agentPosition ), CHAT_NORMAL_RADIUS );

	if( nearAvs != m_nearAvatars )
	{
		m_nearAvatars = nearAvs;
		m_localAgentsPresentSignal();
	}
}


int ManagerImpl::GetLocalAvatarCount() const
{
	return static_cast<int>(m_nearAvatars.size());
}


String ManagerImpl::GetLocalAvatar( const int idx ) const
{
	if( idx < 0 || idx >= static_cast<int>(m_nearAvatars.size()) )
	{
		return String( "out of range" );
	}
	//
	return String(m_nearAvatars[idx].asString().c_str());
}


void ManagerImpl::SendGroupChatInvite( const LLUUID& groupId, const std::string& fromName, const std::string& message )
{
	m_groupChatSignal		( String(groupId.getString().c_str())
							, String(m_groupMap[groupId].mName.c_str())
							, String(fromName.c_str())
							, false
							, String(message.c_str())
							, String(message.c_str())
							, String(m_langId.c_str())
							);
}


std::string	ManagerImpl::GetAgentLanguage( const LLUUID& agentId ) const
{
	if( m_agentLang.find( agentId ) == m_agentLang.end() )
	{
		// Default to true...
		//
		m_agentLang[agentId]["auto"] = true;
	}
	//
	if( agentId == m_agentId )
	{
		// Always fix to local lang if it us who is logged in.
		//
		m_agentLang[agentId]["language"] = m_langId;
	}
	//
	return m_agentLang[agentId]["language"].asString();
}


void ManagerImpl::SetAgentLanguage( const LLUUID& agentId, const std::string& language )
{
	m_agentLang[agentId]["language"] = language;
}


bool ManagerImpl::GetAgentLanguageAuto( const LLUUID& agentId ) const
{
	if( m_agentLang.find( agentId ) == m_agentLang.end() )
	{
		// Default to true...
		//
		m_agentLang[agentId]["auto"] = true;
	}
	//
	return m_agentLang[agentId]["auto"].asBoolean();
}


void ManagerImpl::SetAgentLanguageAuto( const LLUUID& agentId, const bool val )
{
	m_agentLang[agentId]["auto"] = val;
}


void ManagerImpl::SendGroupChatAgentUpdateSignal( const std::string& session_id, const std::string& agent_id, const bool entering )
{
	m_groupChatAgentUpdateSignal( LLC::String(session_id.c_str()), LLC::String(agent_id.c_str()), entering );
}


}
//namespace LLC

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
