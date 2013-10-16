/** 
 * \brief List of well known grids
 *
 * Copyright (c) 2010 by R. Douglas Barbieri
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

#ifndef GRIDLIST_H
#define GRIDLIST_H

namespace LLC
{

class GridList
{
public:
	static int count();
	static int number() { return count(); }	// Deprecated method
	static const char *	name(int index);
	static const char *	url(int index);
	static const int	find( const char* name );

private:
	static struct s_grid
	{
		const char *grid_name;
		const char *grid_url;
	} grid[];
};

}
//namespace LLC

#endif

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
