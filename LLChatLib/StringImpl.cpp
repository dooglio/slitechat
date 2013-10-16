/** 
 * \brief Implements the String methods.
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

#include "StringImpl.h"

namespace LLC
{

/**
 * \brief Constructor
 *
 * Creates a new internal instance of the StringImp class, which is reference counted.
 */
String::String() : m_instance( new StringImpl )
{
}

/**
 * \brief Constructor
 *
 * Creates a new internal instance of the StringImp class, which is reference counted,
 * but copies the string.
 *
 * \param str [in]	Pointer to buffer of chars containing string, null delimited.
 */
String::String( const char* str ) : m_instance( new StringImpl(str) )
{
}

/**
 * \brief Destructor
 *
 * Destroys the string object. Trivial method.
 */
String::~String()
{
}

/**
 * \brief Returns a buffer containing the underlying string.
 *
 * \return Pointer to buffer of chars containing string, null delimited.
 */
const char* String::GetString() const
{
	return m_instance->GetString();
}

/**
 * \brief Set the underlying string.
 *
 * \param str [in]	Pointer to buffer of chars, null delimited, to copy.
 */
void String::SetString( const char* str )
{
	m_instance->SetString( str );
}


}
//namespace LLC

// vim: ts=4 sw=4 noexpandtab

