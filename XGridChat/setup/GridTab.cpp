/** 
 * \brief A tab describing a grid for XGridChat
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
#include "Tester.h"
#include "Utility.h"
#include "GridTab.h"

#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>

#include <stdio.h>

GridTab::GridTab(int tabnum, QWidget *parent)
	: QWidget(parent),
	  m_botFirst(false), m_botLast(false), m_botPassword(false),
	  m_location(true),
	  m_ownerFirst(false), m_ownerLast(false), m_ownerID(false),
	  m_groupName(false), m_groupID(false),
	  m_address(true), m_port(true),
	  m_botFirstEdit(NULL), m_botLastEdit(NULL), m_botPasswordEdit(NULL),
	  m_gridURLValue(NULL), m_locationCombo(NULL),
	  m_ownerFirstEdit(NULL), m_ownerLastEdit(NULL), m_groupNameEdit(NULL),
	  m_testButton(NULL),
	  m_testInProgress(NULL)
{
	QVBoxLayout *mainLayout = new QVBoxLayout;
	QFormLayout *formLayout = new QFormLayout;
	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	mainLayout->addLayout(formLayout);
	mainLayout->addLayout(buttonsLayout);

	QLabel *botFirstLabel = new QLabel("Robot's first name");
	m_botFirstEdit = new QLineEdit;
	QObject::connect(m_botFirstEdit, SIGNAL(textChanged(const QString &)),
					 this, SLOT(onBotFirstTextChanged(const QString &)));
	formLayout->addRow(botFirstLabel, m_botFirstEdit);

	QLabel *botLastLabel = new QLabel("Robot's last name");
	m_botLastEdit = new QLineEdit;
	QObject::connect(m_botLastEdit, SIGNAL(textChanged(const QString &)),
					 this, SLOT(onBotLastTextChanged(const QString &)));
	formLayout->addRow(botLastLabel, m_botLastEdit);

	QLabel *botPasswordLabel = new QLabel("Robot's password");
	m_botPasswordEdit = new QLineEdit;
	m_botPasswordEdit->setEchoMode(QLineEdit::Password);
	QObject::connect(m_botPasswordEdit, SIGNAL(textChanged(const QString &)),
					 this, SLOT(onBotPasswordTextChanged(const QString &)));
	formLayout->addRow(botPasswordLabel, m_botPasswordEdit);

	QLabel *gridLabel = new QLabel("Grid");
	QComboBox *gridCombo = new QComboBox;
	QStringList grids;
	for (int i = 0; i < LLC::GridList::number(); i++)
		grids.append(LLC::GridList::name(i));
	gridCombo->insertItems(0, grids);
	gridCombo->setCurrentIndex(tabnum ? 12 : 14);
	QObject::connect(gridCombo, SIGNAL(currentIndexChanged(int)),
					 this, SLOT(onGridComboIndexChanged(int)));
	formLayout->addRow(gridLabel, gridCombo);

	QLabel *gridURLLabel = new QLabel("Grid URL");
	m_gridURLValue = new QLineEdit(LLC::GridList::url(tabnum ? 12: 14));
	m_gridURLValue->setEnabled(false);
	formLayout->addRow(gridURLLabel, m_gridURLValue);

	QLabel *locationLabel = new QLabel("Inworld location");
	m_locationCombo = new QComboBox;
	QStringList locations;
	locations.append("last");
	locations.append("home");
	m_locationCombo->insertItems(0, locations);
	m_locationCombo->setEditable(true);
	m_locationCombo->setMinimumContentsLength(16);
	QObject::connect(m_locationCombo, SIGNAL(textChanged(const QString &)),
					 this, SLOT(onLocationTextChanged(const QString &)));
	formLayout->addRow(locationLabel, m_locationCombo);

	QLabel *ownerFirstLabel = new QLabel("Owner's first name");
	m_ownerFirstEdit = new QLineEdit;
	QObject::connect(m_ownerFirstEdit, SIGNAL(textChanged(const QString &)),
					 this, SLOT(onOwnerFirstTextChanged(const QString &)));
	formLayout->addRow(ownerFirstLabel, m_ownerFirstEdit);

	QLabel *ownerLastLabel = new QLabel("Owner's last name");
	m_ownerLastEdit = new QLineEdit;
	QObject::connect(m_ownerLastEdit, SIGNAL(textChanged(const QString &)),
					 this, SLOT(onOwnerLastTextChanged(const QString &)));
	formLayout->addRow(ownerLastLabel, m_ownerLastEdit);

	QLabel *ownerIDLabel = new QLabel("Owner's ID");
	m_ownerIDValue = new QLineEdit;
	m_ownerIDValue->setEnabled(false);
	formLayout->addRow(ownerIDLabel, m_ownerIDValue);

	QLabel *groupNameLabel = new QLabel("Inworld group's name");
	m_groupNameEdit = new QLineEdit;
	QObject::connect(m_groupNameEdit, SIGNAL(textChanged(const QString &)),
					 this, SLOT(onGroupNameTextChanged(const QString &)));
	formLayout->addRow(groupNameLabel, m_groupNameEdit);

	QLabel *groupIDLabel = new QLabel("Inworld group's ID");
	m_groupIDValue = new QLineEdit;
	m_groupIDValue->setEnabled(false);
	formLayout->addRow(groupIDLabel, m_groupIDValue);

	QLabel *prefixLabel = new QLabel("Messages prefix");
	QLineEdit *prefixEdit = new QLineEdit(tabnum ? "[OS] ": "[SL] ");
	formLayout->addRow(prefixLabel, prefixEdit);

	QLabel *addressLabel = new QLabel("IP address");
	QLineEdit *addressEdit = new QLineEdit("127.0.0.1");
	QObject::connect(addressEdit, SIGNAL(textChanged(const QString &)),
					 this, SLOT(onAddressTextChanged(const QString &)));
	formLayout->addRow(addressLabel, addressEdit);

	QLabel *portLabel = new QLabel("UDP port");
	char buffer[20];
	sprintf(buffer, "%d", 7777 + tabnum);
	QLineEdit *portEdit = new QLineEdit(buffer);
	QObject::connect(portEdit, SIGNAL(textChanged(const QString &)),
					 this, SLOT(onPortTextChanged(const QString &)));
	formLayout->addRow(portLabel, portEdit);

	QLabel *userLabel = new QLabel("System user");
	QLineEdit *userEdit = new QLineEdit("robot");
	formLayout->addRow(userLabel, userEdit);

	QPalette disabledPalette;
	disabledPalette.setColor(QPalette::ButtonText, QColor("gray"));

	m_testButton = new QPushButton("Test");
	m_testButton->setEnabled(false);
	m_testButton->setPalette(disabledPalette);
	QObject::connect(m_testButton, SIGNAL(clicked()),
					 this, SLOT(onTestButtonClicked()));
	buttonsLayout->addWidget(m_testButton);

	QPushButton *installButton = new QPushButton("Install");
	installButton->setEnabled(false);
	installButton->setPalette(disabledPalette);
	buttonsLayout->addWidget(installButton);

	setLayout(mainLayout); 
}


// Received a group name and id
// Returns true if that was the one we were interested in
bool GridTab::gotOwner(const char *id, const char *avatar_name)
{
	QString receivedName(avatar_name),
			expectedName(m_ownerFirstEdit->text() + " " + m_ownerLastEdit->text());

	if (receivedName == expectedName)
	{
		QString receivedID(id);

		m_ownerIDValue->setText(receivedID);
		m_ownerID = true;

		return true;
	}
	return false;
}

// Received a group name and id
// Returns true if that was the one we were interested in
bool GridTab::gotGroup(const char *id, const char *group_name)
{
	QString receivedName(group_name),
			expectedName(m_groupNameEdit->text());

	if (receivedName == expectedName)
	{
		QString receivedID(id);

		m_groupIDValue->setText(receivedID);
		m_groupID = true;

		return true;
	}
	return false;
}


// Update the grid URL each time the grid name changes
void GridTab::updateGridURLValue(int index)
{
	m_gridURLValue->setText(LLC::GridList::url(index));
}


// Reset the owner ID value each time the owner name changes
void GridTab::updateOwnerIDValue()
{
	m_ownerIDValue->setText("");
	m_ownerID = false;
}


// Reset the group ID value each time the group name changes
void GridTab::updateGroupIDValue()
{
	m_groupIDValue->setText("");
	m_groupID = false;
}


// Do a test to determine the owner and group ids
void GridTab::doTest()
{
	Tester tester(this);
	QPalette palette;

	m_testInProgress = &tester;
	m_testButton->setText("Stop test");

	if (!tester.authenticate(
		QStringToLLCString(m_gridURLValue->text()),
		QStringToLLCString(m_botFirstEdit->text()),
		QStringToLLCString(m_botLastEdit->text()),
	 	LLC::MungedPassword( QStringToLLCString(m_botPasswordEdit->text()) ),
		QStringToLLCString(m_locationCombo->currentText())
	)) tester.runTest();

	m_testButton->setText("Test");
	m_testInProgress = NULL;
}


// Make the "Test" button available only if all necessary data is entered
void GridTab::updateTestButton()
{
	bool oldTestEnabled = m_testButton->isEnabled(),
		 newTestEnabled = m_botFirst && m_botLast && m_botPassword &&
						  m_location &&
						  m_ownerFirst && m_ownerLast &&
						  m_groupName;

	if (oldTestEnabled != newTestEnabled)
	{
		QPalette palette;
		palette.setColor(QPalette::ButtonText, QColor(newTestEnabled ? "black": "gray"));
		m_testButton->setPalette(palette);
		m_testButton->setEnabled(newTestEnabled);
	}
}


void GridTab::onBotFirstTextChanged(const QString &text)
{
	m_botFirst = !text.isEmpty();
	updateOwnerIDValue();
	updateGroupIDValue();
	updateTestButton();
}


void GridTab::onBotLastTextChanged(const QString &text)
{
	m_botLast = !text.isEmpty();
	updateOwnerIDValue();
	updateGroupIDValue();
	updateTestButton();
}


void GridTab::onBotPasswordTextChanged(const QString &text)
{
	m_botPassword = !text.isEmpty();
	updateOwnerIDValue();
	updateGroupIDValue();
	updateTestButton();
}


void GridTab::onGridComboIndexChanged(int index)
{
	updateGridURLValue(index);
	updateOwnerIDValue();
	updateGroupIDValue();
}


void GridTab::onLocationTextChanged(const QString &text)
{
	m_location = !text.isEmpty();
	updateTestButton();
}


void GridTab::onOwnerFirstTextChanged(const QString &text)
{
	m_ownerFirst = !text.isEmpty();
	updateOwnerIDValue();
	updateTestButton();
}


void GridTab::onOwnerLastTextChanged(const QString &text)
{
	m_ownerLast = !text.isEmpty();
	updateOwnerIDValue();
	updateTestButton();
}


void GridTab::onGroupNameTextChanged(const QString &text)
{
	m_groupName = !text.isEmpty();
	updateGroupIDValue();
	updateTestButton();
}


void GridTab::onAddressTextChanged(const QString &text)
{
	m_address = !text.isEmpty();
}


void GridTab::onPortTextChanged(const QString &text)
{
	m_port = !text.isEmpty();
}


void GridTab::onTestButtonClicked()
{
	if (m_testInProgress == NULL)
		doTest();
	else
		m_testInProgress->stop();
}


// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
