/** 
 * \brief Function declarations for the Common namespace
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

#include "Common.h"
#include "Utility.h"

#include <QObject>

namespace Common
{

namespace
{
	QStringMap	g_langMap;
	//
	void CreateLanguageMap()
	{
		if( !g_langMap.empty() )
		{
			return;
		}
		//
		g_langMap["???"		] = QObject::tr("???");
		g_langMap["al"		] = QObject::tr("Albanian"); 
		g_langMap["am"		] = QObject::tr("Amharic");         
		g_langMap["af"		] = QObject::tr("Afrikaans");      
		g_langMap["hy"		] = QObject::tr("Armenian");      
		g_langMap["ar"		] = QObject::tr("Arabic");
		g_langMap["az"		] = QObject::tr("Azerbaijania"); 
		g_langMap["eu"		] = QObject::tr("Basque");
		g_langMap["be"		] = QObject::tr("Belarusian");
		g_langMap["bn"		] = QObject::tr("Bengali");
		g_langMap["bh"		] = QObject::tr("Bihari");
		g_langMap["bg"		] = QObject::tr("Bulgarian");
		g_langMap["my"		] = QObject::tr("Burmese");
		g_langMap["ca"		] = QObject::tr("Catalan");
		g_langMap["chr"		] = QObject::tr("Cherokee");
		g_langMap["zh-CN"	] = QObject::tr("Chinese");
		g_langMap["zh-TW"	] = QObject::tr("Taiwan");
		g_langMap["hr"		] = QObject::tr("Croatian");
		g_langMap["cs"		] = QObject::tr("Czech");
		g_langMap["da"		] = QObject::tr("Danish");
		g_langMap["nl"		] = QObject::tr("Dutch");
		g_langMap["en"		] = QObject::tr("English");
		g_langMap["dv"		] = QObject::tr("Dhiveli");
		g_langMap["eo"		] = QObject::tr("Esperanto");
		g_langMap["et"		] = QObject::tr("Estonian");
		g_langMap["fo"		] = QObject::tr("Faeroese");
		g_langMap["fa"		] = QObject::tr("Farsi");
		g_langMap["tl"		] = QObject::tr("Filipino");
		g_langMap["fi"		] = QObject::tr("Finnish"); 
		g_langMap["fr"		] = QObject::tr("French");
		g_langMap["gd"		] = QObject::tr("Gaelic");
		g_langMap["gl"		] = QObject::tr("Galician");
		g_langMap["ka"		] = QObject::tr("Georgian");
		g_langMap["de"		] = QObject::tr("German");
		g_langMap["el"		] = QObject::tr("Greek");     
		g_langMap["gn"		] = QObject::tr("Guarani");   
		g_langMap["gu"		] = QObject::tr("Guajarati"); 
		g_langMap["iw"		] = QObject::tr("Hebrew");
		g_langMap["hi"		] = QObject::tr("Hindi");     
		g_langMap["hu"		] = QObject::tr("Hungarian"); 
		g_langMap["is"		] = QObject::tr("Icelandic"); 
		g_langMap["id"		] = QObject::tr("Indonesian");
		g_langMap["iu"		] = QObject::tr("Inuktitut"); 
		g_langMap["it"		] = QObject::tr("Italian");   
		g_langMap["ja"		] = QObject::tr("Japanese");  
		g_langMap["kn"		] = QObject::tr("Kannada");   
		g_langMap["kk"		] = QObject::tr("Kazakh");    
		g_langMap["km"		] = QObject::tr("Khmer");     
		g_langMap["ko"		] = QObject::tr("Korean");    
		g_langMap["ku"		] = QObject::tr("Kurdish");   
		g_langMap["ky"		] = QObject::tr("Kyrgyz");    
		g_langMap["lo"		] = QObject::tr("Laotian");   
		g_langMap["lv"		] = QObject::tr("Latvian");   
		g_langMap["lt"		] = QObject::tr("Lithuanian");
		g_langMap["mk"		] = QObject::tr("Macedonian");
		g_langMap["ms"		] = QObject::tr("Malay");     
		g_langMap["ml"		] = QObject::tr("Malayalam"); 
		g_langMap["mt"		] = QObject::tr("Maltese");   
		g_langMap["mr"		] = QObject::tr("Marathi");   
		g_langMap["mn"		] = QObject::tr("Mongolian"); 
		g_langMap["ne"		] = QObject::tr("Nepali");    
		g_langMap["no"		] = QObject::tr("Norwegian");
		g_langMap["or"		] = QObject::tr("Oriya");
		g_langMap["ps"		] = QObject::tr("Pashto");
		g_langMap["pl"		] = QObject::tr("Polish");
		g_langMap["pt-PT"	] = QObject::tr("Portugese");
		g_langMap["pa"		] = QObject::tr("Punjabi");
		g_langMap["ro"		] = QObject::tr("Romanian");
		g_langMap["rm"		] = QObject::tr("Rhaeto-Romanic");
		g_langMap["ru"		] = QObject::tr("Russian");  
		g_langMap["sa"		] = QObject::tr("Sanskrit");
		g_langMap["sr"		] = QObject::tr("Serbian");  
		g_langMap["sb"		] = QObject::tr("Sorbian");
		g_langMap["sd"		] = QObject::tr("Sindhi");
		g_langMap["si"		] = QObject::tr("SinHalese");
		g_langMap["sk"		] = QObject::tr("Slovak");   
		g_langMap["sl"		] = QObject::tr("Slovenian");
		g_langMap["es"		] = QObject::tr("Spanish");  
		g_langMap["sw"		] = QObject::tr("Swahili");
		g_langMap["sv"		] = QObject::tr("Swedish");
		g_langMap["sv-fi"	] = QObject::tr("Finland");  
		g_langMap["sx"		] = QObject::tr("Sutu");            
		g_langMap["sz"		] = QObject::tr("Sami (Lappish)");  
		g_langMap["tg"		] = QObject::tr("Tajik");           
		g_langMap["ta"		] = QObject::tr("Tamil");           
		g_langMap["tl"		] = QObject::tr("Tagalog");         
		g_langMap["ts"		] = QObject::tr("Tsonga");          
		g_langMap["te"		] = QObject::tr("Telugu");          
		g_langMap["th"		] = QObject::tr("Thai");            
		g_langMap["tn"		] = QObject::tr("Tswana");          
		g_langMap["bo"		] = QObject::tr("Tibetan");         
		g_langMap["tr"		] = QObject::tr("Turkish");         
		g_langMap["ug"		] = QObject::tr("Uighur");          
		g_langMap["uk"		] = QObject::tr("Ukranian");        
		g_langMap["ur"		] = QObject::tr("Urdu");            
		g_langMap["uz"		] = QObject::tr("Uzbek");           
		g_langMap["ve"		] = QObject::tr("Venda");         
		g_langMap["vi"		] = QObject::tr("Vietnamese");
		g_langMap["ji"		] = QObject::tr("Yiddish");
		g_langMap["zu"		] = QObject::tr("Zulu");
	}


	// Main application sets this
	//
	bool g_isOnline = false;
}
// namespace


/** \brief Get the language map from language code to full language name.
 *
 * \return Full name language map.
 */
QStringMap GetLanguageMap()
{
	CreateLanguageMap();
	return g_langMap;
}


/** \brief Convert language abbrievation to full language name.
 *
 * \param [in,out] abbr	The abbrievation in can be different when we talk to google, so this value may change.
 *
 * \copyright Ferd Frederix. Used with permission.
 *
 * \return Full language name.
 *
 */
QString ConvertLanguage( QString& abbr )
{
	CreateLanguageMap();

	// google output conversion:
	// http://code.google.com/apis/ajaxlanguage/documentation/reference.html#LangNameArray

	// Windows detection
	// http://msdn.microsoft.com/en-us/library/ms533052(VS.85).aspx

	// Jira
	// https://jira.secondlife.com/browse/VWR-12222


	// detect means it comes from the PC or the viewer or google.

	abbr = abbr.toLower();
	QString new_abbr( abbr );    // stash the original, we will override it in some case

	// Now look up the language name by abbrievation
	//
	if(abbr=="al" || abbr=="sq" )      
	{ 
		new_abbr = "al"; 
	}
	else if(abbr=="ar" ||
		abbr =="ar-sa" ||
		abbr =="ar-iq" ||
		abbr =="ar-eg" ||
		abbr =="ar-ly" ||
		abbr =="ar-dz" ||
		abbr =="ar-ma" ||
		abbr =="ar-tn" ||
		abbr =="ar-om" ||
		abbr =="ar-ye" ||
		abbr =="ar-sy" ||
		abbr =="ar-jo" ||
		abbr =="ar-lb" ||
		abbr =="ar-kw" ||
		abbr =="ar-ae" ||
		abbr =="ar-bh" ||
		abbr =="ar-qa")
	{
		new_abbr = "ar" ;
	}
	else if(abbr=="zh" || abbr=="zh-CN" || abbr=="zh-cn" || abbr=="zh-hk") 
	{
		new_abbr = "zh-CN";
	} 
	else if(abbr=="nl" || abbr=="nl-be") 
	{
		new_abbr = "nl"; 
	}
	else if( abbr == "en-us--multiple" ||
		abbr == "en--multiple" ||
		abbr == "en-us" ||
		abbr == "en-gb" ||
		abbr == "en-au" ||
		abbr == "en-ca" ||
		abbr == "en-nz" ||
		abbr == "en-ie" ||
		abbr == "en-za" ||
		abbr == "en-jm" ||
		abbr == "en-bz" ||
		abbr == "en-tt"  ||
		abbr == "en"     ||
		abbr == ""
		)
	{
		new_abbr = "en";
	}
	else if(abbr=="fr" ||
		abbr=="fr-be" ||
		abbr=="fr-ca" ||
		abbr=="fr-ch" ||
		abbr=="fr-lu"
		)
	{
		new_abbr = "fr";
	}
	else if(abbr=="gd" || abbr=="gd-ie") 
	{
		new_abbr = "gd";
	}       // detect
	else if(abbr=="de" ||
		abbr=="de-ch" ||
		abbr=="de-at" ||
		abbr=="de-lu" ||
		abbr=="de-li"
		)
	{
		new_abbr = "de";
	}
	else if(abbr=="iw" || abbr=="he" )
	{
		new_abbr = "iw";
	}
	else if(abbr=="it" || abbr=="it-ch")
	{
		new_abbr = "it";
	}
	else if(abbr=="nb" || abbr == "no" )
	{
		new_abbr = "no";
	}
	else if(abbr=="pt-pt" ||
		abbr == "pt" ||
		abbr == "pt-br"
		)
	{
		new_abbr = "pt-PT";
	}
	else if(abbr=="ro" || abbr=="ro-mo")
	{
		new_abbr = "ro";
	}
	else if(abbr=="ru" || abbr=="ru-mo")
	{
		new_abbr = "ru";
	}
	else if(abbr=="es" ||
		abbr=="es-mx" ||
		abbr=="es-gt" ||
		abbr=="es-cr" ||
		abbr=="es-pa" ||
		abbr=="es-do" ||
		abbr=="es-ve" ||
		abbr=="es-co" ||
		abbr=="es-pe" ||
		abbr=="es-ar" ||
		abbr=="es-ec" ||
		abbr=="es-cl" ||
		abbr=="es-uy" ||
		abbr=="es-py" ||
		abbr=="es-bo" ||
		abbr=="es-sv" ||
		abbr=="es-hn" ||
		abbr=="es-ni" ||
		abbr=="es-pr"
		)
	{
		new_abbr = "es";
	}
	else if(abbr=="sv-fi")
	{
		new_abbr = "sv";
	}

	// Now assign the potentially modified abbrviation back to the in/out parameter
	//
	abbr = new_abbr;

	// Return the full language name
	//
	return g_langMap[abbr];
}


bool IsOnline()
{
	return g_isOnline;
}


void IsOnline( const bool var )
{
	g_isOnline = var;
}


}
// namespace Common

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
