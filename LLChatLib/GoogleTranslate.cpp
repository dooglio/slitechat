#include "GoogleTranslate.h"
#include "ManagerImpl.h"

// llcommon
//
#include "llsdserialize.h"

// llmessage
//
#include "llbufferstream.h"


namespace LLC
{

const char* GoogleResponderBase::m_detectUrl	= "http://ajax.googleapis.com/ajax/services/language/detect";
const char* GoogleResponderBase::m_translateUrl	= "http://ajax.googleapis.com/ajax/services/language/translate";
const char* GoogleResponderBase::m_apiKey		= "ABQIAAAAGUEOxhujjY-VWquFTRbTWBQSeCpE_9msdrqpT5NdIC7cv9X-JhQWcGFoVQ9jJILbnHYfRy0uoeevbw";


GoogleResponderBase::GoogleResponderBase( const LLSD& params )
	: m_params(params)
{
}


void GoogleResponderBase::completedRaw(
	U32 status,
	const std::string& reason,
	const LLChannelDescriptors& channels,
	const LLIOPipe::buffer_ptr_t& buffer)
{
	LLSD content;
	LLBufferStream istr( channels, buffer.get() );
	LLSDSerialize::fromJSON( content, istr );
	completed( status, reason, content );
}


void GoogleDetectResponder::result( const LLSD& content )
{
	std::string language	= content["responseData"]["language"]	.asString();
	bool		isReliable	= content["responseData"]["isReliable"]	.asBoolean();
	F32			confidence	= content["responseData"]["confidence"]	.asReal();
	std::cout
			<< "Response from Google: language=" << language.c_str()
			<< ", isReliabel=" << isReliable
			<< ", confidence=" << confidence
			<< std::endl;

	// Add message and language results
	//
	LLSD params = m_params;
	params["language"]   = language;
	params["isReliable"] = isReliable;
	params["confidence"] = confidence;
	m_arrivalSignal( params );
}


void GoogleDetectResponder::error( U32 statusNum, const std::string& reason )
{		
	std::cout << "GoogleDetectResponder::error(): " << reason.c_str() << std::endl;
	//
	// TODO: Emit failure signal
	//
}


void GoogleTranslateResponder::result( const LLSD& content )
{
	LLSD params = m_params;
	//
	std::string translatedText	= content["responseData"]["translatedText"]			.asString();
	std::string	detected_lang	= content["responseData"]["detectedSourceLanguage"]	.asString();
	std::string language		= LLC::ManagerImpl::GetInstance()->GetAgentLanguage( params["from_id"].asUUID() );
	std::cout
			<< "Response from Google: translatedText=" << translatedText.c_str()
			<< ", detected_language=" << detected_lang.c_str()
			<< ", language=" << language.c_str()
			<< std::endl;

	// Add message and language results
	//
	params["message"]			= translatedText;
	params["detected_language"]	= detected_lang;
	params["language"]			= language;
	//
	m_arrivalSignal( params );
}


void GoogleTranslateResponder::error( U32 statusNum, const std::string& reason )
{		
	std::cout << "GoogleTranslateResponder::error(): " << reason.c_str() << std::endl;
	//
	// TODO: Emit failure signal
	//
}


}
//namespace LLC

// vim: ts=4 sw=4
