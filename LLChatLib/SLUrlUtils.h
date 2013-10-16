#ifndef SLURLUTILS_H__
#define SLURLUTILS_H__

// STDC++
//
#include <string>

// llcommon
//
#include "stdtypes.h"

namespace LLC
{
	std::string escapeURL(const std::string& url);

	std::string buildSLURL(const std::string& regionname, S32 x, S32 y, S32 z);

	std::string unescapeRegionName(std::string region_name);

	std::string::size_type parseGridIdx(const std::string& in_string,
		std::string::size_type idx0,
		std::string::size_type* res);

	bool UrlParse(const std::string& sim_string,
		std::string *region_name,
		S32 *x, S32 *y, S32 *z);

	std::string xml_escape_string(const std::string& in);

	bool parse(const std::string& sim_string,
					   std::string& region_name,
					   S32 *x, S32 *y, S32 *z);

	bool matchPrefix(const std::string& url, const std::string& prefix);
	std::string stripProtocol(const std::string& url);
}
// namespace LLC

#endif // SLURLUTILS_H__