/** 
 * \brief Methods for the Preferences class.
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

#include "LLChatLib.h"

#include "Preferences.h"
#include "Common.h"
#include "Utility.h"

#include <QPushButton>
#include <QSettings>
#include <QFontDialog>

Preferences::Preferences( QWidget* parent )
	: QDialog(parent)
    , m_ui(new Ui_Preferences)
{
	 m_ui->setupUi(this);

	FillLanguageList();

	QSettings settings;
	settings.beginGroup( GUI_SECTION );
	m_font.fromString( settings.value( GUIFONT, font().toString() ).toString() );
	SetFontString();
	settings.endGroup();
	//
	LLC::Manager llmgr;
	m_ui->m_persistImHistory       ->setChecked      ( llmgr.GetBool( PERSISTIM        ) );
	m_ui->m_persistLcHistory       ->setChecked      ( llmgr.GetBool( PERSISTLC        ) );
	m_ui->m_showTimestamps         ->setChecked      ( llmgr.GetBool( TIMESTAMPS       ) );
	m_ui->m_ignoreAllGroupCheckBox ->setChecked      ( llmgr.GetBool( IGNORE_GROUP_IMS ) );
	m_ui->m_localLanguage          ->setCurrentIndex ( llmgr.GetUInt( LOCAL_LANGUAGE   ) );
	m_ui->m_enableGoogleTranslate  ->setChecked      ( llmgr.GetBool( GOOGLE_XLATE     ) );
	m_ui->m_echoSourceLanguage     ->setChecked      ( llmgr.GetBool( ECHO_SOURCE      ) );
}


Preferences::~Preferences()
{
    delete m_ui;
}


void Preferences::SetFontString()
{
	QString name = tr("%1, %2").arg(m_font.family()).arg(m_font.pointSize());
	m_ui->m_fontEditBox->setText( name );
}

void Preferences::OnAccepted()
{
	// Persist settings
	//
	QSettings settings;
	settings.beginGroup( GUI_SECTION );
	QString fontName = m_font.toString();
	settings.setValue( GUIFONT			, fontName );
	settings.endGroup();
	settings.sync();
	//
	LLC::Manager llmgr;
	const int langIdx = m_ui->m_localLanguage->currentIndex();
	const QString lang_code = m_langIdList[langIdx];
	llmgr.SetBool   ( PERSISTIM           , m_ui->m_persistImHistory       ->isChecked () );
	llmgr.SetBool   ( PERSISTLC           , m_ui->m_persistLcHistory       ->isChecked () );
	llmgr.SetBool   ( TIMESTAMPS          , m_ui->m_showTimestamps         ->isChecked () );
	llmgr.SetBool   ( IGNORE_GROUP_IMS    , m_ui->m_ignoreAllGroupCheckBox ->isChecked () );
	llmgr.SetUInt   ( LOCAL_LANGUAGE      , langIdx                        );
	llmgr.SetString ( LOCAL_LANGUAGE_CODE , Q2LS(lang_code)                );
	llmgr.SetBool   ( GOOGLE_XLATE        , m_ui->m_enableGoogleTranslate  ->isChecked () );
	llmgr.SetBool   ( ECHO_SOURCE         , m_ui->m_echoSourceLanguage     ->isChecked () );
	//
	//
	accept();

	// Set into LLChatLib
	//
	LLC::Manager mgr;
	mgr.SetTranslateMessages( m_ui->m_enableGoogleTranslate->isChecked() );
	mgr.SetLanguage( lang_code == "sys"? Q2LS(GetSystemLangCode()): Q2LS(lang_code) );
}


void Preferences::OnFontButtonClicked()
{
	bool ok;
	QFont font = QFontDialog::getFont( &ok, m_font );
	//
	if( ok )
	{
		m_font = font;
		SetFontString();
	}
}


void Preferences::FillLanguageList()
{
	Common::QStringMap	supported_langs = Common::GetLanguageMap();
	//
	Common::QStringMap::iterator		iter = supported_langs.begin();
	const Common::QStringMap::iterator	end  = supported_langs.end();
	//
	m_langIdList.push_back( "sys" );
	//
	for( ; iter != end; ++iter )
	{
		QString code = iter->first;
		QString lang = iter->second;
		//
		if( code != "???" )
		{
			m_ui->m_localLanguage->addItem( lang, code );
			m_langIdList.push_back( code );
		}
	}
}

	
// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
