# -*- cmake -*-

find_package( Qt4 4.5.0 REQUIRED QtCore QtGui QtNetwork )
set( QT_USE_QTMAIN TRUE )
set( QT_USE_QTNETWORK TRUE )
include( ${QT_USE_FILE} )
#
# I might not need these because of the QT_USE_FILE def above! :-)
#
add_definitions( ${QT_DEFINITIONS} )
