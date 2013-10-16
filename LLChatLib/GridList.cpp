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

#include "GridList.h"

#include <string>

// Public namespace
//
namespace LLC
{


// Static internal data arrays, hidden by the GridList class
struct GridList::s_grid GridList::grid[] =
{
	{ "3rd rock",				"http://grid.3rdrockgrid.com:8002/" },
	{ "Cyber Landia",			"http://grid.cyberlandia.net:8002/" },
	{ "Franco Grid",			"http://grid.francogrid.com:8002/"},
	{ "Fw Estates",				"http://login.fw-estates.com/"},
	{ "Giant Grid",				"http://Gianttest.no-ip.biz:8002/" },
	{ "Helios",					"http://heliosgrid.no-ip.org:8002/" },
	{ "Le monde de darwin",		"http://94.23.8.158:8002/" },
	{ "Legend City Online",		"http://login.legendcityonline.com/" },
	{ "localhost",				"http://127.0.0.1:9000/" },
	{ "New World Grid",			"http://grid.newworldgrid.com:8002/" },
	{ "Nexxt Life",				"http://login.nexxtlife.com/" },
	{ "Open Life",				"http://login.talentraspel.de/" },
	{ "Osgrid",					"http://osgrid.org:8002/" },
	{ "Reaction Grid",			"http://reactiongrid.com:8008/" },
	{ "Second Life",			"https://login.agni.lindenlab.com/cgi-bin/login.cgi" },
	{ "The Gor Grid",			"http://thegorgrid.com:8002/" },
	{ "Virtual Sims",			"http://virtualsims.net:8002/" },
	{ "Virt You",				"http://virtyou.com:11002/" },
	{ "World Sim Terra",		"http://wsterra.com:8002/" },
	{ "Xumeo",					"http://login.xumeo.com/" },
	{ "Your alternative life",	"http://grid.youralternativelife.com/" }
};


/*! \brief Get number of grids in list
 */
int GridList::count()
{
	return sizeof(grid) / sizeof(struct s_grid);
}


/*! \brief Get name of nth grid
 */
const char *GridList::name(int index)
{
	return grid[index].grid_name;
}


/*! \brief Get login URL of nth grid
 */
const char *GridList::url(int index)
{
	return grid[index].grid_url;
}


/*! \brief Search the list for a matching name and return the index.
 * \param name[in] String matching the name of the grid.
 * \sa name, url
 * \return Index matching the grid entry, or -1 if not found.
 */
const int GridList::find( const char* _name )
{
	const int cnt = count();
	std::string name( _name );
	int ret_index = -1;
	//
	for( int index = 0; index < cnt; ++index )
	{
		if( std::string(grid[index].grid_name) == name )
		{
			ret_index = index;
			break;
		}
	}

	return ret_index;
}


}
//namespace LLC

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
