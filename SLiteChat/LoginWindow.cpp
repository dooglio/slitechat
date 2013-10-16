/** 
 * \brief Methods for the LoginWindow class.
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

// LLChatLib
//
#include "LLChatLib.h"
#include "GridList.h"
#include <QSettings>

// Local
//
#include "LoginWindow.h"
#include "MessageDialog.h"

// Qt4
//
#include <QPushButton>
#include <QSettings>

#include "Common.h"

// For debugging
//
#include <iostream>

LoginWindow::LoginWindow( QWidget* parent )
	: QDialog(parent)
    , m_ui(new Ui_LoginWindow)
	, m_loading(true)
	, m_highlighting(false)
	, m_savePasswords(false)
{
	m_ui->setupUi(this);
	m_ui->m_fullNameCombo->setFocus( Qt::OtherFocusReason );

	// Insert grids into grid list
	//
	QStringList grids;
	//
	const int gridCount = LLC::GridList::number();
	for (int i = 0; i < gridCount; i++)
	{
		grids.append( LLC::GridList::name(i) );
	}
	//
	m_ui->m_gridEdit->insertItems( 0, grids );

	// Update button and other UI settings
	//
	OnTextChanged( "" );

	setFocusPolicy( Qt::StrongFocus );

	QObject::connect	( QApplication::instance()
						, SIGNAL(focusChanged(QWidget*,QWidget*))
						, this
						, SLOT(OnFocusChanged(QWidget*,QWidget*))
						);
}

LoginWindow::~LoginWindow()
{
	QObject::disconnect	( QApplication::instance()
						, SIGNAL(focusChanged(QWidget*,QWidget*))
						, this
						, SLOT(OnFocusChanged(QWidget*,QWidget*))
						);

    delete m_ui;
}


void LoginWindow::ResetSimList()
{
	m_ui->m_slurlEdit->clear();

	// This array is empty--must be the first time, so let's load it up with
	// the two entries we always have to have...
	//
	QIcon icon_home;
	icon_home.addFile(QString::fromUtf8(":/mainIcon/home_icon"), QSize(), QIcon::Normal, QIcon::Off);
	m_ui->m_slurlEdit->addItem( icon_home, QString() );
	//
	QIcon icon_last;
	icon_last.addFile(QString::fromUtf8(":/mainIcon/main_icon"), QSize(), QIcon::Normal, QIcon::Off);
	m_ui->m_slurlEdit->addItem( icon_last, QString() );
	//
	m_ui->m_slurlEdit->setItemText(0, QApplication::translate("LoginWindow", "Home", 0, QApplication::UnicodeUTF8));
	m_ui->m_slurlEdit->setItemText(1, QApplication::translate("LoginWindow", "My Last Location", 0, QApplication::UnicodeUTF8));
}


void LoginWindow::LoadSimList( const int gridId )
{
	// Retrieve stored SIM SLURLs and populate the sim combobox
	//
	QString gridName( LLC::GridList::name( gridId ) );
	//
	QSettings settings;
	settings.beginGroup( gridName );
	//
	const int count = settings.beginReadArray( SIMS_SECTION );
	//
	if( count == 0 )
	{
		ResetSimList();
	}
	else
	{
		m_ui->m_slurlEdit->clear();
		//
		for( int idx = 0; idx < count; ++idx )
		{
			settings.setArrayIndex( idx );
			m_ui->m_slurlEdit->addItem( settings.value( SIM_NAME ).toString() );
		}
	}
	//
	settings.endArray();
	settings.endGroup();
}


void LoginWindow::SaveSimList( const int gridId )
{
	// Save the SLURLS just in case they were edited
	//
	QSettings settings;
	QString gridName( LLC::GridList::name( gridId ) );
	//
	settings.beginGroup( gridName );
	//
	settings.beginWriteArray( SIMS_SECTION );
	//
	const int count = m_ui->m_slurlEdit->count();
	for( int idx = 0; idx < count; ++idx )
	{
		settings.setArrayIndex( idx );
		settings.setValue( SIM_NAME, m_ui->m_slurlEdit->itemText( idx ) );
	}
	//
	settings.endArray();
	settings.endGroup();
}


void LoginWindow::LoadAVList( const int gridId )
{
	// Retrieve stored SIM SLURLs and populate the sim combobox
	//
	QString gridName( LLC::GridList::name( gridId ) );
	//
	QSettings settings;
	settings.beginGroup( gridName );

	// Read avs in first
	//
	const int count = settings.beginReadArray( AVS_SECTION );
	m_ui->m_fullNameCombo->clear();
	m_avLoginMap.clear();
	std::vector<QString> nameList;
	//
	for( int idx = 0; idx < count; ++idx )
	{
		settings.setArrayIndex( idx );
		const QString name = settings.value( AV_NAME ).toString();
		m_ui->m_fullNameCombo->addItem( name );
		nameList.push_back( name );
	}
	//
	settings.endArray();

	// Now grab the associated passwords and add those as data to each item
	//
	const int pwdCount = settings.beginReadArray( PWDS_SECTION );
	//
	for( int idx = 0; idx < pwdCount; ++idx )
	{
		settings.setArrayIndex( idx );
		const QString password = settings.value( PWD_NAME ).toString();
		m_avLoginMap[nameList[idx]] = password;
	}
	//
	settings.endArray();

	settings.endGroup();
}


void LoginWindow::SaveAVList( const int gridId )
{
	// Save the SLURLS just in case they were edited
	//
	QSettings settings;
	QString gridName( LLC::GridList::name( gridId ) );
	//
	settings.beginGroup( gridName );

	settings.beginWriteArray( AVS_SECTION );
	//
	std::vector<QString> nameList;
	const int count = m_ui->m_fullNameCombo->count();
	for( int idx = 0; idx < count; ++idx )
	{
		settings.setArrayIndex( idx );
		const QString name = m_ui->m_fullNameCombo->itemText( idx );
		settings.setValue( AV_NAME, name );
		nameList.push_back( name );
	}
	//
	settings.endArray();

	settings.beginWriteArray( PWDS_SECTION );
	//
	const size_t pwdCount = nameList.size();
	for( size_t idx = 0; idx < pwdCount; ++idx )
	{
		settings.setArrayIndex( idx );
		settings.setValue( PWD_NAME, m_avLoginMap[nameList[idx]] );
	}
	//
	settings.endArray();

	settings.endGroup();
}


void LoginWindow::SetLoginInfo( const LoginInfo& info )
{
	m_loginInfo = info;

	// Retrieve stored SIM SLURLs and populate the sim combobox
	//
	LoadSimList( m_loginInfo.m_grid );

	// Now retrieve the stored AV list (AVs that logged in from this application)
	//
	LoadAVList( m_loginInfo.m_grid );

	// Update the dialog
	//
	{
		QString fullName( m_loginInfo.m_firstName + " " + m_loginInfo.m_lastName );
		const int pos = m_ui->m_fullNameCombo->findText( fullName );
		if( pos == -1 )
		{
			m_ui->m_fullNameCombo->addItem( fullName );
		}
		else
		{
			m_ui->m_fullNameCombo->setCurrentIndex( pos );
		}
	}
	//
    if( info.m_savePassword )
    {
	    m_ui->m_passwordEdit->setText( m_loginInfo.m_mungedPassword );
		m_savePasswords = true;
    }
	//
	QString slurl = m_loginInfo.m_slurl;
	//
	if( slurl == "home" )
	{
		slurl = tr("Home");
	}
	else if( slurl == "last" )
	{
		slurl = tr("My Last Location");
	}
	//
	{
		const int pos = m_ui->m_slurlEdit->findText( slurl );
		if( pos != -1 )
		{
			m_ui->m_slurlEdit->setCurrentIndex( pos );
		}
	}
	//
	m_ui->m_gridEdit->setCurrentIndex( m_loginInfo.m_grid  );
	//
	m_ui->m_savePasswordCheckBox->setCheckState( m_loginInfo.m_savePassword
		? Qt::Checked
		: Qt::Unchecked
		);

	if( m_loginInfo.m_firstName.isEmpty() && m_loginInfo.m_lastName.isEmpty() )
	{
		m_ui->m_fullNameCombo->setFocus( Qt::OtherFocusReason );
	}
	else
	{
		m_ui->m_passwordEdit->setFocus( Qt::OtherFocusReason );
	}

	OnTextChanged( QString() );

	m_loading = false;
}


void LoginWindow::OnAccepted()
{
	const QString fullName		= m_ui->m_fullNameCombo->currentText().trimmed();	// Trim leading and trialing spaces
	const int space				= fullName.indexOf( " " );
	//
	m_loginInfo.m_firstName		= fullName.left( space );
	m_loginInfo.m_lastName		= fullName.mid ( space + 1 );
	m_loginInfo.m_savePassword	= m_ui->m_savePasswordCheckBox->checkState() == Qt::Checked;
	m_loginInfo.m_slurl			= m_ui->m_slurlEdit->currentText();
	m_loginInfo.m_grid			= m_ui->m_gridEdit->currentIndex();

	if( m_loginInfo.m_slurl == tr("Home") )
	{
		m_loginInfo.m_slurl = "home";
	}
	else if( m_loginInfo.m_slurl == tr("My Last Location") )
	{
		m_loginInfo.m_slurl = "last";
	}
	else
	{
		if( m_ui->m_slurlEdit->findText( m_loginInfo.m_slurl ) == -1 )
		{
			m_ui->m_slurlEdit->addItem( m_loginInfo.m_slurl );
		}
	}

	m_loginInfo.m_mungedPassword = m_avLoginMap[fullName];

	// Save the SLURLS and AVs just in case they were edited
	//
	SaveSimList( m_loginInfo.m_grid );
	SaveAVList ( m_loginInfo.m_grid );

	// Stop the dialog
	//
	accept();
}


void LoginWindow::OnResetSimLandmarks()
{
	if( MessageDialog::Question	( this
								, tr("Warning")
								, tr("Are you sure you want to clear out your list of SIM Landmarks? This cannot be undone!")
								) )
	{
		ResetSimList();
	}
}


void LoginWindow::OnResetFullNames()
{
	if( MessageDialog::Question	( this
								, tr("Warning")
								, tr("Are you sure you want to clear out your list of AV names and passwords? This cannot be undone!")
								)
		)
	{
		m_ui->m_fullNameCombo->clear();
		m_ui->m_passwordEdit ->clear();
		m_avLoginMap.clear();
		//
		SaveAVList ( m_loginInfo.m_grid );
	}
}


void LoginWindow::OnSavePasswordChecked( bool checked )
{
	m_savePasswords = checked;
}


void LoginWindow::OnTextChanged( QString )
{
	const QString fullName	 =  m_ui->m_fullNameCombo->currentText();
	const bool enable_button =  !fullName.isEmpty()
							 && (fullName.indexOf( " " ) != -1)
							 && !m_ui->m_passwordEdit->text().isEmpty();
	
	//enable_button = !m_ui->m_slurlEdit->currentText().isEmpty();

	QPushButton* okButton = m_ui->m_buttonBox->button( QDialogButtonBox::Ok );
	okButton->setEnabled( enable_button );
}


void LoginWindow::OnFullNameChanged( QString fullName )
{
	OnTextChanged( fullName );
}

void LoginWindow::OnFullNameChanged( int item )
{
	if( m_savePasswords )
	{
		QString fullName = m_ui->m_fullNameCombo->currentText();
		QString password = m_avLoginMap[fullName];
		m_ui->m_passwordEdit->setText( password );
	}
}


void LoginWindow::OnPasswordChanged( QString password )
{
	OnTextChanged( password );
	//
	QString full_name ( m_ui->m_fullNameCombo->currentText() );
	QString munged_pwd( LLC::MungedPassword( password.toAscii().data() ).GetString() );
	m_avLoginMap[full_name] = munged_pwd;
}


void LoginWindow::OnGridChanged( int item )
{
	if( m_loading == false )
	{
		LoadSimList( item );
		LoadAVList ( item );
		//
		if( m_savePasswords )
		{
			QString full_name ( m_ui->m_fullNameCombo->currentText() );
			m_ui->m_passwordEdit->setText( m_avLoginMap[full_name] );
		}
		else
		{
			m_ui->m_passwordEdit->setText( QString() );
		}
	}
}


void LoginWindow::OnFocusChanged( QWidget *old, QWidget *now )
{
	if( old == m_ui->m_fullNameCombo )
	{
		QString fullName = m_ui->m_fullNameCombo->currentText();
		if( m_ui->m_fullNameCombo->findText( fullName ) == -1 )
		{
			m_ui->m_fullNameCombo->addItem( fullName );
		}
	}
}


// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
