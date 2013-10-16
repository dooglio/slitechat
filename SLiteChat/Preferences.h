/** 
 * \brief Header for Preferences UI class
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

#ifndef PREFERENCES_H__
#define PREFERENCES_H__

#include <QDialog>

#include "ui_Preferences.h"

class Preferences
	: public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(Preferences)

public:
	explicit Preferences( QWidget *parent = 0 );
	virtual ~Preferences();

	QFont GetFont() { return m_font; }

private:
    Ui_Preferences*			m_ui;
	QFont					m_font;
	std::vector<QString>	m_langIdList;

	void SetFontString();
	void FillLanguageList();

private slots:
	void OnAccepted();
	void OnFontButtonClicked();
};

#endif // PREFERENCES_H__

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
