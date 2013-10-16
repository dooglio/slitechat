/** 
 * \brief Implements the StringImpl class, and the String methods.
 *
 * Copyright (c) 2009-2010 by R. Douglas Barbieri
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
 */
#ifndef __STRINGIMPL_H__
#define __STRINGIMPL_H__

#include "LLChatLib.h"

#include <string>


namespace LLC
{


/**
 * \class StringImpl
 *
 * Used to wrap an STL string instance.
 *
 * This class is used by this library to allow exchange of strings between internals and public interface.
 * It was created because under Win32 VC++, STL classes are not marked properly for export from a DLL.
 * To avoid this issue, a PImpl was created to hide the string class object from the interface, and
 * deal only with pointer to chars.
 */

class LLCHATLIBEXP	StringImpl
{
public:
	StringImpl() {}
	StringImpl( const char* str ) : m_string(str)
	{
	}

	const char* GetString() const			 { return m_string.c_str(); }
	void        SetString( const char* str ) { m_string = str; }

private:
	std::string	m_string;
};

}

#endif //__STRINGIMPL_H__


// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
