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

#ifndef GRIDTAB_H
#define GRIDTAB_H

#include <QWidget>

class Tester;

class QLineEdit;
class QComboBox;
class QPushButton;

class GridTab : public QWidget
{
	Q_OBJECT
	Q_DISABLE_COPY(GridTab)

public:
	GridTab(int tabnum, QWidget *parent = 0);
	inline bool testInProgress() const { return m_testInProgress != NULL; }
	bool gotOwner(const char *id, const char *avatar_name);
	bool gotGroup(const char *id, const char *group_name);
	inline bool groupIDKnown() const {return m_groupID;}
	inline bool ownerIDKnown() const {return m_ownerID;}

private:
	bool m_botFirst, m_botLast, m_botPassword,
		 m_location,
		 m_ownerFirst, m_ownerLast, m_ownerID,
		 m_groupName, m_groupID,
		 m_address, m_port;

	QLineEdit *m_botFirstEdit, *m_botLastEdit, *m_botPasswordEdit,
	          *m_gridURLValue;
	QComboBox *m_locationCombo;
	QLineEdit *m_ownerFirstEdit, *m_ownerLastEdit, *m_ownerIDValue,
			  *m_groupNameEdit, *m_groupIDValue;
	QPushButton *m_testButton; 

	Tester *m_testInProgress;

	void updateGridURLValue(int index);
	void updateOwnerIDValue();
	void updateGroupIDValue();
	void updateTestButton();
	void doTest();

private slots:
	void onBotFirstTextChanged(const QString &text);
	void onBotLastTextChanged(const QString &text);
	void onBotPasswordTextChanged(const QString &text);
	void onGridComboIndexChanged(int index);
	void onLocationTextChanged(const QString &text);
	void onOwnerFirstTextChanged(const QString &text);
	void onOwnerLastTextChanged(const QString &text);
	void onGroupNameTextChanged(const QString &text);
	void onAddressTextChanged(const QString &text);
	void onPortTextChanged(const QString &text);
	void onTestButtonClicked();
};


#endif

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
