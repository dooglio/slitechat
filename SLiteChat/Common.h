#ifndef COMMON_H__
#define COMMON_H__

#define GUI_SECTION         "MainWindow"
#define SIMS_SECTION        "Sims"
#define SIM_NAME            "SimName"
#define AVS_SECTION         "AVs"
#define AV_NAME             "AVName"
#define PWDS_SECTION        "Passwords"
#define PWD_NAME            "Password"
#define FIRSTNAME           "FirstName"
#define LASTNAME            "LastName"
#define MUNGEDPASSWORD      "MungedPassword"
#define SAVEPASSWORD        "SavePassword"
#define LASTGRID            "LastGridVisited"
#define LASTSLURL           "LastSlurlVisited"
#define SASHPOS             "SashPosition"
#define GROUPSASHPOS        "GroupSashPosition"
#define WINDOWGEOMETRY      "WindowGeometry"
#define WINDOWSTATE         "WindowState"
#define GUIFONT             "GuiFont"
#define PERSISTIM           "PersistIMHistory"
#define DEFAULT_PIM         true
#define PERSISTLC           "PersistLocalChatHistory"
#define DEFAULT_PLC         false
#define TIMESTAMPS          "TimestampConversations"
#define DEFAULT_TIMEST      true
#define IGNORE_GROUP_IMS    "IgnoreGroupIMs"
#define DEFAULT_IGNOREGRP	false
#define LOCAL_LANGUAGE      "LocalLanguage"
#define DEFAULT_LL          0
#define LOCAL_LANGUAGE_CODE "LocalLanguageCode"
#define DEFAULT_LLC         "sys"
#define GOOGLE_XLATE        "GoogleXlate"
#define DEFAULT_GX          "off"
#define ECHO_SOURCE         "EchoSource"
#define DEFAULT_ES          "on"

#define MESSAGE_TIMEOUT_MSECS	10

#include <QString>
#include <map>

namespace Common
{
	typedef std::map<QString,QString> QStringMap;
	QStringMap	GetLanguageMap();
	QString		ConvertLanguage( QString& abbr );

	bool		IsOnline();
	void		IsOnline( const bool var );
}
// namespace Common

#endif // COMMON_H__
