/** 
 * \brief Misc utilities for use in SLiteChat
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

#ifndef UTILITY_H__
#define UTILITY_H__

#include "LLChatLib.h"
#include <QString>

LLC::String QStringToLLCString( const QString& qstr );
QString LLCStringToQString( const LLC::String& llcstr );

inline LLC::String Q2LS( const QString& qstr )		{ return QStringToLLCString(qstr); }
inline QString LS2Q( const LLC::String& llcstr )	{ return LLCStringToQString(llcstr); }

QString GetSystemLangCode();

#endif // UTILITY_H__

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
