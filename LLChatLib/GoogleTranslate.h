#ifndef GOOGLETRANSLATE_H
#define GOOGLETRANSLATE_H

// stdc++
//
#include <string>

// boost
//
#include <boost/signals.hpp>

// llcommon
//
#include "llsd.h"
#include "lluuid.h"

// llmessage
//
#include "llbuffer.h"
#include "llhttpclient.h"
#include "lliopipe.h"


namespace LLC
{


class GoogleResponderBase
	: public LLHTTPClient::Responder
{
public:
	GoogleResponderBase( const LLSD& params );
	virtual ~GoogleResponderBase() {}

	// LLHTTPClient::Responder
	//
	void completedRaw(
		U32 status,
		const std::string& reason,
		const LLChannelDescriptors& channels,
		const LLIOPipe::buffer_ptr_t& buffer);

	typedef boost::signal<void (LLSD)> ArrivalSignal;
	boost::signals::connection ConnectArrivalSignal( const ArrivalSignal::slot_type& slot )
		{ return m_arrivalSignal.connect( slot ); }

	static const char* GetApiKey()			{ return m_apiKey; }
	static const char* GetDetectUrl()		{ return m_detectUrl; }
	static const char* GetTranslateUrl()	{ return m_translateUrl; }

protected:
	LLSD			m_params;			// Load these up to what your subclass will see
	ArrivalSignal	m_arrivalSignal;

private:
	static const char*	m_translateUrl;
	static const char*	m_detectUrl;
	static const char*	m_apiKey;
};


class GoogleDetectResponder
	: public GoogleResponderBase
{
public:
	GoogleDetectResponder( const LLSD& params ) : GoogleResponderBase( params ) {}

	virtual void result(const LLSD& content);
	virtual void error( U32 statusNum, const std::string& reason );
};


class GoogleTranslateResponder
	: public GoogleResponderBase
{
public:
	GoogleTranslateResponder( const LLSD& params ) : GoogleResponderBase( params ) {}

	virtual void result(const LLSD& content);
	virtual void error( U32 statusNum, const std::string& reason );
};


}
//namespace LLC

#endif // GOOGLETRANSLATE_H

// vim: ts=4 sw=4
