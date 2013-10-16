/** 
 * \brief Header for ChatWindow, a GUI dialog for inputing login credentials
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

#ifndef __LOGINWINDOW_H__
#define __LOGINWINDOW_H__

#include <QDialog>

#include "ui_LoginWindow.h"

class LoginWindow
	: public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(LoginWindow)

public:
	explicit LoginWindow( QWidget *parent = 0 );
	virtual ~LoginWindow();

	struct LoginInfo
	{
		LoginInfo() : m_savePassword(false), m_grid(-1) {}

		QString		m_firstName;
		QString		m_lastName;
		QString		m_mungedPassword;
		QString		m_slurl;
		int			m_grid;
		bool		m_savePassword;
	};

	LoginInfo	GetLoginInfo() const					{ return m_loginInfo; }
	void		SetLoginInfo( const LoginInfo& info );	//{ m_loginInfo = info; }

private:
    Ui_LoginWindow*	m_ui;
	LoginInfo		m_loginInfo;
	bool			m_loading;
	bool			m_highlighting;
	bool			m_savePasswords;

	typedef std::map<QString,QString> StringToString;
	StringToString	m_avLoginMap;

	void ResetSimList();
	void LoadSimList ( const int gridId );
	void SaveSimList ( const int gridId );

	void LoadAVList  ( const int gridId );
	void SaveAVList  ( const int gridId );

private slots:
	void OnAccepted();
	void OnSavePasswordChecked( bool );
	void OnTextChanged( QString );
	void OnPasswordChanged( QString );
	void OnResetSimLandmarks();
	void OnResetFullNames();
	void OnGridChanged( int );
	void OnFullNameChanged( int );
	void OnFullNameChanged( QString );
	void OnFocusChanged( QWidget *old, QWidget *now );
};

#endif // __LOGINWINDOW_H__

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
