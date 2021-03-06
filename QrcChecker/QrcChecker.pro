# Project file for QrcChecker
#
QT       += core gui widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG(debug, debug|release) {
	OBJECTS_DIR = debug/obj
	MOC_DIR		= debug/moc
	UI_DIR		= debug/ui
	DESTDIR		= ../bin/debug
}
else {
	OBJECTS_DIR = release/obj
	MOC_DIR		= release/moc
	UI_DIR		= release/ui
	DESTDIR		= ../bin/release
}


SOURCES += \
	src/AboutDialog.cpp \
	src/QrcChecker.cpp \
	src/main.cpp

HEADERS += \
	src/AboutDialog.h \
	src/QrcChecker.h

FORMS += \
	src/AboutDialog.ui \
	src/QrcChecker.ui

TRANSLATIONS += \
	resources/translations/QrcChecker_de.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
	resources/QrcChecker.qrc
