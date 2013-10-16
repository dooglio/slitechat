#include "SLUrlUtils.h"

// STDC++
//
#include <sstream>

// CURL
//
#include <curl/curl.h>

// llcommon
//
#include "llformat.h"
#include "llstring.h"

namespace LLC
{

const std::string SLURL_SL_HELP_PREFIX		= "secondlife://app.";
const std::string SLURL_SL_PREFIX			= "sl://";
const std::string SLURL_SECONDLIFE_PREFIX	= "secondlife://";
const std::string SLURL_SLURL_PREFIX		= "http://slurl.com/secondlife/";
const std::string SLURL_APP_TOKEN			= "app/";

// static
std::string escapeURL(const std::string& url)
{
	// The CURL curl_escape() function escapes colons, slashes,
	// and all characters but A-Z and 0-9.  Do a cheesy mini-escape.
	std::string escaped_url;
	S32 len = url.length();
	for (S32 i = 0; i < len; i++)
	{
		char c = url[i];
		if (c == ' ')
		{
			escaped_url += "%20";
		}
		else if (c == '\\')
		{
			escaped_url += "%5C";
		}
		else
		{
			escaped_url += c;
		}
	}
	return escaped_url;
}

// static
std::string buildSLURL(const std::string& regionname, S32 x, S32 y, S32 z)
{
	std::string slurl = SLURL_SLURL_PREFIX + regionname + llformat("/%d/%d/%d",x,y,z); 
	slurl = escapeURL( slurl );
	return slurl;
}


std::string unescapeRegionName(std::string region_name)
{
	std::string result;
	char* curlstr = curl_unescape(region_name.c_str(), region_name.size());
	result = std::string(curlstr);
	curl_free(curlstr);
	return result;
}


std::string::size_type parseGridIdx(const std::string& in_string,
											 std::string::size_type idx0,
											 std::string::size_type* res)
{
	if (idx0 == std::string::npos || in_string[idx0] != '/')
	{
		return std::string::npos; // parse error
	}
	idx0++;
	std::string::size_type idx1 = in_string.find_first_of('/', idx0);
	std::string::size_type len = (idx1 == std::string::npos) ? std::string::npos : idx1-idx0;
	std::string tstring = in_string.substr(idx0,len);
	if (!tstring.empty())
	{
		std::string::size_type val = atoi(tstring.c_str());
		*res = val;
	}
	return idx1;
}

bool UrlParse(const std::string& sim_string,
						   std::string *region_name,
						   S32 *x, S32 *y, S32 *z)
{
	// strip any bogus initial '/'
	std::string::size_type idx0 = sim_string.find_first_not_of('/');
	if (idx0 == std::string::npos) idx0 = 0;

	std::string::size_type idx1 = sim_string.find_first_of('/', idx0);
	std::string::size_type len = (idx1 == std::string::npos) ? std::string::npos : idx1-idx0;
	std::string tstring = sim_string.substr(idx0,len);
	*region_name = unescapeRegionName(tstring);
	if (!region_name->empty())
	{
		// return position data if found. otherwise leave passed-in values alone. (DEV-18380) -MG
		if (idx1 != std::string::npos)
		{
			std::string::size_type xs = *x, ys = *y, zs = *z;
			idx1 = parseGridIdx(sim_string, idx1, &xs);
			idx1 = parseGridIdx(sim_string, idx1, &ys);
			idx1 = parseGridIdx(sim_string, idx1, &zs);
			*x = xs;
			*y = ys;
			*z = zs;

			return true;
		}
	}
	//
	return false;
}


std::string xml_escape_string(const std::string& in)
{
	std::ostringstream out;
	std::string::const_iterator it = in.begin();
	std::string::const_iterator end = in.end();
	for(; it != end; ++it)
	{
		switch((*it))
		{
		case '<':
			out << "&lt;";
			break;
		case '>':
			out << "&gt;";
			break;
		case '&':
			out << "&amp;";
			break;
		case '\'':
			out << "&apos;";
			break;
		case '"':
			out << "&quot;";
			break;
		default:
			out << (*it);
			break;
		}
	}
	return out.str();
}


bool parse(const std::string& sim_string,
				   std::string& region_name,
				   S32 *x, S32 *y, S32 *z)
{
	// strip any bogus initial '/'
	std::string::size_type idx0 = sim_string.find_first_not_of('/');
	if (idx0 == std::string::npos) idx0 = 0;

	std::string::size_type idx1 = sim_string.find_first_of('/', idx0);
	std::string::size_type len = (idx1 == std::string::npos) ? std::string::npos : idx1-idx0;
	std::string tstring = sim_string.substr(idx0,len);
	region_name = unescapeRegionName(tstring);
	if (!region_name.empty())
	{
		// return position data if found. otherwise leave passed-in values alone. (DEV-18380) -MG
		if (idx1 != std::string::npos)
		{
			std::string::size_type xs = *x, ys = *y, zs = *z;
			idx1 = parseGridIdx(sim_string, idx1, &xs);
			idx1 = parseGridIdx(sim_string, idx1, &ys);
			idx1 = parseGridIdx(sim_string, idx1, &zs);
			*x = xs;
			*y = ys;
			*z = zs;
		}
		
		return true;
	}
	else
	{
		return false;
	}
}


bool matchPrefix(const std::string& url, const std::string& prefix)
{
	std::string test_prefix = url.substr(0, prefix.length());
	LLStringUtil::toLower(test_prefix);
	return test_prefix == prefix;
}


std::string stripProtocol(const std::string& url)
{
	std::string stripped = url;
	if (matchPrefix(stripped, SLURL_SL_HELP_PREFIX))
	{
		stripped.erase(0, SLURL_SL_HELP_PREFIX.length());
	}
	else if (matchPrefix(stripped, SLURL_SL_PREFIX))
	{
		stripped.erase(0, SLURL_SL_PREFIX.length());
	}
	else if (matchPrefix(stripped, SLURL_SECONDLIFE_PREFIX))
	{
		stripped.erase(0, SLURL_SECONDLIFE_PREFIX.length());
	}
	else if (matchPrefix(stripped, SLURL_SLURL_PREFIX))
	{
		stripped.erase(0, SLURL_SLURL_PREFIX.length());
	}
	return stripped;
}

}
// namespace LLC